#include "stdafx.h"
#include "DbgConsole.h"

DbgConsole::DbgConsole()
	: FrameBase(NULL, wxID_ANY, "DbgConsole", wxEmptyString, wxDefaultSize,
		wxDefaultPosition, wxDEFAULT_FRAME_STYLE & ~wxCLOSE_BOX)
	, StepThread("DbgConsole thread")
	, hided(false)
{
	m_console = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(500, 500), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	m_console->SetBackgroundColour(wxColor("Black"));
	m_console->SetFont(wxFont("Lucida Console"));

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
	if(StepThread::IsExit() || hided) return;

	while(m_packets.GetCount() > 200)
	{
		if(StepThread::IsExit() || hided) return;
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
	while(m_packets.GetCount())
	{
		StepThread::SetCancelState(false);
		m_console->SetDefaultStyle(m_packets[0].m_ch == 1 ? *m_color_red : *m_color_white);
		m_console->WriteText(m_packets[0].m_text);
		m_packets.RemoveAt(0);
		StepThread::SetCancelState(true);
	}

	Sleep(1);
}

bool DbgConsole::Close(bool force)
{
	StepThread::Exit();
	m_packets.Clear();

	return FrameBase::Close(force);
}