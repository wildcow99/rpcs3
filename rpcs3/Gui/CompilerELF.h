#pragma once

#include "Emu/Opcodes/Opcodes.h"
#include "Emu/ElfLoader.h"

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

public:
	CompilerELF(wxWindow* parent) : wxFrame(parent, wxID_ANY, "CompilerELF")
	{
		wxMenuBar& menubar(*new wxMenuBar());
		wxMenu& menu_code(*new wxMenu());
		menubar.Append(&menu_code, "Code");

		menu_code.Append(id_analyze_code, "Analyze");
		menu_code.Append(id_compile_code, "Compile");

		SetMenuBar(&menubar);

		wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer* list_sizer = new wxBoxSizer(wxHORIZONTAL);
		
		SetBackgroundStyle(wxBG_STYLE_COLOUR);
		SetBackgroundColour(wxColour(200, 200, 200));
		asm_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
			wxTE_LEFT | wxTE_MULTILINE | wxTE_DONTWRAP);
		hex_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
			wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
		err_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
			wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY);

		const wxSize& size = wxSize(640, 680);

		asm_list->SetSize(size.GetWidth() * 0.75, size.GetHeight());
		hex_list->SetSize(size.GetWidth() * 0.25, size.GetHeight());

		list_sizer->Add(asm_list);
		list_sizer->AddSpacer(2);
		list_sizer->Add(hex_list);

		main_sizer->Add(list_sizer);
		main_sizer->AddSpacer(2);
		main_sizer->Add(err_list);

		SetSizerAndFit(main_sizer);

		scroll_timer = new wxTimer(this);

		Connect( wxEVT_SIZE, wxSizeEventHandler(CompilerELF::OnResize) );
		Connect(asm_list->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerELF::OnUpdate));
		Connect(id_analyze_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::AnalyzeCode));
		Connect(id_compile_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::CompileCode));

		Connect(scroll_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(CompilerELF::OnUpdateScroll));

		SetSize(wxSize(640, 680));

		scroll_timer->Start(5);

		a_instr = new wxTextAttr(wxColour(0, 160, 0));

		asm_list->SetFont(GetFont(16));
		hex_list->SetFont(GetFont(16));

		hex_list->Disable();

		asm_list->SetValue(
			//"T2R r3,\"opcode\"\n"
			"li r3, \"tx\"\n"
			"li r4, 0x000001\n"
			"li r5, 0\n"
			"li r6, 0\n"
			"li r8, 0\n"
			"li r11, 801\n"
			"sc 2");
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
		RS_RT_IMM16,
		RS_IMM16,
		IMM16,
		IMM26,
	};

	enum OPCODES
	{
		O_MULLI,
		O_ADDI,
		O_LI,
		O_SC,
		O_B,
		O_BL,
	};

	MASKS OpcodeToMask(int opcode)
	{
		switch(opcode)
		{
		case O_MULLI:
		case O_ADDI:
			return RS_RT_IMM16;

		case O_LI:
			return RS_IMM16;

		case O_SC:
			return IMM16;

		case O_BL:
		case O_B:
			return IMM26;

		default: break;
		}

		return MASK_ERROR;
	}

	int StringToOpcode(const wxString& str)
	{
		if(!str.Cmp("mulli"))	return O_MULLI;
		if(!str.Cmp("sc"))		return O_SC;
		//g1
			if(!str.Cmp("addi"))return O_ADDI;
			if(!str.Cmp("li"))	return O_LI;
		//
		//BRANCH
			if(!str.Cmp("b"))	return O_B;
			if(!str.Cmp("bl"))	return O_BL;
		//

		//if(!str.Cmp("T2R"))  return -2; //TODO

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

	s32 ToOpcode(u32 i)	const { return i << 26; }
	s32 ToRS(u32 i)		const { return i << 21; }
	s32 ToRT(u32 i)		const { return i << 16; }
	s32 ToIMM16(u32 i)	const { return i & 0xffff; }
	s32 ToIMM26(u32 i)	const { return i & 0xffffff; }
	s32 ToLK(u32 i)		const { return i; }
	s32 ToLS(u32 i)		const { return i << 16; }

	int Compile(wxArrayLong& arr, MASKS mask)
	{
		switch(mask)
		{
		case RS_RT_IMM16:
			switch(arr[0]) //opcode
			{
			case O_ADDI: return ToOpcode(G1) | ToLS(ADDI) | ToRS(arr[1]) | ToRT(arr[2]) | ToIMM16(arr[3]);
			case O_MULLI: return ToOpcode(MULLI) | ToRS(arr[1]) | ToRT(arr[2]) | ToIMM16(arr[3]);
			default: return 0;
			}
		break;
		case RS_IMM16:
			switch(arr[0]) //opcode
			{
			case O_LI: return ToOpcode(G1) | ToLS(LI) | ToRS(arr[1]) | ToIMM16(arr[2]);
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

		case IMM26:
			switch(arr[0]) //opcode
			{
			case O_B: return ToOpcode(BRANCH) | ToLK(B) | ToIMM26(arr[1]);
			case O_BL: return ToOpcode(BRANCH) | ToLK(BL) | ToIMM26(arr[1]);
			default: break;
			}
		break;
		}

		return 0; //NOP
	}

	bool IsSkipChar(const wxString& str)
	{
		return !str.Cmp(" ") || !str.Cmp("	");
	}

	bool SearchReg(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		wxString buf = wxEmptyString;

		for(p++; p<str.Length(); p++)
		{
			const wxString& cur_str = str(p, 1);

			if(IsSkipChar(cur_str) && buf.IsEmpty() && p != str.Length() - 1) continue;

			if(p == str.Length() - 1 || !cur_str.Cmp(",") || !cur_str.Cmp(" ") || !cur_str.Cmp("#"))
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

	void DoAnalyzeCode(bool compile)
	{
		wxFile f;

		ElfLoader::Elf64_Ehdr ehdr;
		ElfLoader::Elf64_Phdr phdr;

		if(compile)
		{
			f.Open("compiled.elf", wxFile::write);

			//TODO
			ehdr.e_magic = 0x7F454C46;
			ehdr.e_class = 2;
			ehdr.e_data = 2;
			ehdr.e_curver = 1;
			ehdr.e_os_abi = 0x66;
			ehdr.e_abi_ver = 0;
			ehdr.e_type = 2;
			ehdr.e_machine = 0x15;
			ehdr.e_version = 1;
			ehdr.e_entry = 0x20050;
			ehdr.e_phoff = 0x40;
			ehdr.e_shoff = 0x0; //FIXME
			ehdr.e_flags = 0x0; //FIXME
			ehdr.e_ehsize = 0x40;
			ehdr.e_phentsize = 0x40;
			ehdr.e_phnum = 1;
			ehdr.e_shentsize = 0x0; //FIXME
			ehdr.e_shnum = 0; //FIXME
			ehdr.e_shstrndx = 0; //FIXME

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

			for(uint i=ehdr.e_ehsize; i<ehdr.e_phoff; ++i) Write8(f, 0);

			phdr.p_type = 0x1;
			phdr.p_flags = 0x4;
			phdr.p_offset = ehdr.e_phoff + 0x38 + 0x2;
			phdr.p_vaddr = 0x20050;
			phdr.p_paddr = 0x20050;
			phdr.p_filesz = asm_list->GetNumberOfLines() * 4 * 15000;
			phdr.p_memsz = phdr.p_filesz;
			phdr.p_align = 0x0;

			Write32(f, phdr.p_type);
			Write32(f, phdr.p_flags);
			Write64(f, phdr.p_offset);
			Write64(f, phdr.p_vaddr);
			Write64(f, phdr.p_paddr);
			Write64(f, phdr.p_filesz);
			Write64(f, phdr.p_memsz);
			Write64(f, phdr.p_align);

			for(uint i=ehdr.e_phoff + 0x38; i<phdr.p_offset; ++i) Write8(f, 0);
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

					if(opcode == -2) //TODO: T2R
					{
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}

						wxString text;
						if(!SearchText(p, str, line, text))
						{
							errors_count++;
							break;
						}

						if(compile)
						{
							Write32(f, ToOpcode(G1) | ToLS(LI) | ToRS(arr[0]) | ToIMM16(0));

							ConLog.Write("Text size: %d", WXSIZEOF(text));

							const u64 text_u64 = *(u64*)text.char_str().data();

							const s16 count = text_u64 / 0x7fff;
							const u64 l_v = text_u64 - (count * 0x7fff);

							Write32(f, ToOpcode(G1) | ToLS(LI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM16(0x7fff));
							Write32(f, ToOpcode(MULLI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM16(count));
							Write32(f, ToOpcode(G1) | ToLS(ADDI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM16(l_v));
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
					case RS_RT_IMM16:
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}
					case RS_IMM16:
						if(!SearchReg(p, str, line, arr))
						{
							errors_count++;
							break;
						}
					case IMM26:
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

					if(compile) Write32(f, compiled);

					virt_line++;
					break;
				}

				buf += cur_str;
			}
		}

		if(compile && errors_count == 0) wxMessageBox("Compile done.", "Compile message");
	}
};