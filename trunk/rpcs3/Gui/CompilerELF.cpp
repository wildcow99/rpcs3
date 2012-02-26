#include "stdafx.h"
#include "CompilerELF.h"

void Write8(wxFile& f, const u8 data)
{
	f.Write(&data, 1);
}
void Write16(wxFile& f, const u16 data)
{
	Write8(f, data >> 8);
	Write8(f, data);
}
void Write32(wxFile& f, const u32 data)
{
	Write16(f, data >> 16);
	Write16(f, data);
}
void Write64(wxFile& f, const u64 data)
{
	Write32(f, data >> 32);
	Write32(f, data);
}

enum CompilerIDs
{
	id_analyze_code = 0x555,
	id_compile_code,
	id_load_elf,
};

wxFont GetFont(int size)
{
	return wxFont(size, wxMODERN, wxNORMAL, wxNORMAL);
}

enum MASKS
{
	MASK_ERROR = -1,
	MASK_UNK,
	MASK_NO_ARG,
	MASK_RST_RA_IMM16,
	MASK_RA_RST_IMM16,
	MASK_BF_L_RA_IMM16,
	MASK_BO_BI_BD_AA_LK,
	MASK_SYS,
	MASK_LL_AA_LK,
	MASK_RA_RS_SH_MB_ME,
	MASK_RST_RA_D,
	MASK_RST_RA_DS,
};

static const struct Instruction
{
	MASKS mask;
	wxString name;
	u32 op_num;
	u32 op_g;
	bool oe;
	bool rc;
} m_instructions[] =
{
	//G_04 = 0x04,
	{MASK_NO_ARG,			"nop",		0,		0, 0, 0},
	{MASK_RST_RA_IMM16,		"mulli",	MULLI,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"subfic",	SUBFIC,	0, 0, 0},
	//{0, "dozi", DOZI, 0, 0, 0},
	{MASK_BF_L_RA_IMM16,	"cmpli",	CMPLI,	0, 0, 0},
	{MASK_BF_L_RA_IMM16,	"cmpl",		CMPL,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"addic",	ADDIC,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"addic.",	ADDIC_,	0, 0, 1},
	{MASK_RST_RA_IMM16,		"addi",		ADDI,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"addis",	ADDIS,	0, 0, 0},
	{MASK_BO_BI_BD_AA_LK,	"bc",		BC,		0, 0, 0},
	{MASK_SYS,				"sc",		SC,		0, 0, 0},
	{MASK_LL_AA_LK,			"b",		B,		0, 0, 0},
	//G_13 = 0x13,
	{MASK_RA_RS_SH_MB_ME,	"rlwinm",	RLWINM,	0, 0, 0},
	{MASK_RA_RS_SH_MB_ME,	"rlwinm.",	RLWINM,	0, 0, 1},
	{MASK_RST_RA_IMM16,		"ori",		ORI,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"oris",		ORIS,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"xori",		XORI,	0, 0, 0},
	{MASK_RST_RA_IMM16,		"xoris",	XORIS,	0, 0, 0},
	{MASK_RA_RST_IMM16,		"andi.",	ANDI_,	0, 0, 1},
	{MASK_RA_RST_IMM16,		"andis.",	ANDIS_,	0, 0, 1},
	//G_1e = 0x1e,
	//G_1f = 0x1f,
	{MASK_RST_RA_D,			"lwz",		LWZ,	0, 0, 0},
	{MASK_RST_RA_DS,		"lwzu",		LWZU,	0, 0, 0},
	{MASK_RST_RA_D,			"lbz",		LBZ,	0, 0, 0},
	{MASK_RST_RA_DS,		"lbzu",		LBZU,	0, 0, 0},
	{MASK_RST_RA_D,			"stw",		STW,	0, 0, 0},
	{MASK_RST_RA_DS,		"stwu",		STWU,	0, 0, 0},
	{MASK_RST_RA_D,			"stb",		STB,	0, 0, 0},
	{MASK_RST_RA_DS,		"stbu",		STBU,	0, 0, 0},
	{MASK_RST_RA_D,			"lhz",		LHZ,	0, 0, 0},
	{MASK_RST_RA_DS,		"lhzu",		LHZU,	0, 0, 0},
	{MASK_RST_RA_D,			"lhz",		LHA,	0, 0, 0},
	{MASK_RST_RA_DS,		"lhau",		LHAU,	0, 0, 0},
	{MASK_RST_RA_D,			"sth",		STH,	0, 0, 0},
	//{0, "lmw", LMW, 0, false, false},
	//LFS = 0x30,
	//LFSU = 0x31,
	//LFD = 0x32,
	//LFDU = 0x33,
	//STFS = 0x34,
	//STFD = 0x36,
	//LFQ = 0x38,
	//LFQU = 0x39,
	//G_3a = 0x3a,
	//G_3b = 0x3b,
	//G_3e = 0x3e,
	//G_3f = 0x3f,
	{MASK_UNK, "unk", 0, 0},
}, m_error_instr = {MASK_ERROR, "unk", 0, 0, 0, 0};
static const u32 instr_count = WXSIZEOF(m_instructions);

Instruction GetInstruction(const wxString& str)
{
	for(u32 i=0; i<instr_count; ++i)
	{
		if(!!m_instructions[i].name.Cmp(str)) continue;
		return m_instructions[i];
	}

	return m_error_instr;
}

s32 SetField(s32 src, u32 from, u32 to)
{
	return (src & ((1 << ((to - from) + 1)) - 1)) << (31 - to);
}

s32 SetField(s32 src, u32 p)
{
	return (src & 0x1) << (31 - p);
}

