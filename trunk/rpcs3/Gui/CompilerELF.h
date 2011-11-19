#pragma once

#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/ElfLoader.h"
#include "wx/aui/aui.h"

void Write8(wxFile& f, const u8 data);
void Write16(wxFile& f, const u16 data);
void Write32(wxFile& f, const u32 data);
void Write64(wxFile& f, const u64 data);

class CompilerELF : public wxFrame
{
	wxTextCtrl* asm_list;
	wxTextCtrl* hex_list;
	wxTextCtrl* err_list;

	wxTimer* scroll_timer;

	wxTextAttr* a_instr;
	wxAuiManager m_aui_mgr;

public:
	CompilerELF(wxWindow* parent);
	~CompilerELF();

	wxFont GetFont(int size)
	{
		return wxFont(size, wxMODERN, wxNORMAL, wxNORMAL);
	}
	
	void UpdateScroll();

	void OnUpdateScroll(wxTimerEvent& event);
	void OnUpdate(wxCommandEvent& event);
	void WriteError(const wxString& error, const uint line);

	void AnalyzeCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(false);
	}

	void CompileCode(wxCommandEvent& WXUNUSED(event))
	{
		DoAnalyzeCode(true);
	}

	bool SearchReg(u64& p, const wxString& str, const u64 line, wxArrayLong& arr);
	bool SearchText(u64& p, const wxString& str, const u64 line, wxString& dst);
	bool SearchImm(u64& p, const wxString& str, const u64 line, wxArrayLong& arr);

	void DoAnalyzeCode(bool compile);
};