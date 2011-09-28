#include "stdafx.h"
#include "rpcs3.h"
#include "Ini.h"
#include "Emu/System.h"
#include <wx/msw/wrapwin.h>

IMPLEMENT_APP(Rpcs3App)
Rpcs3App* TheApp;

bool Rpcs3App::OnInit()
{
	TheApp = this;
	SetAppName("rpcs3");

	Ini.Load();

	ConLogFrame = new LogFrame();
	ConLogFrame->Show();

	m_MainFrame = new MainFrame();
	m_MainFrame->Show();

	return true;
}

void Rpcs3App::Exit()
{
	Ini.Save();
	Emu.Stop();

	if(ConLogFrame && ConLogFrame->runned) ConLogFrame->~LogFrame();

	if(m_MainFrame && m_MainFrame->IsShown())
	{
		Ini.Gui.m_MainWindow.SetValue(WindowInfo(m_MainFrame->GetSize(), m_MainFrame->GetPosition()));
		m_MainFrame->~MainFrame();
	}

	wxApp::Exit();
}

GameInfo CurGameInfo;

CPUThread& GetCPU(const u8 core)
{
	return Emu.GetCPU().Get(core);
}