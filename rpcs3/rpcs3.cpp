#include "stdafx.h"
#include "rpcs3.h"

IMPLEMENT_APP(TheApp)

bool TheApp::OnInit()
{
	SetAppName("rpcs3");

	m_log = new LogFrame();
	m_main = new MainFrame();

	m_log->Show();
	m_main->Show();

	return true;
}

GameInfo CurGameInfo;