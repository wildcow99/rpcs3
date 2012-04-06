#pragma once
#include "Emu/Cell/PPCThread.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUDisAsm.h"
#include "Emu/Cell/SPUDecoder.h"
#include "Emu/Cell/SPUDisAsm.h"

class InterpreterDisAsmFrame : public FrameBase
{
	wxListView* m_list;
	wxPanel& m_main_panel;
	PPCThread& CPU;
	DisAsm* disasm;
	Decoder* decoder;
	u32 PC;
	wxTextCtrl* m_regs;
	bool exit;
	Array<u32> m_break_points;

public:
	InterpreterDisAsmFrame(const wxString& title, PPCThread* cpu);

	virtual void Close(bool force = false);
	virtual void OnKeyDown(wxKeyEvent& event);
	void DoUpdate();
	void ShowPc(const int pc);
	void WriteRegs();
	virtual void OnUpdate(wxCommandEvent& event);
	virtual void Show_Val(wxCommandEvent& event);
	virtual void Show_PC(wxCommandEvent& event);
	virtual void DoRun(wxCommandEvent& event);
	virtual void DoStep(wxCommandEvent& event);
	virtual void DClick(wxListEvent& event);
	void OnResize(wxSizeEvent& event);
	void MouseWheel(wxMouseEvent& event);
	bool IsBreakPoint(u32 pc);
	void AddBreakPoint(u32 pc);
	bool RemoveBreakPoint(u32 pc);
};