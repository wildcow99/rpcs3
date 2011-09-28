#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Emu/ElfLoader.h"
#include "Ini.h"

#include "Emu/Cell/PPU.h"
#include "Emu/Cell/SPU.h"

Emulator::Emulator()
{
	m_status = Stoped;
	m_mode = 0;
}

void Emulator::SetSelf(wxString self_patch)
{
	Loader.SetElf(self_patch);
	IsSlef = true;
}

void Emulator::SetElf(wxString elf_patch)
{
	Loader.SetElf(elf_patch);
	IsSlef = false;
}

CPUThread& Emulator::AddThread(bool isSPU)
{
	const int id = GetCPU().GetCount() + 1;
	if(isSPU)
	{
		GetCPU().Add(*new SPUThread(id));
	}
	else
	{
		GetCPU().Add(*new PPUThread(id));
	}

	return GetCPU().Get(GetCPU().GetCount() - 1);
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

	if(IsSlef)
	{
		Loader.LoadSelf();
	}
	else
	{
		Loader.LoadElf();
	}
	
	CPUThread& cpu_thread = AddThread(Loader.isSPU);
	cpu_thread.SetPc(Loader.entry);

	if(!m_memory_viewer) m_memory_viewer = new MemoryViewerPanel(NULL);

	m_memory_viewer->SetPC(cpu_thread.PC);
	m_memory_viewer->Show();
	m_memory_viewer->ShowPC();

	cpu_thread.Run();

	wxGetApp().m_MainFrame->UpdateUI();
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

	for(uint i=0; i<GetCPU().GetCount(); ++i)
	{
		GetCPU().Get(i).SysResume();
	}
}

void Emulator::Stop()
{
	if(IsStoped()) return;
	//ConLog.Write("shutdown...");

	m_status = Stoped;
	wxGetApp().m_MainFrame->UpdateUI();

	for(uint i=0; i<GetCPU().GetCount(); ++i)
	{
		GetCPU().Get(i).Close();
	}

	GetCPU().Clear();

	Memory.Close();
	CurGameInfo.Reset();

	if(m_memory_viewer && !m_memory_viewer->exit) m_memory_viewer->Hide();
}

Emulator Emu;