#pragma once
#include "Emu/Cell/PPCThread.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUDisAsm.h"
#include "Emu/Cell/SPUDecoder.h"
#include "Emu/Cell/SPUDisAsm.h"

class InterpreterDisAsmFrame
	: public FrameBase
	, public StepThread
{
	wxListView* m_list;
	wxPanel& m_main_panel;
	PPCThread& CPU;
	DisAsm* disasm;
	Decoder* decoder;
	u64 PC;
	wxTextCtrl* m_regs;
	bool exit;
	Array<u64> m_break_points;
	wxButton* m_btn_step;
	wxButton* m_btn_run;
	wxButton* m_btn_pause;

public:
	InterpreterDisAsmFrame(const wxString& title, PPCThread* cpu);

	virtual void Close(bool force = false);
	virtual void OnKeyDown(wxKeyEvent& event);
	void DoUpdate();
	void ShowPc(const u64 pc);
	void WriteRegs();
	virtual void OnUpdate(wxCommandEvent& event);
	virtual void Show_Val(wxCommandEvent& event);
	virtual void Show_PC(wxCommandEvent& event);
	virtual void DoRun(wxCommandEvent& event);
	virtual void DoPause(wxCommandEvent& event);
	virtual void DoStep(wxCommandEvent& event);
	virtual void DClick(wxListEvent& event);
	void OnResize(wxSizeEvent& event);
	void MouseWheel(wxMouseEvent& event);
	bool IsBreakPoint(u64 pc);
	void AddBreakPoint(u64 pc);
	bool RemoveBreakPoint(u64 pc);

	virtual void Step();
};