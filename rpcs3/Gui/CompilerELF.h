#pragma once

#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/ElfLoader.h"
#include "wx/aui/aui.h"

static void Write8(wxFile& f, const u8 data)
{
	f.Write(&data, 1);
}
static void Write16(wxFile& f, const u16 data)
{
	Write8(f, data >> 8);
	Write8(f, data);
}
static void Write32(wxFile& f, const u32 data)
{
	Write16(f, data >> 16);
	Write16(f, data);
}
static void Write64(wxFile& f, const u64 data)
{
	Write32(f, data >> 32);
	Write32(f, data);
}

enum CompilerIDs
{
	id_analyze_code = 0x555,
	id_compile_code,
};

class CompilerELF : public wxFrame
{
	wxTextCtrl* asm_list;
	wxTextCtrl* hex_list;
	wxTextCtrl* err_list;

	wxTimer* scroll_timer;

	wxTextAttr* a_instr;
	wxAuiManager m_aui_mgr;

public:
	CompilerELF(wxWindow* parent) : wxFrame(parent, wxID_ANY, "CompilerELF")
	{
		m_aui_mgr.SetManagedWindow(this);

		wxMenuBar& menubar(*new wxMenuBar());
		wxMenu& menu_code(*new wxMenu());
		menubar.Append(&menu_code, "Code");

		menu_code.Append(id_analyze_code, "Analyze");
		menu_code.Append(id_compile_code, "Compile");

		SetMenuBar(&menubar);

		//wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
		//wxBoxSizer* list_sizer = new wxBoxSizer(wxHORIZONTAL);
		
		//SetBackgroundStyle(wxBG_STYLE_COLOUR);
		//SetBackgroundColour(wxColour(200, 200, 200));
		SetSize(wxSize(640, 680));

		asm_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(500, 500),
			wxTE_LEFT | wxTE_MULTILINE | wxTE_DONTWRAP);
		hex_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(640, 180),
			wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
		err_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(140, 200),
			wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY);

		m_aui_mgr.AddPane(asm_list, wxAuiPaneInfo().Name(L"asm_list").CenterPane().PaneBorder(false).CloseButton(false));
		m_aui_mgr.AddPane(hex_list, wxAuiPaneInfo().Name(L"hex_list").Right().PaneBorder(false).CloseButton(false));
		m_aui_mgr.AddPane(err_list, wxAuiPaneInfo().Name(L"err_list").Bottom().PaneBorder(false).CloseButton(false));
		m_aui_mgr.GetPane(L"asm_list").Show();
		m_aui_mgr.GetPane(L"hex_list").Show();
		m_aui_mgr.GetPane(L"err_list").Show();
		m_aui_mgr.Update();

		//const wxSize& size = wxSize(640, 680);

		//asm_list->SetSize(size.GetWidth() * 0.75, size.GetHeight());
		//hex_list->SetSize(size.GetWidth() * 0.25, size.GetHeight());

		//list_sizer->Add(asm_list);
		//list_sizer->AddSpacer(2);
		//list_sizer->Add(hex_list);

		//main_sizer->Add(list_sizer);
		//main_sizer->AddSpacer(2);
		//main_sizer->Add(err_list);

		//SetSizerAndFit(main_sizer);

		scroll_timer = new wxTimer(this);

		//Connect( wxEVT_SIZE, wxSizeEventHandler(CompilerELF::OnResize) );
		Connect(asm_list->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerELF::OnUpdate));
		Connect(id_analyze_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::AnalyzeCode));
		Connect(id_compile_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::CompileCode));

		Connect(scroll_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(CompilerELF::OnUpdateScroll));

		scroll_timer->Start(5);

		a_instr = new wxTextAttr(wxColour(0, 160, 0));

		asm_list->SetFont(GetFont(16));
		hex_list->SetFont(GetFont(16));

		hex_list->Disable();

		asm_list->SetValue(
			"T2M r3, 0x200, \"outlog.txt\"\n"
			"li r4, 0x000001\n"
			"li r11, 801\n"
			"sc 2\n"
			"\n"
			"mr r12, r5\n"
			"mr r3, r5\n"
			"T2M r4, 0x200, \"Hello world!\"\n"
			"li r5, 12 #12 = strlen(\"Hello world!\")\n"
			"li r11, 803\n"
			"sc 2\n"
			"\n"
			"mr r3, r12\n"
			"li r11, 804\n"
			"sc 2\n"
		);
	}

	~CompilerELF()
	{
		scroll_timer->Stop();
	}

	wxFont GetFont(int size)
	{
		return wxFont(size, wxMODERN, wxNORMAL, wxNORMAL);
	}

	bool Cmp(const wxString& s1, const wxString& s2, u64& from, u64& to)
	{
		if(s1.Length() > s2.Length())
		{
			const int pos = s1.Find(s2);
			if(pos == -1) return false;
			from = pos;
			to = s2.Length();
			return true;
		}
		if(!s2.Cmp(s1))
		{
			from = 0;
			to = s2.Length();
			return true;
		}

		return false;
	}
	
	void UpdateScroll()
	{
		const int asm_count = asm_list->GetScrollPos(wxVERTICAL);
		int hex_count = hex_list->GetScrollPos(wxVERTICAL);

		if(asm_count == hex_count) return;

		if(hex_count > asm_count)
		{
			for(; hex_count>asm_count; --hex_count)
				::SendMessage((HWND)hex_list->GetHWND(), WM_VSCROLL, SB_LINEUP, 0);
		}
		else
		{
			const int lines_count = hex_list->GetNumberOfLines();
			for(; hex_count<asm_count && hex_count<lines_count; ++hex_count)
				::SendMessage((HWND)hex_list->GetHWND(), WM_VSCROLL, SB_LINEDOWN, 0);
		}
	}

	void OnUpdateScroll(wxTimerEvent& WXUNUSED(event))
	{
		UpdateScroll();
	}

	void OnUpdate(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(false);
		UpdateScroll();
		//TODO...
	}

	void OnResize(wxSizeEvent& WXUNUSED(event))
	{
		const wxSize size(GetClientSize());

		static const int hex_w = 130;
		asm_list->SetSize(size.GetWidth() - hex_w, size.GetHeight() * 0.75);
		hex_list->SetSize(hex_w, size.GetHeight() * 0.75);
		err_list->SetSize(size.GetWidth(), size.GetHeight() * 0.25);

		hex_list->SetPosition(wxPoint(asm_list->GetSize().x + 3, 0));
		err_list->SetPosition(wxPoint(0, asm_list->GetSize().y + 2));

		UpdateScroll();
	}

	void WriteError(const wxString& error, const uint line)
	{
		err_list->WriteText(wxString::Format("line %d: %s\n", line, error));
	}

	int StringToReg(wxString str)
	{
		if(!str(0, 1).Cmp("r") && (str.Length() == 2 || str.Length() == 3))
		{
			str.erase(0, 1);
			long ret;

			str.ToLong(&ret);
			
			if(ret != 0)
			{
				if(ret > 31 || ret < 0) return -1;
				return ret;
			}

			if(str.Length() > 1) return -1;
			if(!str(0, 1).Cmp("0")) return 0;

			return -1;
		}

		return -1;
	}

	enum MASKS
	{
		MASK_ERROR,
		R2_IMM16,
		R1_IMM16,
		IMM16,
		LL_AA_LK,
		R2_DS,
		R2,
	};

	enum OPCODES
	{
		O_STH,	O_ADDI, O_LI, O_SC, O_B,
		O_BL,	O_MR,
	};

	MASKS OpcodeToMask(int opcode)
	{
		switch(opcode)
		{
		case O_STH:
			return R2_DS;

		case O_ADDI:
			return R2_IMM16;

		case O_LI:
			return R1_IMM16;

		case O_SC:
			return IMM16;

		case O_MR:
			return R2;

		case O_BL:
		case O_B:
			return LL_AA_LK;

		default: break;
		}

		return MASK_ERROR;
	}

	int StringToOpcode(const wxString& str)
	{
		if(!str.Cmp("sth"))		return O_STH;
		if(!str.Cmp("sc"))		return O_SC;
		if(!str.Cmp("mr"))		return O_MR;
		if(!str.Cmp("addi"))	return O_ADDI;
		if(!str.Cmp("li"))		return O_LI;

		//b
			if(!str.Cmp("b"))	return O_B;
			if(!str.Cmp("bl"))	return O_BL;
		//

		if(!str.Cmp("T2M"))		return -2;

		return -1;
	}

	bool StringToImm(wxString str, s64* imm)
	{
		if(str.IsEmpty()) return false;

		str.ToLongLong(imm);
		if(*imm == 0)
		{
			if(str.Length() == 1) return !str.Cmp("0");

			if(str.Length() <= 2 || !!str(0, 2).Cmp("0x")) return false;
			
			str.erase(0, 2);

			sscanf(str, "%x", imm);
			return true;
		}

		return true;
	}

	void AnalyzeCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(false);
	}

	void CompileCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(true);
	}

	s32 SetField(s32 src, u32 from, u32 to) const
	{
		return (src & ((1 << ((to - from) + 1)) - 1)) << (31 - to);
	}

	s32 SetField(s32 src, u32 p) const
	{
		return (src & 0x1) << (31 - p);
	}

	//s32 ToOpcode(u32 i)	const { return i << 26; }
	s32 ToOpcode(s32 i)	const { return SetField(i, 0, 5); }
	s32 ToRS(s32 i)		const { return SetField(i, 6, 10); }
	s32 ToRT(s32 i)		const { return SetField(i, 6, 10); }
	s32 ToRA(s32 i)		const { return SetField(i, 11, 15); }
	s32 ToRB(s32 i)		const { return SetField(i, 16, 20); }
	s32 ToLL(s32 i)		const { return SetField(i, 6, 31); }
	s32 ToAA(s32 i)		const { return SetField(i, 30); }
	s32 ToLK(s32 i)		const { return SetField(i, 31); }
	s32 ToIMM16(s32 i)	const { return SetField(i, 16, 31); }
	s32 ToD(s32 i)		const { return SetField(i, 16, 31); }
	s32 ToDS(s32 i) const
	{
		if(i < 0) return ToD(i + 1);
		return ToD(i);
	}

	int Compile(wxArrayLong& arr, MASKS mask)
	{
		switch(mask)
		{
		case R2_IMM16:
			switch(arr[0]) //opcode
			{
			case O_ADDI: return ToOpcode(ADDI)  | ToRT(arr[1]) | ToRA(arr[2]) | ToIMM16(arr[3]);
			//case O_MULLI: return ToOpcode(MULLI) | ToRS(arr[1]) | ToRT(arr[2]) | ToIMM16(arr[3]);
			default: break;
			}
		break;
		case R1_IMM16:
			switch(arr[0]) //opcode
			{
			case O_LI: return ToOpcode(ADDI) | ToRT(arr[1]) | ToRA(0) | ToIMM16(arr[2]);
			default: break;
			}
		break;

		case IMM16:
			switch(arr[0]) //opcode
			{
			case O_SC: return ToOpcode(SC) | ToIMM16(arr[1]);
			default: break;
			}
		break;

		case R2:
			switch(arr[0])
			{
			case O_MR: return ToOpcode(G_1f) | SetField(MR, 22, 30) | ToRA(arr[1]) | ToRB(arr[2]);
			default: break;
			}
		break;
		
		case LL_AA_LK:
			switch(arr[0]) //opcode
			{
			case O_B: return ToOpcode(B) | ToLL(arr[1]) | ToAA(0) | ToLK(0);
			case O_BL: return ToOpcode(B) | ToLL(arr[1]) | ToAA(0) | ToLK(1);
			}
		break;

		case R2_DS:
			switch(arr[0]) //opcode
			{
			case O_STH:  return ToOpcode(STH) | ToRS(arr[1]) | ToRA(arr[2]) | ToD(arr[3]);
			default: break;
			}
		}

		return 0; //NOP
	}

	bool IsSkipChar(const wxString& str)
	{
		return !str.Cmp(" ") || !str.Cmp("	");
	}

	bool IsNewLine(const wxString& str)
	{
		return !str.Cmp("\n");
	}

	bool IsBreakChar2(const wxString& str)
	{
		return IsNewLine(str) || !str.Cmp(",") || !str.Cmp("#");
	}

	bool IsBreakChar(const wxString& str)
	{
		return IsSkipChar(str) || IsBreakChar2(str);
	}

	bool IsSkipRange(const wxString& str, const wxString& buf, const u64 p, const u64 len)
	{
		return IsSkipChar(str) && buf.IsEmpty() && p != len - 1;
	}

	bool SearchReg(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		wxString buf = wxEmptyString;

		for(p++; p<str.Length(); p++)
		{
			const wxString& cur_str = str(p, 1);

			if(IsSkipRange(cur_str, buf, p, str.Length())) continue;

			if(p == str.Length() - 1 || IsBreakChar(cur_str))
			{
				if(p == str.Length()-1 && !IsSkipChar(cur_str)) buf += cur_str;

				const int reg = StringToReg(buf);

				if(reg == -1) break;

				arr.Add(reg);
				return true;
			}

			buf += cur_str;
		}

		WriteError(wxString::Format("Unknown register '%s'", buf), line);
		return false;
	}

	enum ImmType
	{
		IMM_NUM,
		IMM_TEXT,
	};

	bool GetImm(u64& p, const wxString& str, const u64 line, wxArrayLong& arr, ImmType type)
	{
		wxString buf = wxEmptyString;
		for(; p<str.Length(); p++)
		{
			const wxString& cur_str = str(p, 1);
			switch(type)
			{
			case IMM_TEXT:
				if(cur_str.Cmp("\""))
				{
					arr.Add(*(s64*)buf(1, buf.Length()).c_str());
					return true;
				}
				if(p == str.Length() - 1) goto _ERROR_;
			break;

			case IMM_NUM:
				if(IsSkipRange(cur_str, buf, p, str.Length())) continue;

				if(p == str.Length() - 1 || IsBreakChar(cur_str))
				{
					if(p == str.Length() - 1) buf += cur_str;
					s64 imm;
					bool is16 = !!buf(0, 2).Cmp("0x");
					sscanf(buf, (is16 ? "0x%x" : "%d"), &imm);

					if(imm == 0 && (buf.Length() > 1 || (buf.Length() == 1 && !!buf.Cmp("0"))))
					{
						goto _ERROR_;
					}
					arr.Add(imm);

					return true;
				}
			break;
			};

			buf += cur_str;
		}

_ERROR_:
		WriteError(wxString::Format("Unknown immediate '%s'", buf), line+1);
		return false;
	}
	/*
	bool SearchImm(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		ImmType type = IMM_NUM;
		if(!str(p, 1).Cmp("\""))
		{
			p++;
			type = IMM_TEXT;
		}

		return GetImm(p, str, line, arr, type);
	}
	*/
	bool SearchImm(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		wxString buf = wxEmptyString;

		for(p++; p<str.Length(); p++)
		{
			const wxString& cur_str = str(p, 1);
			
			s64 imm;
			if(!buf(0, 1).Cmp("\""))
			{
				if(!cur_str.Cmp("\""))
				{
					for(p++; p<str.Length(); p++)
					{
						if(!str(p, 1).Cmp("#")) break;
						if(!IsSkipChar(cur_str))
						{
							WriteError(wxString::Format("Unknown symbol '%s'", buf), line);
							return false;
						}
					}

					imm = *(s64*)buf(1, buf.Length()).c_str();
					arr.Add(imm);
					return true;
				}
			}
			else
			{
				if(IsSkipChar(cur_str) && p != str.Length() - 1) continue;
				if(p == str.Length() - 1 || !cur_str.Cmp(",") || !cur_str.Cmp("#"))
				{
					if(p == str.Length()-1 && !IsSkipChar(cur_str)) buf += cur_str;
					
					if(StringToImm(buf, &imm))
					{
						arr.Add(imm);
						return true;
					}
					break;
				}
			}
			
			buf += cur_str;
		}
		
		WriteError(wxString::Format("Unknown immediate '%s'", buf), line);
		return false;
	}

	bool SearchText(u64& p, const wxString& str, const u64 line, wxString& dst)
	{
		wxString buf = wxEmptyString;

		for(p++; p<str.Length(); ++p)
		{
			const wxString& cur_str = str(p, 1);

			if(!buf(0, 1).Cmp("\""))
			{
				if(!cur_str.Cmp("\""))
				{
					for(p++; p<str.Length(); p++)
					{
						if(!str(p, 1).Cmp("#")) break;
						if(!IsSkipChar(cur_str))
						{
							WriteError(wxString::Format("Unknown symbol '%s'", buf), line);
							return false;
						}
					}

					dst = buf(1, buf.Length()).c_str();
					return true;
				}
			}
			else
			{
				if(IsSkipChar(cur_str) && p != str.Length() - 1) continue;
			}

			buf += cur_str;
		}

		WriteError(wxString::Format("Text is not found! (%s)", buf), line);
		return false;
	}

	void WriteEhdr(wxFile& f, ElfLoader::Elf64_Ehdr& ehdr)
	{
		Write32(f, ehdr.e_magic);
		Write8(f, ehdr.e_class);
		Write8(f, ehdr.e_data);
		Write8(f, ehdr.e_curver);
		Write8(f, ehdr.e_os_abi);
		Write64(f, ehdr.e_abi_ver);
		Write16(f, ehdr.e_type);
		Write16(f, ehdr.e_machine);
		Write32(f, ehdr.e_version);
		Write64(f, ehdr.e_entry);
		Write64(f, ehdr.e_phoff);
		Write64(f, ehdr.e_shoff);
		Write32(f, ehdr.e_flags);
		Write16(f, ehdr.e_ehsize);
		Write16(f, ehdr.e_phentsize);
		Write16(f, ehdr.e_phnum);
		Write16(f, ehdr.e_shentsize);
		Write16(f, ehdr.e_shnum);
		Write16(f, ehdr.e_shstrndx);
	}

	void WritePhdr(wxFile& f, ElfLoader::Elf64_Phdr& phdr)
	{
		Write32(f, phdr.p_type);
		Write32(f, phdr.p_flags);
		Write64(f, phdr.p_offset);
		Write64(f, phdr.p_vaddr);
		Write64(f, phdr.p_paddr);
		Write64(f, phdr.p_filesz);
		Write64(f, phdr.p_memsz);
		Write64(f, phdr.p_align);
	}

	void WriteShdr(wxFile& f, ElfLoader::Elf64_Shdr& shdr)
	{
		Write32(f, shdr.sh_name);
		Write32(f, shdr.sh_type);
		Write64(f, shdr.sh_flags);
		Write64(f, shdr.sh_addr);
		Write64(f, shdr.sh_offset);
		Write64(f, shdr.sh_size);
		Write32(f, shdr.sh_link);
		Write32(f, shdr.sh_info);
		Write64(f, shdr.sh_addralign);
		Write64(f, shdr.sh_entsize);
	}

	void WriteShdrName(wxFile& f, const wxString& name, u32 sh_offset, u32& name_offs)
	{
		f.Seek(sh_offset + name_offs);
		f.Write(name.char_str().data(), name.Length());
		name_offs += name.Length() + 0x2;
	}

	void DoAnalyzeCode(bool compile)
	{
		wxFile f;

		static const u32 ehdr_size = 0x40;
		static const u32 phdr_size = 0x38;
		static const u32 shdr_size = 0x40;

		static const u32 phdr_count = 3;
		static const u32 shdr_count = phdr_count + 1;
		static const u32 phdr_rcount = phdr_count;
		static const u32 shdr_rcount = shdr_count + 1;

		static const u32 phoff = ehdr_size + 0x2;
		static const u32 shoff = phoff + (phdr_size * phdr_rcount) + 0x2;
		static const u32 entry = 0x10200;

		static const s32 strtab_offs = shoff + (shdr_size * shdr_rcount) + 0x2;

		wxArrayString name_arr;
		wxArrayString got_arr;
		wxArrayInt hex_arr;

		ElfLoader::Elf64_Ehdr ehdr;
		ElfLoader::Elf64_Phdr* phdr = NULL;
		ElfLoader::Elf64_Shdr* shdr = NULL;

		u32 prog_offset;
		u32 p_names_size;
		u32 opd_section_num;
		u32 got_section_num;

		const u32 got_offs = 0x00080000;
		u32 cur_got_offs = got_offs;

		if(compile)
		{
			hex_arr.Clear();
			name_arr.Clear();
			got_arr.Clear();
			prog_offset = 0;
			p_names_size = 0;

			phdr = new ElfLoader::Elf64_Phdr[phdr_count];
			shdr = new ElfLoader::Elf64_Shdr[shdr_count];

			//TODO
			ehdr.e_magic = 0x7F454C46;
			ehdr.e_class = 2; //ELF64
			ehdr.e_data = 2;
			ehdr.e_curver = 1; //ver 1
			ehdr.e_os_abi = 0x66; //Cell OS LV-2
			ehdr.e_abi_ver = 0;
			ehdr.e_type = 2; //EXEC (Executable file)
			ehdr.e_machine = 0x15; //PowerPC64
			ehdr.e_version = 1; //ver 1
			ehdr.e_entry = entry;
			ehdr.e_phoff = phoff;
			ehdr.e_shoff = shoff;
			ehdr.e_flags = 0x0;
			ehdr.e_ehsize = ehdr_size;
			ehdr.e_phentsize = phdr_size;
			ehdr.e_phnum = phdr_rcount;
			ehdr.e_shentsize = shdr_size;
			ehdr.e_shnum = shdr_rcount;
			ehdr.e_shstrndx = 1;

			phdr[0].p_type = 0x1;
			phdr[0].p_flags = 0x4;
			phdr[0].p_align = 0x10000;

			phdr[1].p_type = 0x1;
			phdr[1].p_flags = 0x4;
			phdr[1].p_vaddr = entry;
			phdr[1].p_paddr = entry;
			phdr[1].p_align = 0x10000;
			opd_section_num = 1;

			phdr[2].p_type = 0x1;
			phdr[2].p_flags = 0x4;
			phdr[2].p_vaddr = got_offs;
			phdr[2].p_paddr = got_offs;
			phdr[2].p_align = 0x10000;
			got_section_num = 2;

			name_arr.Add(".strtab");
			shdr[0].sh_type = SHT_STRTAB;
			shdr[0].sh_flags = 0;
			shdr[0].sh_addr = 0;
			shdr[0].sh_offset = strtab_offs;
			shdr[0].sh_link = 0;
			shdr[0].sh_info = 0;
			shdr[0].sh_addralign = 0;
			shdr[0].sh_entsize = 0;

			name_arr.Add(".text");
			shdr[1].sh_type = SHT_PROGBITS;
			shdr[1].sh_flags = SHF_ALLOC | SHF_EXECINSTR;
			shdr[1].sh_link = 0;
			shdr[1].sh_info = 0;
			shdr[1].sh_addralign = 0;
			shdr[1].sh_entsize = 0;

			name_arr.Add(".opd");
			shdr[2].sh_type = SHT_PROGBITS;
			shdr[2].sh_flags = SHF_ALLOC | SHF_WRITE;
			shdr[2].sh_link = 0;
			shdr[2].sh_info = 0;
			shdr[2].sh_addralign = 0;
			shdr[2].sh_entsize = 0;

			name_arr.Add(".got");
			shdr[3].sh_type = SHT_PROGBITS;
			shdr[3].sh_flags = SHF_ALLOC | SHF_WRITE;
			shdr[3].sh_link = 0;
			shdr[3].sh_info = 0;
			shdr[3].sh_addralign = 0;
			shdr[3].sh_entsize = 0;

			for(uint i=0; i<name_arr.GetCount(); ++i)
			{
				p_names_size += name_arr[i].Length() * 2 + 0x2;
			}

			prog_offset = strtab_offs + p_names_size;
		}

		const uint lines_count = (uint)asm_list->GetNumberOfLines();
		u64 errors_count = 0;

		hex_list->Clear();
		err_list->Clear();

		for(uint line=0, virt_line = 0; line<lines_count; ++line)
		{
			if(line > 0) hex_list->WriteText("\n");
			if(compile && errors_count > 0) break;
			wxString buf = wxEmptyString;
			wxString str = asm_list->GetLineText(line);

			for(u64 p=0; p<str.Length(); ++p)
			{
				const wxString cur_str = str(p, 1);

				if(!cur_str.Cmp("#") && p < str.Length() - 1) continue;

				if(IsSkipChar(cur_str) || p == str.Length() - 1)
				{
					if(!IsSkipChar(cur_str) && p == str.Length() - 1)
					{
						buf += cur_str;
					}

					const int opcode = StringToOpcode(buf);
					if(opcode == -1)
					{
						WriteError(wxString::Format("Unknown opcode '%s'", buf), line);
						errors_count++;
						continue;
					}

					wxArrayLong arr;
					arr.Clear();

					if(opcode == -2)
					{
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}

						if(!SearchImm(p, str, line, arr))
						{
							errors_count++;
							break;
						}

						wxString text = wxEmptyString;
						if(!SearchText(p, str, line, text))
						{
							errors_count++;
							break;
						}

						if(compile)
						{
							got_arr.Add(text);
							hex_arr.Add(ToOpcode(ADDI) | ToRT(arr[0]) | ToRA(0) | ToIMM16(cur_got_offs));
							cur_got_offs += text.Length() * 2 + 0x4;
							/*
							if(text.Length() & 1) text += " ";

							for(u32 i=0; i<text.Length(); i+=2)
							{
								const s32& t = *(s32*)text(i, 2).c_str();
								hex_arr.Add(ToOpcode(ADDI) | ToRT(arr[0]) | ToRA(0) | ToIMM16(t)); //li arr[0], t
								hex_arr.Add(ToOpcode(STH) | ToRS(arr[0]) | ToRA(0) | ToD(arr[1] + i)); //sth arr[0], r0, arr[1] + 1
							}

							hex_arr.Add(ToOpcode(ADDI) | ToRT(arr[0]) | ToRA(0) | ToIMM16(0));
							hex_arr.Add(ToOpcode(STH) | ToRS(arr[0]) | ToRA(0) | ToD(text.Length() + 1));
							hex_arr.Add(ToOpcode(ADDI) | ToRT(arr[0]) | ToRA(0) | ToIMM16(arr[1])); //li arr[0], arr[1]
							*/
						}

						break;
					}

					const MASKS mask = OpcodeToMask(opcode);
					if(mask == MASK_ERROR)
					{
						WriteError(wxString::Format("Unknown opcode '%s' mask", buf), line);
						errors_count++;
						break;
					}

					arr.Add(opcode);

					switch(mask)
					{
					case R2:
					case R2_DS:
					case R2_IMM16:
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}
					case R1_IMM16:
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}
					if(mask == R2) break;

					case LL_AA_LK:
					case IMM16:
						if(!SearchImm(p, str, line, arr))
						{
							errors_count++;
							break;
						}
					break;
					};

					if(errors_count > 0) break;

					const s32 compiled = Compile(arr, mask);
					hex_list->WriteText(wxString::Format("%08x", compiled));

					if(compile) hex_arr.Add(compiled);

					virt_line++;
					break;
				}

				buf += cur_str;
			}
		}

		if(compile)
		{
			if(errors_count == 0)
			{
				f.Open("compiled.elf", wxFile::write);
				f.Seek(0);
				WriteEhdr(f, ehdr);

				phdr[0].p_filesz = hex_arr.GetCount() * 4;
				phdr[0].p_paddr = entry - phdr[0].p_filesz - 0x4;
				phdr[0].p_vaddr = phdr[0].p_paddr;

				phdr[opd_section_num].p_filesz = sizeof(u64);
				phdr[got_section_num].p_filesz = cur_got_offs - got_offs;

				for(uint i=0, p_offset = phoff; i<phdr_count; ++i, p_offset += phdr_size)
				{
					phdr[i].p_offset = prog_offset;
					phdr[i].p_memsz = phdr[i].p_filesz;
					f.Seek(p_offset);
					WritePhdr(f, phdr[i]);
					prog_offset += phdr[i].p_filesz + 0x4;
				}

				for(uint i=0, name_offs = 0xb, sh_offset = shoff + shdr_size; i<shdr_count; ++i, sh_offset += shdr_size)
				{
					if(ehdr.e_shstrndx-1 == i)
					{
						shdr[i].sh_size = p_names_size;
					}
					else
					{
						shdr[i].sh_size = phdr[i-1].p_filesz;
						shdr[i].sh_offset = phdr[i-1].p_offset;
						shdr[i].sh_addr = phdr[i-1].p_paddr;
					}

					shdr[i].sh_name = name_offs;

					f.Seek(sh_offset);
					WriteShdr(f, shdr[i]);

					WriteShdrName(f, name_arr[i], strtab_offs, name_offs);
				}

				f.Seek(phdr[0].p_offset);

				for(uint i=0; i<hex_arr.GetCount(); ++i)
				{
					Write32(f, hex_arr[i]);
				}

				f.Seek(phdr[1].p_offset);
				Write32(f, phdr[0].p_paddr);
				Write32(f, 0x00010000);

				f.Seek(phdr[2].p_offset);
				for(uint i=0; i<got_arr.GetCount(); ++i)
				{
					f.Write(got_arr[i]);
					Write16(f, 0x0);
				}

				wxMessageBox("Compile done.", "Compile message");
			}

			hex_arr.Clear();
			name_arr.Clear();

			safe_delete(phdr);
			safe_delete(shdr);
		}
	}
};