#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/ElfLoader.h"
#include "Ini.h"

SysThread::SysThread() : StepThread()
{
	m_cur_state = STOP;
	m_mode = 0;
}

void SysThread::SetSelf(wxString self_patch)
{
	elf_loader.SetElf(self_patch);
	IsSlef = true;
}

void SysThread::SetElf(wxString elf_patch)
{
	elf_loader.SetElf(elf_patch);
	IsSlef = false;
}

void SysThread::Run()
{
	if(IsRunned()) return;
	if(IsPaused()) Resume();

	Memory.Init();

	if(IsSlef)
	{
		elf_loader.LoadSelf();
	}
	else
	{
		elf_loader.LoadElf();
	}
	
	m_mode = Ini.Emu.m_DecoderMode.GetValue();

	switch(m_mode)
	{
	case DisAsm:
		decoder = new Decoder(*new DisAsmOpcodes());
	break;
	case Interpreter:
		decoder = new Decoder(*new InterpreterOpcodes());
	break;
	};

	if(!m_memory_viewer) m_memory_viewer = new MemoryViewerPanel(NULL);

	m_memory_viewer->SetPC(CPU.PC);
	m_memory_viewer->Show();
	m_memory_viewer->ShowPC();

	m_cur_state = RUN;
	StepThread::Start();
	StepThread::DoStep();
}

void SysThread::Pause()
{
	if(!IsRunned()) return;

	m_cur_state = PAUSE;
}

void SysThread::Resume()
{
	if(!IsPaused()) return;

	m_cur_state = RUN;
	StepThread::DoStep();
}

void SysThread::Stop(bool CloseFrames)
{
	if(IsStoped()) return;

	m_cur_state = STOP;
	StepThread::Exit();

	ConLog.Write("Shutdown is started...");

	Memory.Close();
	CPU.Reset();
	CurGameInfo.Reset();

	if(CloseFrames)
	{
		if(m_memory_viewer && !m_memory_viewer->exit) m_memory_viewer->Hide();
		if(decoder) delete decoder;
	}
}

void SysThread::Step()
{
	while(m_cur_state == RUN)
	{
		StepThread::SetCancelState(false);
		{
			decoder->DoCode(Memory.Read32(CPU.PC));

			CPU.NextPc();
		}
		StepThread::SetCancelState(true);
	}

	ThreadAdv::Sleep(1);
}

void SysThread::CleanupInThread(void* arg)
{
	(*(SysThread*)arg).Stop();
}

SysThread System;