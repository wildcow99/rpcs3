#include "stdafx.h"
#include "DbgConsole.h"

BEGIN_EVENT_TABLE(DbgConsole, FrameBase)
	EVT_CLOSE(DbgConsole::OnQuit)
END_EVENT_TABLE()

DbgConsole::DbgConsole()
	: FrameBase(NULL, wxID_ANY, "DbgConsole", wxEmptyString, wxDefaultSize, wxDefaultPosition)
	, StepThread("DbgConsole thread")
{
	m_console = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(500, 500), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	m_console->SetBackgroundColour(wxColor("Black"));
	m_console->SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	m_color_white = new wxTextAttr(wxColour(255, 255, 255));
	m_color_red = new wxTextAttr(wxColour(255, 0, 0));
	StepThread::Start();
}

DbgConsole::~DbgConsole()
{
	StepThread::Exit();
	m_packets.Clear();
}

void DbgConsole::Write(int ch, const wxString& text)
{
	if(!StepThread::IsAlive() || IsBeingDeleted()) return;

	while(m_packets.GetCount() > 50)
	{
		if(!StepThread::IsAlive() || IsBeingDeleted()) return;
		Sleep(1);
	}
	m_packets.Add(new DbgPacket(ch, text));
	DoStep();
}

void DbgConsole::Clear()
{
	m_console->Clear();
}

void DbgConsole::Step()
{
	if(m_packets.GetCount() == 0) return;

	const u32 max_lines_count = 500;
	if(!DbgConsole::IsShown()) Show();

	m_console->SetFocus();
	m_console->Freeze();
	while(m_packets.GetCount())
	{
		m_console->SetDefaultStyle(m_packets[0].m_ch == 1 ? *m_color_red : *m_color_white);
		m_console->SetInsertionPointEnd();
		m_console->WriteText(m_packets[0].m_text);
		m_packets[0].Clear();
		m_packets.RemoveAt(0);
		Sleep(1);
	}
	m_console->Thaw();

	Sleep(1);
}

void DbgConsole::OnQuit(wxCloseEvent& event)
{
	Hide();
}