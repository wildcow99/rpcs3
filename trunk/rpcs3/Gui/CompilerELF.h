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
			wxTE_LEFT | wxTE_MULTILINE );
		hex_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
			wxTE_LEFT | wxTE_MULTILINE | wxTE_READONLY);


		wxSize size = wxSize(640, 680);

		asm_list->SetSize(size.GetWidth() * 0.75, size.GetHeight());
		hex_list->SetSize(size.GetWidth() * 0.25, size.GetHeight());

		list_sizer->Add(asm_list);
		list_sizer->AddSpacer(2);
		list_sizer->Add(hex_list);

		main_sizer->Add(list_sizer);

		SetSizerAndFit(main_sizer);

		Connect( wxEVT_SIZE, wxSizeEventHandler(CompilerELF::OnResize) );
		Connect(asm_list->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerELF::OnUpdate));
		Connect(id_analyze_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::AnalyzeCode));
		Connect(id_compile_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::CompileCode));

		SetSize(wxSize(640, 680));

		a_instr = new wxTextAttr(wxColour(0, 160, 0));

		asm_list->SetFont(GetFont(16));

		asm_list->SetValue(
			//"T2R r3,\"opcode\"\n"
			"li r3,\"tx\"\n"
			"li r4,0x000001\n"
			"li r5,0x0\n"
			"li r6,0x0\n"
			"li r8,0x0\n"
			"li r11,801\n"
			"sc 2");
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

	void OnUpdate(wxCommandEvent& event)
	{
		//TODO...
	}

	void OnResize(wxSizeEvent& WXUNUSED(event))
	{
		const wxSize size(GetClientSize());

		asm_list->SetSize(size.GetWidth() * 0.75, size.GetHeight());
		hex_list->SetSize(size.GetWidth() * 0.25 - 3, size.GetHeight());
		hex_list->SetPosition(wxPoint(asm_list->GetSize().x + 3, 0));
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
		RS_RT_IMM,
		RS_IMM,
		IMM,
	};

	MASKS OpcodeToMask(int opcode)
	{
		switch(opcode)
		{
		case MULLI:
		case ADDI:	return RS_RT_IMM;

		case LI:	return RS_IMM;

		case SC:	return IMM;
		}

		return MASK_ERROR;
	}

	int StringToOpcode(const wxString& str)
	{
		if(!str.Cmp("mulli"))return MULLI;
		if(!str.Cmp("addi")) return ADDI;
		if(!str.Cmp("li"))   return LI;
		if(!str.Cmp("sc"))   return SC;
		//if(!str.Cmp("T2R"))  return -2; //TODO

		return -1;
	}

	bool StringToImm(wxString str, s64* imm)
	{
		if(str.IsEmpty()) return false;

		str.ToLongLong(imm);
		if(*imm == 0)
		{
			if(str.Length() == 1) return str.Cmp("0") == 0;

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
	s32 ToIMM(u32 i)	const { return i & 0xffff; }
	s32 ToLS(u32 i)		const { return i << 16; }

	int Compile(wxArrayLong& arr, MASKS mask)
	{
		switch(mask)
		{
		case RS_RT_IMM:
			switch(arr[0]) //opcode
			{
			case ADDI: return ToOpcode(G1) | ToLS(ADDI) | ToRS(arr[1]) | ToRT(arr[2]) | ToIMM(arr[3]);
			case MULLI: return ToOpcode(MULLI) | ToRS(arr[1]) | ToRT(arr[2]) | ToIMM(arr[3]);
			default: return -1;
			}
		break;
		case RS_IMM:
			switch(arr[0]) //opcode
			{
			case LI: return ToOpcode(G1) | ToLS(LI) | ToRS(arr[1]) | ToIMM(arr[2]);
			default: return -1;
			}
		break;

		case IMM:
			switch(arr[0]) //opcode
			{
			case SC: return ToOpcode(SC) | ToIMM(arr[1]);
			default: return -1;
			}
		break;
		}

		return -1;
	}

	bool SearchReg(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		p++;
		wxString buf = wxEmptyString;

		for(; p<str.Length(); ++p)
		{
			const wxString cur_str = str(p, 1);

			if( !cur_str.Cmp("\n") || p == str.Length() ||
				!cur_str.Cmp(",") || !cur_str.Cmp(" "))
			{
				const int reg = StringToReg(buf);

				if(reg == -1)
				{
					ConLog.Error("line %d: Unknown register '%s'", line, buf);
					return false;
				}

				arr.Add(reg);
				return true;
			}

			buf += cur_str;
		}

		return false;
	}

	bool SearchImm(u64& p, const wxString& str, const u64 line, wxArrayLong& arr)
	{
		p++;
		wxString buf = wxEmptyString;

		for(;; ++p)
		{
			const wxString cur_str = str(p, 1);

			if( !cur_str.Cmp("\n") || p == str.Length() ||
				!cur_str.Cmp(",") || !cur_str.Cmp(" "))
			{
				s64 imm;
				if(!buf(0, 1).Cmp("\"") || !buf(buf.Length()-1, 1).Cmp("\""))
				{
					imm = *(s64*)buf(1, buf.Length()-1).c_str();
				}
				else if(!StringToImm(buf, &imm))
				{
					ConLog.Error("line %d: Unknown immediate '%s'", line, buf);
					return false;
				}
				
				arr.Add(imm);
				return true;
			}

			buf += cur_str;
		}

		return false;
	}

	bool SearchText(u64& p, const wxString& str, const u64 line, wxString& dst)
	{
		p++;
		wxString buf = wxEmptyString;

		for(;; ++p)
		{
			const wxString cur_str = str(p, 1);

			if( !cur_str.Cmp("\n") || p == str.Length() ||
				!cur_str.Cmp(",") || !cur_str.Cmp(" "))
			{
				if(!buf(0, 1).Cmp("\"") || !buf(buf.Length()-1, 1).Cmp("\""))
				{
					dst = buf(1, buf.Length()-1);
					return true;
				}

				ConLog.Error("line %d: Text is not found! (%s)", line, buf);
				return false;
			}

			buf += cur_str;
		}

		return false;
	}

	bool GoToNextLine(u64& p, u32& line, const wxString& str)
	{
		for(;p<str.Length(); ++p)
		{
			if(str(p, 1).Cmp("\n"))
			{
				line++;
				p--;
				return true;
			}
		}

		return false;
	}

	void DoAnalyzeCode(bool compile)
	{
		const wxString str = asm_list->GetLabel();
		wxString buf = wxEmptyString;
		uint line = 1;
		bool skip = false;

		wxFile f;
		if(compile)
		{
			f.Open("compiled.elf", wxFile::write);
			ElfLoader::Elf64_Ehdr ehdr;

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

			ElfLoader::Elf64_Phdr phdr;

			phdr.p_type = 0x1;
			phdr.p_flags = 0x4;
			phdr.p_offset = ehdr.e_phoff + 0x38 + 0x2;
			phdr.p_vaddr = 0x20050;
			phdr.p_paddr = 0x20050;
			phdr.p_filesz = asm_list->GetNumberOfLines() * 4*15000;
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

		for(u64 p=0; p<str.Length(); ++p)
		{
			const wxString cur_str = str(p, 1);

			if(!cur_str.Cmp(" "))
			{
				const int opcode = StringToOpcode(buf);
				if(opcode == -1)
				{
					ConLog.Error("line %d: Unknown opcode '%s'", line, buf);
					if(compile || !GoToNextLine(p, line, str)) return;
					buf = wxEmptyString;
					continue;
				}

				wxArrayLong arr;
				arr.Clear();

				if(opcode == -2) //TODO: T2R
				{
					if(!SearchReg(p, str, line, arr))
					{
						if(compile || !GoToNextLine(p, line, str)) return;
						buf = wxEmptyString;
						continue;
					}

					wxString text;
					if(!SearchText(p, str, line, text))
					{
						if(compile || !GoToNextLine(p, line, str)) return;
						buf = wxEmptyString;
						continue;
					}

					if(compile)
					{
						Write32(f, ToOpcode(G1) | ToLS(LI) | ToRS(arr[0]) | ToIMM(0));

						ConLog.Write("Text size: %d", WXSIZEOF(text));

						const u64 text_u64 = *(u64*)text.char_str().data();

						const s16 count = text_u64 / 0x7fff;
						const u64 l_v = text_u64 - (count * 0x7fff);

						Write32(f, ToOpcode(G1) | ToLS(LI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM(0x7fff));
						Write32(f, ToOpcode(MULLI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM(count));
						Write32(f, ToOpcode(G1) | ToLS(ADDI) | ToRS(arr[0]) | ToRT(arr[0]) | ToIMM(l_v));
					}

					buf = wxEmptyString;
					continue;
				}

				const MASKS mask = OpcodeToMask(opcode);
				if(mask == MASK_ERROR)
				{
					ConLog.Error("line %d: Unknown opcode '%s' mask", line, buf);
					if(compile || !GoToNextLine(p, line, str)) return;
					buf = wxEmptyString;
					continue;
				}

				arr.Add(opcode);

				switch(mask)
				{
				case RS_RT_IMM:
					if(!SearchReg(p, str, line, arr))
					{
						if(compile || !GoToNextLine(p, line, str)) return;
						buf = wxEmptyString;
						continue;
					}
				case RS_IMM:
					if(!SearchReg(p, str, line, arr))
					{
						if(compile || !GoToNextLine(p, line, str)) return;
						buf = wxEmptyString;
						continue;
					}
				case IMM:
					if(!SearchImm(p, str, line, arr))
					{
						if(compile || !GoToNextLine(p, line, str)) return;
						buf = wxEmptyString;
						continue;
					}
				break;
				};

				if(compile) Write32(f, Compile(arr, mask));
				line++;
				buf = wxEmptyString;
				continue;
			}

			buf += cur_str;
		}
	}
};