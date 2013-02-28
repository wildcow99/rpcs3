#pragma once
#include <wx/listctrl.h>
#include "wx/aui/aui.h"

class DebuggerPanel : public wxPanel
{
	wxAuiManager m_aui_mgr;
	wxAuiNotebook* m_nb;

public:
	DebuggerPanel(wxWindow* parent);
	~DebuggerPanel();

	void UpdateUI();
	void HandleCommand(wxCommandEvent& event);
};