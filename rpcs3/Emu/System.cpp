#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Emu/ElfLoader.h"
#include "Ini.h"

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

	GetCPU().SetCount(8);

	if(IsSlef)
	{
		Loader.LoadSelf();
	}
	else
	{
		Loader.LoadElf();
	}
	
	/*
	m_mode = Ini.Emu.m_DecoderMode.GetValue();

	switch(m_mode)
	{
	case DisAsm:
		ppc_decoder = new PPCDecoder(*new DisAsmOpcodes());
		//decoder = new Decoder(*new DisAsmOpcodes());
	break;
	case Interpreter:
		decoder = new Decoder(*new InterpreterOpcodes());
	break;
	};
	*/

	if(!m_memory_viewer) m_memory_viewer = new MemoryViewerPanel(NULL);

	m_memory_viewer->SetPC(GetPPU().PC);
	m_memory_viewer->Show();
	m_memory_viewer->ShowPC();

	GetPPU().Run();

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
		CPUThread* cpu = &GetCPU().Get(i);
		cpu->Stop();
		cpu->~CPUThread();
	}

	GetCPU().Clear();

	Memory.Close();
	CurGameInfo.Reset();

	if(m_memory_viewer && !m_memory_viewer->exit) m_memory_viewer->Hide();
}

Emulator Emu;