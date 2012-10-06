#include "stdafx.h"
#include "DbgConsole.h"

BEGIN_EVENT_TABLE(DbgConsole, FrameBase)
	EVT_CLOSE(DbgConsole::OnQuit)
END_EVENT_TABLE()

DbgConsole::DbgConsole()
	: FrameBase(NULL, wxID_ANY, "DbgConsole", wxEmptyString, wxDefaultSize, wxDefaultPosition)
	, ThreadBase(false, "DbgConsole thread")
{
	m_console = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(500, 500), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	m_console->SetBackgroundColour(wxColor("Black"));
	m_console->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	m_color_white = new wxTextAttr(wxColour(255, 255, 255));
	m_color_red = new wxTextAttr(wxColour(255, 0, 0));

	ThreadBase::Start();
}

DbgConsole::~DbgConsole()
{
	m_dbg_buffer.Flush();
}

void DbgConsole::Write(int ch, const wxString& text)
{
	while(m_dbg_buffer.IsBusy()) Sleep(0);
	m_dbg_buffer.Push(DbgPacket(ch, text));
}

void DbgConsole::Clear()
{
	m_console->Clear();
}

void DbgConsole::Task()
{
	while(!TestDestroy())
	{
		Sleep(1);

		if(!m_dbg_buffer.HasNewPacket()) continue;

		m_console->Freeze();
		while(m_dbg_buffer.HasNewPacket())
		{
			DbgPacket packet = m_dbg_buffer.Pop();
			m_console->SetDefaultStyle(packet.m_ch == 1 ? *m_color_red : *m_color_white);
			m_console->SetInsertionPointEnd();
			m_console->WriteText(packet.m_text);

		}
		m_console->Thaw();

		if(!DbgConsole::IsShown()) Show();
		SetFocus();
	}
}

void DbgConsole::OnQuit(wxCloseEvent& event)
{
	Hide();
}