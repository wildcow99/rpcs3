#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/ElfLoader.h"
#include "Ini.h"

SysThread::SysThread()
{
	m_cur_state = STOP;
	m_mode = 0;

	m_done = true;
	m_exit = false;
	
	m_hThread = ::CreateThread
	(
		NULL,
		0,
		SysThread::ThreadStart,
		(LPVOID)this,
		0,
		NULL
	);
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
	if(IsPaused()) Stop();

	Memory.Init();
	CurGameInfo.Reset();

	if(IsSlef)
	{
		elf_loader.LoadSelf();
	}
	else
	{
		elf_loader.LoadElf();
	}
	
	m_mode = ini.Load("DecoderMode", 1);

	switch(m_mode)
	{
	case DisAsm:
		decoder = new Decoder(*new DisAsmOpcodes());
	break;
	case Interpreter:
		decoder = new Decoder(*new InterpreterOpcodes());
	break;
	};

	if(m_memory_viewer == NULL) m_memory_viewer = new MemoryViewerPanel(NULL);

	m_memory_viewer->SetPC(CPU.PC);
	m_memory_viewer->Show();
	m_memory_viewer->ShowPC();

	m_cur_state = RUN;
	m_sem_wait.Post();
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
	m_sem_wait.Post();
}

void SysThread::Stop()
{
	if(IsStoped()) return;

	m_cur_state = STOP;

	if(m_memory_viewer != NULL) m_memory_viewer->Show(false);
	Memory.Close();
	CPU.Reset();

	delete decoder;
}

void SysThread::Task()
{
	while(m_cur_state == RUN)
	{
		decoder->DoCode(Memory.Read32(CPU.PC));

		CPU.NextPc();
	}
}

SysThread System;