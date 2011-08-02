#include "stdafx.h"
#include "rpcs3.h"
#include "Ini.h"
#include "Emu/System.h"

IMPLEMENT_APP(Rpcs3App)
Rpcs3App* TheApp;

bool Rpcs3App::OnInit()
{
	TheApp = this;
	SetAppName("rpcs3");

	Ini.Load();

	ConLogFrame = new LogFrame();
	ConLogFrame->Show();

	m_main = new MainFrame();
	m_main->Show();

	return true;
}

void Rpcs3App::Exit()
{
	Ini.Save();
	System.Stop();
	System.~SysThread();

	if(ConLogFrame && ConLogFrame->runned) ConLogFrame->~LogFrame();

	if(m_main && m_main->IsShown())
	{
		Ini.Gui.m_MainWindow.SetValue(WindowInfo(m_main->GetSize(), m_main->GetPosition()));
		m_main->~MainFrame();
	}

	wxApp::Exit();
}

GameInfo CurGameInfo;