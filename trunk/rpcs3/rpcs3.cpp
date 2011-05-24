#include "stdafx.h"

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"

#include "rpcs3.h"

IMPLEMENT_APP(TheApp)

bool TheApp::OnInit()
{
	m_log = new LogFrame();
	m_main = new MainFrame();

	m_log->Show();
	m_main->Show();

	return true;
}