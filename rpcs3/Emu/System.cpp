#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Ini.h"

#include "Emu/Cell/PPCThreadManager.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/SPUThread.h"

#include "Emu/SysCalls/SysCalls.h"

SysCalls SysCallsManager;

Emulator::Emulator()
	: m_status(Stoped)
	, m_mode(DisAsm)
	, m_dbg_console(NULL)
{
}

void Emulator::Init()
{
	//if(m_memory_viewer) m_memory_viewer->Close();
	//m_memory_viewer = new MemoryViewerPanel(wxGetApp().m_MainFrame);
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
	ArrayF<PPCThread>& threads = GetCPU().GetThreads();
	if(!threads.GetCount())
	{
		Stop();
		return;	
	}

	bool IsAllPaused = true;
	for(u32 i=0; i<threads.GetCount(); ++i)
	{
		if(threads[i].IsPaused()) continue;
		IsAllPaused = false;
		break;
	}
	if(IsAllPaused)
	{
		ConLog.Warning("all paused!");
		Pause();
		return;
	}

	bool IsAllStoped = true;
	for(u32 i=0; i<threads.GetCount(); ++i)
	{
		if(threads[i].IsStoped()) continue;
		IsAllStoped = false;
		break;
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

	GetInfo().Reset();

	//SetTLSData(0, 0, 0);
	//SetMallocPageSize(0x100000);

	m_loader.Open(m_path);
	if(!m_loader.Load())
	{
		Memory.Close();
		Stop();
		return;
	}
	
	if(m_loader.GetMachine() == MACHINE_Unknown)
	{
		ConLog.Error("Unknown machine type");
		Memory.Close();
		Stop();
		return;
	}

	PPCThread& thread = GetCPU().AddThread(m_loader.GetMachine() == MACHINE_PPC64);

	thread.SetPc(m_loader.GetEntry());
	thread.SetArg(thread.GetId());
	thread.Run();

	//if(m_memory_viewer && m_memory_viewer->exit) safe_delete(m_memory_viewer);

	//m_memory_viewer->SetPC(loader.GetEntry());
	//m_memory_viewer->Show();
	//m_memory_viewer->ShowPC();

	wxGetApp().m_MainFrame->UpdateUI();

	if(!m_dbg_console) m_dbg_console = new DbgConsole();

	GetGSManager().Init();

	if(Ini.CPUDecoderMode.GetValue() != 1)
	{
		GetCPU().Start();
		GetCPU().Exec();
	}
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

	GetGSManager().Close();

	GetCPU().Close();
	SysCallsManager.Close();

	Memory.Close();
	CurGameInfo.Reset();
	GetIdManager().Clear();
	GetPadManager().Close();

	if(m_dbg_console){GetDbgCon().Close();m_dbg_console = NULL;}
	//if(m_memory_viewer && m_memory_viewer->IsShown()) m_memory_viewer->Hide();
}

Emulator Emu;