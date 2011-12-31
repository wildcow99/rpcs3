#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Ini.h"

#include "Emu/Cell/PPCThreadManager.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/SPUThread.h"

#include "Loader/Loader.h"
#include "Emu/Display/Display.h"

#include "Emu/SysCalls/SysCalls.h"

SysCalls SysCallsManager;

Emulator::Emulator()
	: m_pad_manager(NULL)
{
	m_status = Stoped;
	m_mode = 0;
}

void Emulator::Init()
{
	if(m_pad_manager) free(m_pad_manager);
	m_pad_manager = new PadManager();
}

void Emulator::SetSelf(const wxString& path)
{
	m_path = path;
	IsSelf = true;
}

void Emulator::SetElf(const wxString& path)
{
	m_path = path;
	IsSelf = false;
}

void Emulator::CheckStatus()
{
	if(!GetCPU().GetIDs().HasID())
	{
		Stop();
		return;	
	}

	bool IsAllPaused = true;
	u32 pos = 0;
	ID thread;
	while(GetCPU().GetIDs().GetNext(pos, thread))
	{
		if(!(*(PPCThread*)thread.m_data).IsPaused())
		{
			IsAllPaused = false;
			break;
		}
	}
	if(IsAllPaused)
	{
		ConLog.Warning("all paused!");
		Pause();
		return;
	}

	bool IsAllStoped = true;
	pos = 0;
	while(GetCPU().GetIDs().GetNext(pos, thread))
	{
		if(!(*(PPCThread*)thread.m_data).IsStoped())
		{
			IsAllStoped = false;
			break;
		}
	}
	if(IsAllStoped)
	{
		ConLog.Warning("all stoped!");
		Pause(); //Stop();
	}
}

void Emulator::Run()
{
	if(IsRunned()) Stop();
	if(IsPaused())
	{
		Resume();
		return;
	}

	//ConLog.Write("run...");
	m_status = Runned;

	Memory.Init();

	Loader& loader = Loader(m_path);
	if(!loader.Load())
	{
		Memory.Close();
		return;
	}
	
	if(loader.GetMachine() == MACHINE_Unknown)
	{
		ConLog.Error("Unknown machine type");
		Memory.Close();
		return;
	}

#ifdef USE_GS_FRAME
	new GSFrame_GL(wxGetApp().m_MainFrame);
#endif

	const u32 id = GetCPU().AddThread(loader.GetMachine() == MACHINE_PPC64);

	PPCThread& thread = (*(PPCThread*)GetCPU().GetIDs().GetIDData(id).m_data);
	thread.SetPc(loader.GetEntry());
	thread.Run();

	if(m_memory_viewer && m_memory_viewer->exit) safe_delete(m_memory_viewer);
	if(!m_memory_viewer) m_memory_viewer = new MemoryViewerPanel(NULL);

	m_memory_viewer->SetPC(loader.GetEntry());
	m_memory_viewer->Show();
	m_memory_viewer->ShowPC();

	wxGetApp().m_MainFrame->UpdateUI();

	m_dbg_console = new DbgConsole();
	//m_dbg_console->Show();

	if(Ini.m_DecoderMode.GetValue() != 1) GetCPU().Exec();
}

void Emulator::Pause()
{
	if(!IsRunned()) return;
	//ConLog.Write("pause...");

	m_status = Paused;
	wxGetApp().m_MainFrame->UpdateUI();
}

void Emulator::Resume()
{
	if(!IsPaused()) return;
	//ConLog.Write("resume...");

	m_status = Runned;
	wxGetApp().m_MainFrame->UpdateUI();

	CheckStatus();
	if(IsRunned()) GetCPU().Exec();
}

void Emulator::Stop()
{
	if(IsStoped()) return;
	//ConLog.Write("shutdown...");

	m_status = Stoped;
	wxGetApp().m_MainFrame->UpdateUI();

	SysCallsManager.Close();
	GetCPU().Close();

	Memory.Close();
	CurGameInfo.Reset();

	m_dbg_console->Close();

	if(m_memory_viewer && !m_memory_viewer->exit) m_memory_viewer->Hide();
}

Emulator Emu;