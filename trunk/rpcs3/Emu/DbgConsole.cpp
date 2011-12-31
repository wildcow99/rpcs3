#include "stdafx.h"
#include "DbgConsole.h"

DbgConsole::DbgConsole() : FrameBase(NULL, wxID_ANY, "DbgConsole")
{
	m_console = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(500, 500), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	m_console->SetBackgroundColour(wxColor("Black"));

	m_color_white = new wxTextAttr(wxColour(255, 255, 255));
	m_color_red = new wxTextAttr(wxColour(255, 0, 0));
}

void DbgConsole::Write(int ch, const wxString& text)
{
	if(!IsShown()) Show();
	m_console->SetDefaultStyle(ch == 1 ? *m_color_red : *m_color_white);
	m_console->WriteText(text);
}

void DbgConsole::Clear()
{
	m_console->Clear();
}