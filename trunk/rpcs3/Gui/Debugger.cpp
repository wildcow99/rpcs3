#include "stdafx.h"
#include "Debugger.h"
#include "Emu/Memory/Memory.h"
#include "InterpreterDisAsm.h"

DebuggerPanel::DebuggerPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 600), wxTAB_TRAVERSAL)
{
	m_aui_mgr.SetManagedWindow(this);

	m_nb = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT |
		wxAUI_NB_TAB_EXTERNAL_MOVE | wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_MOVE | wxNO_BORDER);

	m_aui_mgr.AddPane(m_nb, wxAuiPaneInfo().Center().CaptionVisible(false).CloseButton().MaximizeButton());
	m_aui_mgr.Update();

	m_app_connector.Connect(wxEVT_DBG_COMMAND, wxCommandEventHandler(DebuggerPanel::HandleCommand), (wxObject*)0, this);
}

DebuggerPanel::~DebuggerPanel()
{
	m_aui_mgr.UnInit();
}

void DebuggerPanel::UpdateUI()
{
}

void DebuggerPanel::HandleCommand(wxCommandEvent& event)
{
	PPCThread* thr = (PPCThread*)event.GetClientData();

	switch(event.GetId())
	{
	case DID_CREATE_THREAD:
		m_nb->AddPage(new InterpreterDisAsmFrame(m_nb, thr), thr->GetFName());
	break;

	case DID_REMOVE_THREAD:
		for(uint i=0; i<m_nb->GetPageCount(); ++i)
		{
			InterpreterDisAsmFrame* page = (InterpreterDisAsmFrame*)m_nb->GetPage(i);

			if(page->CPU.GetId() == thr->GetId())
			{
				m_nb->DeletePage(i);
				break;
			}
		}
	break;
	}

	event.Skip();
}