u32 ToOpcode(s32 i)	{ return SetField(i, 0, 5); }
u32 ToRS(s32 i)		{ return SetField(i, 6, 10); }
u32 ToRT(s32 i)		{ return SetField(i, 6, 10); }
u32 ToRA(s32 i)		{ return SetField(i, 11, 15); }
u32 ToRB(s32 i)		{ return SetField(i, 16, 20); }
u32 ToLL(s32 i)		{ return SetField(i, 6, 31); }
u32 ToAA(s32 i)		{ return SetField(i, 30); }
u32 ToLK(s32 i)		{ return SetField(i, 31); }
u32 ToIMM16(s32 i)	{ return SetField(i, 16, 31); }
u32 ToD(s32 i)		{ return SetField(i, 16, 31); }
u32 ToDS(s32 i)
{
	if(i < 0) return ToD(i + 1);
	return ToD(i);
}
u32 ToSYS(s32 i)	{ return SetField(i, 6, 31); }
u32 ToBF(s32 i)		{ return SetField(i, 6, 10); }
u32 ToBO(s32 i)		{ return SetField(i, 6, 10); }
u32 ToBI(s32 i)		{ return SetField(i, 11, 15); }
u32 ToBD(s32 i)		{ return SetField(i, 16, 31); }
u32 ToMB(s32 i)		{ return SetField(i, 21, 25); }
u32 ToME(s32 i)		{ return SetField(i, 26, 30); }
u32 ToSH(s32 i)		{ return SetField(i, 16, 20); }
u32 ToRC(s32 i)		{ return SetField(i, 31); }
u32 ToOE(s32 i)		{ return SetField(i, 21); }
u32 ToL(s32 i)		{ return SetField(i, 10); }

u32 CompileInstruction(Instruction instr, wxArrayLong& arr)
{
	u32 code = ToOE(instr.oe) | ToRC(instr.rc);
	switch(instr.op_g)
	{
	case 0:
	default:
		code |= ToOpcode(instr.op_num);
	break;
	}

	switch(instr.mask)
	{
	case MASK_RST_RA_IMM16:
		return code | ToRS(arr[0]) | ToRA(arr[1]) | ToIMM16(arr[2]);
	case MASK_RA_RST_IMM16:
		return code | ToRA(arr[0]) | ToRS(arr[1]) | ToIMM16(arr[2]);
	case MASK_BF_L_RA_IMM16:
		return code | ToBF(arr[0]) | ToL(arr[1])  | ToRA(arr[2]) | ToIMM16(arr[3]);
	case MASK_BO_BI_BD_AA_LK:
		return code | ToBO(arr[0]) | ToBI(arr[1]) | ToBD(arr[2]) | ToAA(arr[3]) | ToLK(arr[4]);
	case MASK_SYS: 
		return code | ToSYS(/*arr[0]*/1);
	case MASK_LL_AA_LK:
		return code | ToLL(arr[0]) | ToAA(arr[1]) | ToLK(arr[2]);
	case MASK_RA_RS_SH_MB_ME:
		return code | ToRA(arr[0]) | ToRS(arr[1]) | ToSH(arr[2]) | ToMB(arr[3]) | ToME(arr[4]);
	case MASK_RST_RA_D:
		return code | ToRS(arr[0]) | ToRA(arr[1]) | ToD(arr[2]);
	case MASK_RST_RA_DS:
		return code | ToRS(arr[0]) | ToRA(arr[1]) | ToDS(arr[2]);
	case MASK_UNK:
		return arr[0];
	case MASK_NO_ARG:
		return code;
	}

	return 0; //NOP
}

CompilerELF::CompilerELF(wxWindow* parent) : FrameBase(parent, wxID_ANY, "CompilerELF", wxEmptyString, wxSize(640, 680))
{
	m_aui_mgr.SetManagedWindow(this);

	wxMenuBar& menubar(*new wxMenuBar());
	wxMenu& menu_code(*new wxMenu());
	menubar.Append(&menu_code, "Code");

	//menu_code.Append(id_analyze_code, "Analyze");
	menu_code.Append(id_compile_code, "Compile");
	menu_code.Append(id_load_elf, "Load ELF");

	SetMenuBar(&menubar);

	asm_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxTE_MULTILINE | wxTE_DONTWRAP | wxTE_RICH2);
	hex_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(155, -1),
		wxTE_MULTILINE | wxTE_DONTWRAP | wxTE_READONLY | wxTE_RICH2);    
	err_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(140, 200),
		wxTE_MULTILINE | wxTE_READONLY);

	const wxSize& client_size = GetClientSize();

	m_aui_mgr.AddPane(asm_list, wxAuiPaneInfo().Name(L"asm_list").CenterPane().PaneBorder(false).CloseButton(false));
	m_aui_mgr.AddPane(hex_list, wxAuiPaneInfo().Name(L"hex_list").Left().PaneBorder(false).CloseButton(false));
	m_aui_mgr.AddPane(err_list, wxAuiPaneInfo().Name(L"err_list").Bottom().PaneBorder(false).CloseButton(false));
	m_aui_mgr.GetPane(L"asm_list").Show();
	m_aui_mgr.GetPane(L"hex_list").CaptionVisible(false).Show();
	m_aui_mgr.GetPane(L"err_list").Caption("Errors").Show();
	m_aui_mgr.Update();

	FrameBase::LoadInfo();

	m_update_scroll_timer = new wxTimer(this);
	
	Connect(asm_list->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CompilerELF::OnUpdate));
	Connect(m_update_scroll_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(CompilerELF::OnUpdateScroll));
	Connect(id_analyze_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::AnalyzeCode));
	Connect(id_compile_code, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::CompileCode));
	Connect(id_load_elf,	 wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CompilerELF::LoadElf));

	m_update_scroll_timer->Start(5);
	a_instr = new wxTextAttr(wxColour(0, 160, 0));

	asm_list->SetValue(
		"start_section_info\n"
		"\tname \".text\"\n"
		"\taddr 0x10230\n"
		"\talign 0x4\n"
		"\ttype 1\n"
		"\tflags 6\n"
		"end_section_info\n"
		"\n"
	);
	/*
	asm_list->SetValue(
		"stdu r1,r1,-160\n"
		"std r26,r1,112\n"
		"std r28,r1,128\n"
		"std r30,r1,144\n"
		"std r31,r1,152\n"
		"std r27,r1,120\n"
		"std r29,r1,136\n"
		"std r0,r1,176\n"
		"\n"
		"mr r31,r3\n"
		"mr r28,r4\n"
		"mr r26,r5\n"
		"mr r30,r6\n"
		"mr r29,r12\n"
		"\n"
		"T2M r3, r0, \"outlog.txt\"\n"
		"li r4, 0x000001\n"
		"li r6, 0\n"
		"li r7, 0\n"
		"li r8, 0\n"
		"li r11, 801\n"
		"sc 2\n"
		"\n"
		"mr r0, r5\n"
		"mr r3, r5\n"
		"T2M r4, r5, \"Hello world!\"\n"
		"li r11, 803\n"
		"sc 2\n"
		"\n"
		"mr r3, r0\n"
		"li r11, 804\n"
		"sc 2\n"
	);*/
}

