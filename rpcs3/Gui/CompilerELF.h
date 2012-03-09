#pragma once

#include "Emu/Cell/PPUOpcodes.h"
#include "wx/aui/aui.h"
#include "Loader/ELF64.h"

void Write8(wxFile& f, const u8 data);
void Write16(wxFile& f, const u16 data);
void Write32(wxFile& f, const u32 data);
void Write64(wxFile& f, const u64 data);

struct SectionInfo
{
	Elf64_Shdr shdr;
	wxString name;
	Array<u32> code;

	SectionInfo() : name(".unknown")
	{
		code.Clear();
		memset(&shdr, 0, sizeof(Elf64_Shdr));
		shdr.sh_addr = 0;
		shdr.sh_addralign = 0;
	}
};

struct ProgramInfo
{
	Array<u32> code;
	Elf64_Phdr phdr;
	bool is_preload;

	ProgramInfo()
	{
		is_preload = false;
		code.Clear();
		memset(&phdr, 0, sizeof(Elf64_Phdr));
	}
};

class CompilerELF : public FrameBase
{
	wxTextAttr* a_instr;
	wxAuiManager m_aui_mgr;
	wxTimer* m_update_scroll_timer;

public:
	CompilerELF(wxWindow* parent);
	~CompilerELF();

	wxTextCtrl* asm_list;
	wxTextCtrl* hex_list;
	wxTextCtrl* err_list;
	
	void OnUpdate(wxCommandEvent& event);
	void OnUpdateScroll(wxTimerEvent& event);
	void UpdateScroll();
	void WriteError(const u32 line, const wxString& error);

	void AnalyzeCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(false);
	}

	void CompileCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(true);
	}

	bool SearchReg(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxArrayLong& arr);
	bool SearchText(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxString& dst);
	bool SearchImm(u64& p, const wxString& line_text, const u32 line_num, const u32 line_len, wxArrayLong& arr);

	int CompileLine_Instr(wxString line_text, const u32 line_num, const u32 line_len, bool& has_error);
	void CompileLine_Section(Array<SectionInfo>& sections_info, u64& errors_count, u32& addr, u32& current_section, u32& line_num, const u32 lines_count);
	void CompileLine_Program(Array<ProgramInfo>& programs_info, u64& errors_count, u32& addr, u32& current_program, u32& line_num, const u32 lines_count);
	void CompileLine_Elf(Elf64_Ehdr& elf_info, u64& errors_count, u32& line_num, const u32 lines_count);

	void LoadElf(wxCommandEvent& event);
	void LoadElf(const wxString& path);

	void DoAnalyzeCode(bool compile);
};