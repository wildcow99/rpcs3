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

void Rpcs3App::CleanUp()
{
	System.Stop();
	wxApp::CleanUp();
}

void Rpcs3App::Exit()
{
	if(ConLogFrame && ConLogFrame->runned) ConLogFrame->~LogFrame();

	if(m_main && m_main->IsShown())
	{
		Ini.Gui.m_MainWindow.SetValue(WindowInfo(m_main->GetSize(), m_main->GetPosition()));
		m_main->Hide();
	}
	CleanUp();
	Ini.Save();

	wxApp::Exit();
}

GameInfo CurGameInfo;