CompilerELF::~CompilerELF()
{
	m_update_scroll_timer->Stop();
	m_aui_mgr.UnInit();
}

void CompilerELF::OnUpdate(wxCommandEvent& WXUNUSED(event))
{
	DoAnalyzeCode(false);
	UpdateScroll();
	//TODO
}

void CompilerELF::OnUpdateScroll(wxTimerEvent& WXUNUSED(event))
{
	UpdateScroll();
}

void CompilerELF::UpdateScroll()
{
	return;
	const int asm_count = asm_list->GetScrollPos(wxVERTICAL);
	const int hex_count = hex_list->GetScrollPos(wxVERTICAL);

	if(asm_count != hex_count) hex_list->ScrollLines(asm_count - hex_count);
}

void CompilerELF::WriteError(const u32 line, const wxString& error)
{
	err_list->WriteText(wxString::Format("line %d: %s\n", line + 1, error));
}

bool Cmp(const wxString& s1, const wxString& s2, u64& from, u64& to)
{
	if(s1.Length() > s2.Length())
	{
		const int pos = s1.Find(s2);
		if(pos == -1) return false;
		from = pos;
		to = pos+s2.Length();
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

bool IsSkipChar(const wxString& str)	{return !str.Cmp("\t") || !str.Cmp(" ");}
bool IsNewLine(const wxString& str)		{return !str.Cmp("\n");}
bool IsBreakChar2(const wxString& str)	{return IsNewLine(str) || !str.Cmp(",") || !str.Cmp("#");}
bool IsBreakChar(const wxString& str)	{return IsSkipChar(str) || IsBreakChar2(str);}
bool IsSkipRange(const wxString& str, const wxString& buf, const u64 p, const u64 len)
{
	return IsSkipChar(str) && buf.IsEmpty() && p != len - 1;
}

bool CompilerELF::SearchReg(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxArrayLong& arr)
{
	wxString buf = wxEmptyString;

	for(p++; p<line_len; p++)
	{
		const wxString& cur_str = line_text(p, 1);
		if(IsSkipRange(cur_str, buf, p, line_len)) continue;

		if(p == line_len - 1 || IsBreakChar(cur_str))
		{
			if(p == line_len-1 && !IsSkipChar(cur_str)) buf += cur_str;

			const int reg = StringToReg(buf);
			if(reg == -1) break;

			arr.Add(reg);
			return true;
		}

		buf += cur_str;
	}

	WriteError(line_num, wxString::Format("Unknown register '%s'", buf));
	return false;
}

bool CompilerELF::SearchImm(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxArrayLong& arr)
{
	wxString buf = wxEmptyString;

	for(p++; p<line_len; p++)
	{
		const wxString& cur_str = line_text(p, 1);
			
		s64 imm;
		if(!buf(0, 1).Cmp("\""))
		{
			if(!cur_str.Cmp("\""))
			{
				for(p++; p<line_len; p++)
				{
					if(!line_text(p, 1).Cmp("#")) break;
					if(!IsSkipChar(cur_str))
					{
						WriteError(line_num, wxString::Format("Unknown symbol '%s'", buf));
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
			if(IsSkipChar(cur_str) && p != line_len - 1) continue;
			if(p == line_len - 1 || !cur_str.Cmp(",") || !cur_str.Cmp("#"))
			{
				if(p == line_len-1 && !IsSkipChar(cur_str)) buf += cur_str;
					
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
		
	WriteError(line_num, wxString::Format("Unknown immediate '%s'", buf));
	return false;
}

bool CompilerELF::SearchText(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxString& dst)
{
	wxString buf = wxEmptyString;

	for(p++; p<line_len; ++p)
	{
		const wxString& cur_str = line_text(p, 1);

		if(!buf(0, 1).Cmp("\""))
		{
			if(!cur_str.Cmp("\""))
			{
				for(p++; p<line_len; p++)
				{
					if(!line_text(p, 1).Cmp("#")) break;
					if(!IsSkipChar(cur_str))
					{
						WriteError(line_num, wxString::Format("Unknown symbol '%s'", buf));
						return false;
					}
				}
				
				dst = buf(1, buf.Length()).c_str();
				return true;
			}
		}
		else
		{
			if(IsSkipChar(cur_str) && p != line_len - 1) continue;
		}

		buf += cur_str;
	}

	WriteError(line_num, wxString::Format("Text is not found! (%s)", buf));
	return false;
}

void WriteEhdr(wxFile& f, Elf64_Ehdr& ehdr)
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

void WritePhdr(wxFile& f, Elf64_Phdr& phdr)
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

void WriteShdr(wxFile& f, Elf64_Shdr& shdr)
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

void CompilerELF::LoadElf(wxCommandEvent& event)
{
	wxFileDialog ctrl(this, L"Select ELF", wxEmptyString, wxEmptyString,
		"Elf Files (*.elf, *.bin)|*.elf;*.bin|"
		"All Files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL) return;
	LoadElf(ctrl.GetPath());
}

#include "Emu/Cell/PPUDisAsm.h"
#include "Emu/Cell/PPUDecoder.h"

void CompilerELF::LoadElf(const wxString& path)
{
	if(!wxFile::Access(path, wxFile::read))
	{
		ConLog.Error("Error opening '%s'", path);
		return;
	}

	Memory.Init();

	wxFile f(path);
	ELF64Loader l(f);
	if(!l.Load())
	{
		ConLog.Error("Error loading '%s'", path);
		Memory.Close();
		l.Close();
		return;
	}

	Memory.Close();
	
	Sleep(10);
	asm_list->SetLabel(wxEmptyString);
	asm_list->Freeze();
	PPU_DisAsm disasm(*(PPCThread*)0, CompilerElfMode);
	PPU_Decoder dec(disasm);

	asm_list->WriteText("start_elf_info\n");
	asm_list->WriteText(wxString::Format("\tentry 0x%x\n", l.ehdr.e_entry));
	asm_list->WriteText(wxString::Format("\tshoff 0x%x\n", l.ehdr.e_shoff));
	asm_list->WriteText(wxString::Format("\tcurver 0x%x\n", l.ehdr.e_curver));
	asm_list->WriteText(wxString::Format("\tversion 0x%x\n", l.ehdr.e_version));
	asm_list->WriteText(wxString::Format("\tflags 0x%x\n", l.ehdr.e_flags));
	asm_list->WriteText("end_elf_info\n");
	asm_list->WriteText("\n");

	Sleep(5);

	for(u32 p=0; p<l.phdr_arr.GetCount(); ++p)
	{
		asm_list->WriteText("start_program_info\n");

		if(l.phdr_arr[p].p_vaddr != l.phdr_arr[p].p_paddr)
		{
			asm_list->WriteText(wxString::Format("\tvaddr 0x%llx\n", l.phdr_arr[p].p_vaddr));
			asm_list->WriteText(wxString::Format("\tpaddr 0x%llx\n", l.phdr_arr[p].p_paddr));
		}
		else
		{
			asm_list->WriteText(wxString::Format("\taddr 0x%llx\n", l.phdr_arr[p].p_vaddr));
		}

		if(l.phdr_arr[p].p_filesz != l.phdr_arr[p].p_memsz) 
			asm_list->WriteText(wxString::Format("\tmemsz 0x%llx\n", l.phdr_arr[p].p_memsz));

		asm_list->WriteText(wxString::Format("\ttype 0x%x\n", l.phdr_arr[p].p_type));
		asm_list->WriteText(wxString::Format("\tflags 0x%x\n", l.phdr_arr[p].p_flags));
		asm_list->WriteText(wxString::Format("\talign 0x%llx\n", l.phdr_arr[p].p_align));
		asm_list->WriteText("end_program_info\n");
		asm_list->WriteText("\n");

		if(!l.phdr_arr[p].p_offset) continue;
		if(!l.phdr_arr[p].p_vaddr) continue;
		f.Seek(l.phdr_arr[p].p_offset);
		for(u32 o=0; o<l.phdr_arr[p].p_filesz;)
		{
			if(l.phdr_arr[p].p_align < 4)
			{
				switch(l.phdr_arr[p].p_align)
				{
					case 1:
					{
						const char c = Read8(f);
						if(c) asm_list->WriteText(wxString::Format("unk \"%c\"\n", c));
						else asm_list->WriteText(wxString::Format("unk 0x%02x\n", c));
					}
					break;

					case 2:
						asm_list->WriteText(wxString::Format("unk 0x%04x\n", Read16(f)));
					break;

					default:
						ConLog.Error("Bad program align! program: %d, align: %lld", p, l.phdr_arr[p].p_align);
					break;
				}

				o += l.phdr_arr[p].p_align;
				continue;
			}

			const u32 code = Read32(f);
			dec.Decode(code);
			wxString opcode = wxEmptyString;
			for(u32 i=0; i<disasm.last_opcode.Length(); ++i)
			{
				const wxString& s = disasm.last_opcode[i];
				if(s == "\n" || s == " " || s == "#") break;
				opcode += s;
			}
			Instruction instr = GetInstruction(opcode);
			if(instr.mask == MASK_ERROR)
			{
				asm_list->WriteText(wxString::Format("unk 0x%x\n", code));
			}
			else
			{
				hex_list->SetInsertionPointEnd();
				for(u32 j=0; j<disasm.last_opcode.Len(); ++j) *asm_list << disasm.last_opcode[j];
			}

			o += 4;
		}

		asm_list->WriteText("\n");
	}

	Sleep(5);

	for(u32 i=0; i<l.shdr_arr.GetCount(); ++i)
	{
		if(l.shdr_name_arr.GetCount() <= i)
		{
			ConLog.Error("Error loading section names");
			asm_list->SetLabel(wxEmptyString);
			l.Close();
			asm_list->Thaw();
			return;
		}

		if(!l.shdr_name_arr[i].Length()) continue;
		if(i == l.ehdr.e_shstrndx) continue;

		bool data_in_program = false;
		for(u32 p=0; p<l.phdr_arr.GetCount(); ++p)
		{
			if(!l.phdr_arr[p].p_offset) continue;
			if(!l.phdr_arr[p].p_vaddr) continue;

			if(!(l.phdr_arr[p].p_vaddr <= l.shdr_arr[i].sh_addr &&
				l.phdr_arr[p].p_vaddr + l.phdr_arr[p].p_filesz >= l.shdr_arr[i].sh_addr + l.shdr_arr[i].sh_size)) continue;
			data_in_program = true;
			break;
		}

		asm_list->WriteText("start_section_info\n");
		asm_list->WriteText(wxString::Format("\tname \"%s\"\n", l.shdr_name_arr[i]));
		asm_list->WriteText(wxString::Format("\taddr 0x%llx\n", l.shdr_arr[i].sh_addr));
		if(data_in_program) asm_list->WriteText(wxString::Format("\tsize 0x%llx\n", l.shdr_arr[i].sh_size));
		asm_list->WriteText(wxString::Format("\talign %lld\n", l.shdr_arr[i].sh_addralign));
		asm_list->WriteText(wxString::Format("\ttype %d\n", l.shdr_arr[i].sh_type));
		asm_list->WriteText(wxString::Format("\tflags %lld\n", l.shdr_arr[i].sh_flags));
		asm_list->WriteText("end_section_info\n");
		asm_list->WriteText("\n");

		if(data_in_program) continue;
		f.Seek(l.shdr_arr[i].sh_offset);

		for(u32 o = 0; o<l.shdr_arr[i].sh_size;)
		{
			if(l.shdr_arr[i].sh_addralign < 4)
			{
				switch(l.shdr_arr[i].sh_addralign)
				{
					case 1:
					{
						const char c = Read8(f);
						if(c) asm_list->WriteText(wxString::Format("unk \"%c\"\n", c));
						else asm_list->WriteText(wxString::Format("unk 0x%02x\n", c));
					}
					break;

					case 2:
						asm_list->WriteText(wxString::Format("unk 0x%04x\n", Read16(f)));
					break;

					default:
						ConLog.Error("Bad section align! section: \"%s\"[%d], align: %lld", l.shdr_name_arr[i], i, l.shdr_arr[i].sh_addralign);
					break;
				}

				o += l.shdr_arr[i].sh_addralign;
			}
			else
			{
				const u32 code = Read32(f);
				dec.Decode(code);
				wxString opcode = wxEmptyString;
				for(u32 i=0; i<disasm.last_opcode.Length(); ++i)
				{
					const wxString& s = disasm.last_opcode[i];
					if(s == "\n" || s == " " || s == "#") break;
					opcode += s;
				}
				Instruction instr = GetInstruction(opcode);
				if(instr.mask == MASK_ERROR)
				{
					asm_list->WriteText(wxString::Format("unk 0x%x\n", code));
				}
				else
					asm_list->WriteText(disasm.last_opcode);

				o += 4;
			}
		}

		asm_list->WriteText("\n");
	}

	asm_list->Thaw();
	l.Close();
}

enum
{
	LNTYPE_NULL,
	LNTYPE_COMMIT,
	LNTYPE_INSTR,
	LNTYPE_SECTION,
	LNTYPE_PROGRAM,
	LNTYPE_ELF,
};


bool CmpLine(const wxString& line_text, const wxString& text)
{
	const u32 line_len = line_text.Length();
	wxString buf = wxEmptyString;

	for(u64 p=0; p<line_len; ++p)
	{
		const wxString& cur_str = line_text(p, 1);

		const bool skip = cur_str == " " || cur_str == "\t";
		const bool end_ln = p == line_len - 1;
		const bool stop = cur_str == "#" || end_ln;

		if(!buf.IsEmpty())
		{
			if(skip || stop)
			{
				if(end_ln) buf += cur_str;
				return buf == text;
			}
		}

		if(skip) continue;
		if(stop)
		{
			if(end_ln) buf += cur_str;
			return buf == text;
		}

		buf += cur_str;
	}

	return false;
}

u32 SearchLineType(const wxString& line_text)
{
	const u32 line_len = line_text.Length();

	for(u64 p=0; p<line_len; ++p)
	{
		const wxString& cur_str = line_text(p, 1);
		if(cur_str == " " || cur_str == "\t") continue;
		if(cur_str == "#") return LNTYPE_COMMIT;
		if(CmpLine(line_text(p, line_len), "start_section_info")) return LNTYPE_SECTION;
		if(CmpLine(line_text(p, line_len), "start_program_info")) return LNTYPE_PROGRAM;
		if(CmpLine(line_text(p, line_len), "start_elf_info")) return LNTYPE_ELF;
		return LNTYPE_INSTR;
	}

	return LNTYPE_NULL;
}

void WriteLine(wxTextCtrl* hex_list, int line, const wxString& text)
{
	if(hex_list->GetNumberOfLines() <= line)
	{
		while(hex_list->GetNumberOfLines() <= line)
		{
			hex_list->SetInsertionPointEnd();
			hex_list->WriteText("\n");
		}

		hex_list->SetInsertionPointEnd();
		hex_list->WriteText(text);
		return;
	}

	const wxString& label = hex_list->GetLabel();
	u32 cl=0;
	for(u32 p=0; p<label.Length(); ++p)
	{
		if(cl != line)
		{
			if(label[p] == '\n') cl++;
			continue;
		}

		u32 pend;
		for(pend = p + 1; pend<label.Length(); ++pend)
		{
			if(label[pend] == '\n') break;
		}

		if(pend - 1 != p)
		{
			pend = pend == label.Length() ? pend : pend - 1;
			if(!!label(p, pend - p).Cmp(text) && text.Length()) return;
			hex_list->Remove(p, pend);
		}

		hex_list->SetInsertionPoint(p);
		hex_list->WriteText(text);
		return;
	}
	hex_list->SetInsertionPointEnd();
	hex_list->WriteText(text);
}

void SetLineCount(wxTextCtrl* hex_list, int count)
{
	const wxString& label = hex_list->GetLabel();

	if(hex_list->GetNumberOfLines() <= count) return;

	int cl = hex_list->GetNumberOfLines();
	for(u32 p=label.Length() - 1; p; --p)
	{
		if(cl != count)
		{
			if(label[p] == '\n') cl--;
			continue;
		}

		hex_list->Remove(p + 1, label.Length());
		return;
	}
}

int CompilerELF::CompileLine_Instr(wxString line_text, const u32 line_num, const u32 line_len, bool& has_error)
{
	wxString buf = wxEmptyString;
	has_error = false;

	for(u64 p=0; p<line_len; ++p)
	{
		const wxString cur_str = line_text(p, 1);

		const bool skip = !cur_str.Cmp(" ") || !cur_str.Cmp("\t");
		const bool commit = !cur_str.Cmp("#");
		const bool end_line = p == line_len - 1;
		const bool buf_has_data = buf.Length() > 0;

		const bool break_search = commit || end_line;
		const bool skip_search = skip;
		if(break_search && !buf_has_data) break;
		if(skip_search && !buf_has_data) continue;

		if(skip_search || break_search)
		{
			if(!IsSkipChar(cur_str) && p == line_len - 1) buf += cur_str;

			const wxString& opcode = buf;
			wxArrayLong arr;
			arr.Clear();

			Instruction instr = GetInstruction(opcode);
#define SEARCH_ERROR \
	{ \
		has_error = true; \
		break; \
	} \

#define SEARCH_REG if(!SearchReg(p, line_text, line_num, line_len, arr)) SEARCH_ERROR
#define SEARCH_IMM if(!SearchImm(p, line_text, line_num, line_len, arr)) SEARCH_ERROR

			switch(instr.mask)
			{
			case MASK_NO_ARG:
			break;

			case MASK_UNK:
				SEARCH_IMM;
			break;

			case MASK_RST_RA_D:
			case MASK_RST_RA_DS:
			case MASK_RST_RA_IMM16:
			case MASK_RA_RST_IMM16:
				SEARCH_REG;
				SEARCH_REG;
				SEARCH_IMM;
			break;

			case MASK_BF_L_RA_IMM16:
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_REG;
				SEARCH_IMM;
			break;

			case MASK_BO_BI_BD_AA_LK:
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_IMM;
			break;

			case MASK_SYS:
				//SEARCH_IMM;
			break;

			case MASK_LL_AA_LK:
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_IMM;
			break;

			case MASK_RA_RS_SH_MB_ME:
				SEARCH_REG;
				SEARCH_REG;
				SEARCH_IMM;
				SEARCH_IMM;
				SEARCH_IMM;
			break;

			case MASK_ERROR:
			default:
				WriteError(line_num, wxString::Format("Unknown opcode '%s'", opcode));
				has_error = true;
			break;
			}

			if(has_error) return -1;
			return CompileInstruction(instr, arr);
		}

		buf += cur_str;
	}

	WriteError(line_num, wxString::Format("Unknown opcode '%s'", line_text));
	has_error = true;
	return -1;
}

void CompilerELF::CompileLine_Section(Array<SectionInfo>& sections_info, u64& errors_count, u32& addr, u32& current_section, u32& line_num, const u32 lines_count)
{
	bool end_search = false;
	SectionInfo info;
	memset(&info.shdr, 0, sizeof(info.shdr));
	info.shdr.sh_addr = 0x10200;
	info.shdr.sh_addralign = 4;
	for(line_num++; line_num<lines_count && !end_search; ++line_num)
	{
		WriteLine(hex_list, line_num, wxEmptyString);
		wxString buf = wxEmptyString;
		const wxString& line_text = asm_list->GetLineText(line_num);
		const u32 line_len = line_text.length();
		for(u64 p=0; p<line_len; ++p)
		{
			const wxString& cur_str = line_text(p, 1);
			const bool skip = cur_str == " " || cur_str == "\t";
			const bool commit = cur_str == "#";
			const bool end_ln = p == line_len - 1;
			const bool is_buf_empty = buf.Length() == 0;
			if(is_buf_empty)
			{
				if(skip) continue;
				if(commit || end_ln) break;
			}
			else if(skip || commit || end_ln)
			{
				if(end_ln) buf += cur_str;
						
				if(!buf.Cmp("end_section_info"))
				{
					addr = info.shdr.sh_addr;
					current_section = sections_info.GetCount();
					sections_info.AddCpy(info);
					end_search = true;
				}
				else if(!buf.Cmp("name"))
				{
					SearchText(p, line_text, line_num, line_len, info.name);
				}
				else if(!buf.Cmp("addr"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_addr = imm[0];
				}
				else if(!buf.Cmp("size"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_size = imm[0];
				}
				else if(!buf.Cmp("offset"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_offset = imm[0];
				}
				else if(!buf.Cmp("align"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_addralign = imm[0];
				}
				else if(!buf.Cmp("flags"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_flags = imm[0];
				}
				else if(!buf.Cmp("type"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_type = imm[0];
				}
				else if(!buf.Cmp("info"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}

					info.shdr.sh_info = imm[0];
				}
				else
				{
					WriteError(line_num, wxString::Format("Unknown section info command '%s'", buf));
					errors_count++;
					end_search = true;
				}

				break;
			}

			buf += cur_str;
		}
	}

	if(end_search) line_num--;
}

void CompilerELF::CompileLine_Program(Array<ProgramInfo>& programs_info, u64& errors_count, u32& addr, u32& current_program, u32& line_num, const u32 lines_count)
{
	bool end_search = false;
	ProgramInfo info;
	memset(&info.phdr, 0, sizeof(info.phdr));
	info.phdr.p_type = 0x00000001;
	info.phdr.p_vaddr = info.phdr.p_paddr = 0x10200;

	for(line_num++; line_num<lines_count && !end_search; ++line_num)
	{
		WriteLine(hex_list, line_num, wxEmptyString);
		wxString buf = wxEmptyString;
		const wxString& line_text = asm_list->GetLineText(line_num);
		const u32 line_len = line_text.length();
		for(u64 p=0; p<line_len; ++p)
		{
			const wxString& cur_str = line_text(p, 1);
			const bool skip = cur_str == " " || cur_str == "\t";
			const bool commit = cur_str == "#";
			const bool end_ln = p == line_len - 1;
			const bool is_buf_empty = buf.Length() == 0;
			if(is_buf_empty)
			{
				if(skip) continue;
				if(commit || end_ln) break;
			}
			else if(skip || commit || end_ln)
			{
				if(end_ln) buf += cur_str;
						
				if(!buf.Cmp("end_program_info"))
				{
					addr = info.phdr.p_vaddr;
					current_program = programs_info.GetCount();
					programs_info.AddCpy(info);
					end_search = true;
				}
				else if(!buf.Cmp("align"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_align = imm[0];
				}
				else if(!buf.Cmp("filesz"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_filesz = imm[0];
				}
				else if(!buf.Cmp("memsz"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_memsz = imm[0];
				}
				else if(!buf.Cmp("vaddr"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_vaddr = imm[0];
				}
				else if(!buf.Cmp("paddr"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_paddr = imm[0];
				}
				else if(!buf.Cmp("addr"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_vaddr = info.phdr.p_paddr = imm[0];
				}
				else if(!buf.Cmp("type"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_type = imm[0];
				}
				else if(!buf.Cmp("flags"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					info.phdr.p_flags = imm[0];
				}
				else
				{
					WriteError(line_num, wxString::Format("Unknown program info command '%s'", buf));
					errors_count++;
					end_search = true;
				}

				break;
			}

			buf += cur_str;
		}
	}

	if(end_search) line_num--;
}

void CompilerELF::CompileLine_Elf(Elf64_Ehdr& elf_info, u64& errors_count, u32& line_num, const u32 lines_count)
{
	bool end_search = false;

	for(line_num++; line_num<lines_count && !end_search; ++line_num)
	{
		WriteLine(hex_list, line_num, wxEmptyString);
		wxString buf = wxEmptyString;
		const wxString& line_text = asm_list->GetLineText(line_num);
		const u32 line_len = line_text.length();
		for(u64 p=0; p<line_len; ++p)
		{
			const wxString& cur_str = line_text(p, 1);
			const bool skip = cur_str == " " || cur_str == "\t";
			const bool commit = cur_str == "#";
			const bool end_ln = p == line_len - 1;
			const bool is_buf_empty = buf.Length() == 0;
			if(is_buf_empty)
			{
				if(skip) continue;
				if(commit || end_ln) break;
			}
			else if(skip || commit || end_ln)
			{
				if(end_ln) buf += cur_str;
						
				if(!buf.Cmp("end_elf_info"))
				{
					end_search = true;
				}
				else if(!buf.Cmp("abi_ver"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_abi_ver = imm[0];
				}
				else if(!buf.Cmp("curver"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_curver = imm[0];
				}
				else if(!buf.Cmp("version"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_version = imm[0];
				}
				else if(!buf.Cmp("entry"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_entry = imm[0];
				}
				else if(!buf.Cmp("flags"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_flags = imm[0];
				}
				else if(!buf.Cmp("shoff"))
				{
					wxArrayLong imm;
					imm.Clear();
					if(!SearchImm(p, line_text, line_num, line_len, imm))
					{
						errors_count++;
						break;
					}
					elf_info.e_shoff = imm[0];
				}
				else
				{
					WriteError(line_num, wxString::Format("Unknown elf info command '%s'", buf));
					errors_count++;
					end_search = true;
				}

				break;
			}

			buf += cur_str;
		}
	}

	if(end_search) line_num--;
}

#include <Utilites/MTProgressDialog.h>

void CompilerELF::DoAnalyzeCode(bool compile)
{
	if(!compile) return;
	const uint lines_count = (uint)asm_list->GetNumberOfLines();
	u64 errors_count = 0;

	//hex_list->Clear();
	err_list->Clear();
	hex_list->Freeze();

	Array<SectionInfo> sections_info;
	Array<ProgramInfo> programs_info;
	Elf64_Ehdr elf_info;
	sections_info.Clear();
	programs_info.Clear();
	memset(&elf_info, 0, sizeof(Elf64_Ehdr));
	u32 current_section = 0;
	u32 current_program = 0;
	bool store_in_program = true;

	elf_info.e_magic = 0x7F454C46;
	elf_info.e_curver = 0x1;
	elf_info.e_version = 0x1;
	elf_info.e_class = 0x1;
	elf_info.e_magic = 0x7F454C46;
	elf_info.e_class = 2; //ELF64
	elf_info.e_data = 2;
	elf_info.e_curver = 1; //ver 1
	elf_info.e_os_abi = 0x66; //Cell OS LV-2
	elf_info.e_abi_ver = 0;
	elf_info.e_type = 2; //EXEC (Executable file)
	elf_info.e_machine = MACHINE_PPC64; //PowerPC64
	elf_info.e_version = 1; //ver 1
	elf_info.e_entry = 0x10200;
	elf_info.e_phoff = sizeof(Elf64_Ehdr);
	elf_info.e_shoff = 0;
	elf_info.e_flags = 0x0;
	elf_info.e_ehsize = sizeof(Elf64_Ehdr);
	elf_info.e_phentsize = sizeof(Elf64_Phdr);
	elf_info.e_shentsize = sizeof(Elf64_Shdr);
	elf_info.e_shstrndx = 1;

	MTProgressDialog* progress_dial = 
		new MTProgressDialog(this, wxDefaultSize, "Analyze code...", "Loading...", wxArrayLong(), 1);

	progress_dial->SetMaxFor(0, lines_count);

	for(u32 l=0, addr=0; l<lines_count; ++l)
	{
		WriteLine(hex_list, l, wxEmptyString);
		//if(line > 0) hex_list->WriteText("\n");
		if(compile && errors_count > 0) break;
		wxString buf = wxEmptyString;
		wxString line_text = asm_list->GetLineText(l);
		progress_dial->Update(0, l, wxString::Format("Line %d of %d", l + 1, lines_count));
		const u32 type = SearchLineType(line_text);
		switch(type)
		{
			case LNTYPE_NULL: case LNTYPE_COMMIT: continue;
			case LNTYPE_INSTR:
			{
				bool has_error;
				const u32 compiled_code = CompileLine_Instr(line_text, l, line_text.Length(), has_error);

				if(has_error) errors_count++;
				if(errors_count) continue;

				if(!store_in_program && current_section >= sections_info.GetCount())
				{
					WriteError(l, "Section info not found");
					errors_count++;
					continue;
				}
				else if(store_in_program && current_program >= programs_info.GetCount())
				{
					WriteError(l, "Program info not found");
					errors_count++;
					continue;
				}

				WriteLine(hex_list, l, wxString::Format("%x: [%08x]", addr, compiled_code));

				if(compile)
				{
					Array<u32>& code = !store_in_program ? sections_info[current_section].code : programs_info[current_program].code;
					const u32 align = !store_in_program ? sections_info[current_section].shdr.sh_addralign : programs_info[current_program].phdr.p_align;
					code.AddCpy(compiled_code);
					addr += min<int>(align, 4);
				}
			}
			break;

			case LNTYPE_SECTION:
				CompileLine_Section(sections_info, errors_count, addr, current_section, l, lines_count);
				store_in_program = false;
			break;

			case LNTYPE_PROGRAM:
				CompileLine_Program(programs_info, errors_count, addr, current_program, l, lines_count);
				store_in_program = true;
			break;

			case LNTYPE_ELF:
				CompileLine_Elf(elf_info, errors_count, l, lines_count);
			continue;
		}
	}

	SetLineCount(hex_list, lines_count);
	hex_list->Thaw();
	progress_dial->Close();
	if(!compile) return;

	wxFile f("compiled.elf", wxFile::write);

	{
		SectionInfo shstrtab;
		shstrtab.name = ".shstrtab";
		shstrtab.shdr.sh_type = 3;
		shstrtab.shdr.sh_addralign = 1;
		sections_info.AddCpy(shstrtab);
	}

	elf_info.e_phnum = programs_info.GetCount();
	elf_info.e_shnum = sections_info.GetCount();
	elf_info.e_shstrndx = sections_info.GetCount() - 1;
	WriteEhdr(f, elf_info);

	u32 hdr_offset = elf_info.e_phoff + sizeof(Elf64_Phdr) * elf_info.e_phnum;

	f.Seek(elf_info.e_phoff);
	for(u32 i=0; i<programs_info.GetCount(); ++i)
	{
		programs_info[i].phdr.p_filesz = programs_info[i].code.GetCount() * min<int>(programs_info[i].phdr.p_align, 4);
		programs_info[i].phdr.p_offset = hdr_offset;
		hdr_offset += programs_info[i].phdr.p_filesz;
		WritePhdr(f, programs_info[i].phdr);
	}

	f.Seek(elf_info.e_shoff);
	u32 str_len = 0;
	for(u32 i=0; i<sections_info.GetCount(); ++i)
	{
		sections_info[i].shdr.sh_name = str_len;
		str_len += sections_info[i].name.Len() + 1;

		if(i == elf_info.e_shstrndx)
		{
			sections_info[i].shdr.sh_size = str_len;
			sections_info[i].shdr.sh_offset = hdr_offset;
			hdr_offset += str_len;
		}
		else
		{
			if(sections_info[i].code.GetCount())
			{
				sections_info[i].shdr.sh_size = sections_info[i].code.GetCount() * min<int>(sections_info[i].shdr.sh_addralign, 4);
				sections_info[i].shdr.sh_offset = hdr_offset;
				hdr_offset += sections_info[i].shdr.sh_size;
			}
			else
			{
				sections_info[i].shdr.sh_offset = 0;
				for(u32 p=0; p<programs_info.GetCount(); ++p)
				{
					if(!programs_info[p].phdr.p_offset) continue;
					if(!programs_info[p].phdr.p_vaddr) continue;

					if(!(programs_info[p].phdr.p_vaddr <= sections_info[i].shdr.sh_addr &&
						programs_info[p].phdr.p_vaddr + programs_info[p].phdr.p_filesz >= sections_info[i].shdr.sh_addr + sections_info[i].shdr.sh_size))
						continue;

					sections_info[i].shdr.sh_offset = 
						programs_info[p].phdr.p_offset + (sections_info[i].shdr.sh_addr - programs_info[p].phdr.p_vaddr);
					break;
				}

				if(!sections_info[i].shdr.sh_offset) sections_info[i].shdr.sh_offset = hdr_offset;
			}
		}

		WriteShdr(f, sections_info[i].shdr);
	}

	f.Seek(sections_info[elf_info.e_shstrndx].shdr.sh_offset);
	for(u32 i=0; i<sections_info.GetCount(); ++i)
	{
		f.Write(sections_info[i].name);
		f.Seek(f.Tell() + 1);
	}

	for(u32 i=0; i<sections_info.GetCount(); ++i)
	{
		f.Seek(sections_info[i].shdr.sh_offset);
		for(u32 c=0; c<sections_info[i].code.GetCount();)
		{
			switch(min<int>(sections_info[i].shdr.sh_addralign, 4))
			{
			case 1: Write8 (f, sections_info[i].code[c]); c += 1; break;
			case 2: Write16(f, sections_info[i].code[c]); c += 2; break;
			case 4: Write32(f, sections_info[i].code[c]); c += 4; break;
			}
		}
	}

	for(u32 i=0; i<programs_info.GetCount(); ++i)
	{
		f.Seek(programs_info[i].phdr.p_offset);
		for(u32 c=0; c<programs_info[i].code.GetCount();)
		{
			switch(min<int>(programs_info[i].phdr.p_align, 4))
			{
			case 1: Write8 (f, programs_info[i].code[c]); c += 1; break;
			case 2: Write16(f, programs_info[i].code[c]); c += 2; break;
			case 4: Write32(f, programs_info[i].code[c]); c += 4; break;
			}
		}
	}

	f.Close();

	wxMessageBox("elf saved.");
}