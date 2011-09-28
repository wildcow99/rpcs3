#pragma once
#include "Emu/Cell/CPU.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUDisAsm.h"

class InterpreterDisAsmFrame : public wxFrame
{
	wxListView* m_list;
	wxPanel& m_main_panel;
	PPUThread& CPU;
	PPU_DisAsm* disasm;
	PPU_Decoder* decoder;
	u32 PC;
	wxTextCtrl* m_regs;

	static const int show_lines = 40;

public:
	InterpreterDisAsmFrame(const wxString& title, PPUThread* cpu)
		: wxFrame(NULL, wxID_ANY, title)
		, m_main_panel(*new wxPanel(this))
		, CPU(*cpu)
		, PC(0)
	{
		disasm = new PPU_DisAsm(&CPU, true);
		decoder = new PPU_Decoder(*disasm);
		wxBoxSizer& s_p_main = *new wxBoxSizer(wxVERTICAL);
		wxBoxSizer& s_b_main = *new wxBoxSizer(wxHORIZONTAL);

		m_list = new wxListView(&m_main_panel);
		wxButton& b_show_PC = *new wxButton(&m_main_panel, wxID_ANY, "Show PC");
		wxButton& b_show_nPC = *new wxButton(&m_main_panel, wxID_ANY, "Show nPC");
		wxButton& b_next_opcode = *new wxButton(&m_main_panel, wxID_ANY, "Do next opcode");
		s_b_main.Add(&b_show_PC);
		s_b_main.AddSpacer(5);
		s_b_main.Add(&b_show_nPC);
		s_b_main.AddSpacer(5);
		s_b_main.Add(&b_next_opcode);

		m_regs = new wxTextCtrl(&m_main_panel, wxID_ANY, wxEmptyString,
			wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER|wxTE_RICH2);
		m_regs->SetMinSize(wxSize(495, 100));

		m_regs->SetEditable(false);

		s_p_main.AddSpacer(5);
		s_p_main.Add(&s_b_main);
		s_p_main.Add(m_regs);
		s_p_main.AddSpacer(8);
		s_p_main.Add(m_list);
		s_p_main.AddSpacer(5);

		wxBoxSizer& s_p_main_h = *new wxBoxSizer(wxVERTICAL);
		s_p_main_h.AddSpacer(5);
		s_p_main_h.Add(&s_p_main);
		s_p_main_h.AddSpacer(5);
		m_main_panel.SetSizerAndFit(&s_p_main_h);

		wxBoxSizer& s_main = *new wxBoxSizer(wxVERTICAL);
		s_main.Add(&m_main_panel);
		SetSizerAndFit(&s_main);

		m_list->InsertColumn(0, "ASM");

		for(uint i=0; i<show_lines; ++i)
		{
			m_list->InsertItem(m_list->GetItemCount(), wxEmptyString);
		}

		Connect(m_regs->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(InterpreterDisAsmFrame::OnUpdate));
		Connect( b_show_PC.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InterpreterDisAsmFrame::Show_cPC ));
		Connect( b_show_nPC.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InterpreterDisAsmFrame::Show_nPC ));
		Connect( b_next_opcode.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InterpreterDisAsmFrame::DoOpcode ));
		Connect( wxEVT_SIZE, wxSizeEventHandler(InterpreterDisAsmFrame::OnResize) );
		wxGetApp().Connect(m_list->GetId(), wxEVT_MOUSEWHEEL, wxMouseEventHandler(InterpreterDisAsmFrame::MouseWheel), (wxObject*)0, this);
		wxGetApp().Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(InterpreterDisAsmFrame::OnKeyDown), (wxObject*)0, this);
		SetSize(500, 700);

		Show_cPC(wxCommandEvent());
		WriteRegs();
		//m_regs->Disable();
	}

	virtual void OnKeyDown(wxKeyEvent& event)
	{
		if(wxGetActiveWindow() != this)
		{
			event.Skip();
			return;
		}

		if(event.GetKeyCode() == WXK_SPACE && event.ControlDown())
		{
			DoOpcode(wxCommandEvent());
			return;
		}

		//event.Skip();
	}

	void ShowPc(const int pc)
	{
		PC = pc;
		for(uint i=0; i<show_lines; ++i, PC += 4)
		{
			disasm->dump_pc = PC;
			decoder->DoCode(Memory.Read32(PC));
			m_list->SetItem(i, 0, disasm->last_opcode);

			if(PC == CPU.PC)
			{
				//m_list->SetItemTextColour( i, wxColour("Black") );
				m_list->SetItemBackgroundColour( i, wxColour("Green") );
			}
			else if(PC == CPU.nPC)
			{
				m_list->SetItemBackgroundColour( i, wxColour("Wheat") );
			}
			else
			{
				m_list->SetItemBackgroundColour( i, wxColour("White") );
			}
		}
	}

	void WriteRegs()
	{
		wxTextCtrl& regs = *m_regs;
		regs.Clear();

		for(uint i=0; i<32; ++i) regs.WriteText(wxString::Format("GPR[%d] = 0x%x\n", i, CPU.GPR[i]));
		for(uint i=0; i<32; ++i) regs.WriteText(wxString::Format("FPR[%d] = %f\n", i, CPU.FPR[i]));
		regs.WriteText(wxString::Format("LR = 0x%x\n", CPU.LR));
		regs.WriteText(wxString::Format("CTR = 0x%x\n", CPU.CTR));
		regs.WriteText(wxString::Format("CR = 0x%x\n", CPU.CR));
		regs.WriteText(wxString::Format("XER = 0x%x\n", CPU.XER));
	}

	virtual void OnUpdate(wxCommandEvent& event)
	{
		//WriteRegs();
	}

	virtual void Show_cPC(wxCommandEvent& WXUNUSED(event))
	{
		ShowPc(CPU.PC - ((show_lines*4)/2));
	}

	virtual void Show_nPC(wxCommandEvent& WXUNUSED(event))
	{
		ShowPc(CPU.nPC - ((show_lines/2)*4));
	}

	virtual void DoOpcode(wxCommandEvent& WXUNUSED(event))
	{
		const int pc = CPU.PC;
		const int npc = CPU.nPC;
		CPU.Resume();
		while(CPU.PC == pc && CPU.nPC == npc) Sleep(1);
		Show_cPC(wxCommandEvent());
		WriteRegs();
	}
	
	void OnResize(wxSizeEvent& event)
	{
		const wxSize& size( GetClientSize() );

		m_list->SetMinSize(wxSize(size.GetWidth(),  size.GetHeight()-55));
		m_main_panel.SetSize( size );
		m_main_panel.GetSizer()->RecalcSizes();

		m_list->SetColumnWidth(0, size.GetWidth()-8);
	}

	void MouseWheel(wxMouseEvent& event)
	{
		const int value = (event.m_wheelRotation / event.m_wheelDelta);
		ShowPc(PC - ((show_lines+value) * 4));
		event.Skip();
	}
};