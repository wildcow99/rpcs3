#include "stdafx.h"
#include "DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/System.h"


DisAsmFrame::DisAsmFrame() : wxFrame(NULL, wxID_ANY, "DisAsm")
{
	m_PC = 0;

	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );
	wxBoxSizer& s_b_panel( *new wxBoxSizer(wxHORIZONTAL) );

	m_disasm_list = new wxListView(this);

	wxButton& b_fprev	= *new wxButton(this, wxID_ANY, L"<<");
	wxButton& b_prev	= *new wxButton(this, wxID_ANY, L"<");
	wxButton& b_next	= *new wxButton(this, wxID_ANY, L">");
	wxButton& b_fnext	= *new wxButton(this, wxID_ANY, L">>");

	s_b_panel.Add(&b_fprev);
	s_b_panel.Add(&b_prev);
	s_b_panel.AddSpacer(5);
	s_b_panel.Add(&b_next);
	s_b_panel.Add(&b_fnext);

	s_panel.Add(&s_b_panel);
	s_panel.Add(m_disasm_list);

	m_disasm_list->InsertColumn(0, "ASM");

	SetSizerAndFit( &s_panel );

	SetSize(50, 650);

	Connect( wxEVT_SIZE, wxSizeEventHandler(DisAsmFrame::OnResize) );

	Connect(b_prev.GetId(),	 wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Prev));
	Connect(b_next.GetId(),  wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Next));
	Connect(b_fprev.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fPrev));
	Connect(b_fnext.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fNext));
}

void DisAsmFrame::OnResize(wxSizeEvent& event)
{
	const wxSize size(GetClientSize());
	m_disasm_list->SetSize( size.GetWidth(), size.GetHeight() - 25);
	m_disasm_list->SetColumnWidth(0, size.GetWidth() - 4);
}

void DisAsmFrame::AddLine(wxString line)
{
	static int cur_item_add = 0;
	if(++cur_item_add >= LINES_OPCODES)
	{
		System.Pause();
		cur_item_add = 0;
	}

	int item_count = m_disasm_list->GetItemCount();
	if(item_count >= LINES_OPCODES)
	{
		for(uint i=0; i<item_count-LINES_OPCODES; ++i) m_disasm_list->DeleteItem(0);
		item_count = m_disasm_list->GetItemCount();
	}

	m_disasm_list->InsertItem(item_count, line);
}

void DisAsmFrame::Prev (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.pc - 4*(LINES_OPCODES+1)); System.Run(); } }
void DisAsmFrame::Next (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.pc - 4*(LINES_OPCODES-1)); System.Run(); } }
void DisAsmFrame::fPrev(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.pc - (4*LINES_OPCODES)*2); System.Run(); } }
void DisAsmFrame::fNext(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { System.Run(); } }