#include "stdafx.h"
#include "System.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/ElfLoader.h"


SysThread::SysThread() : Thread()
{
	m_cur_state = STOP;
	m_mode = DisAsm;
	
	Thread::Start();
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

	if(IsPaused())
	{
		m_cur_state = RUN;
		m_mtx_pause.Post();
		Task();
		return;
	}
	switch(m_mode)
	{
	case DisAsm:
		disasm = new DisAsmOpcodes();
		decoder = new Decoder(*disasm);
	break;
	};

	Memory.Init();

	if(IsSlef)
	{
		elf_loader.LoadSelf();
	}
	else
	{
		elf_loader.LoadElf();
	}

	m_cur_state = RUN;
	m_mtx_wait.Post();
	Task();
}

void SysThread::Pause()
{
	if(!IsRunned()) return;

	m_cur_state = PAUSE;
}

void SysThread::Stop()
{
	if(IsStoped()) return;

	if(IsPaused()) Run();

	m_cur_state = STOP;
	Memory.Close();
	CPU.Reset();
}

void SysThread::Task()
{

	/*
	ConLog.Write("Task");
	for(;;)
	{
		m_mtx_wait.Wait();
	*/

		for(;;)
		{
			switch(m_cur_state)
			{
			case STOP:
			case PAUSE: //m_mtx_pause.Wait(); break;
				return;
			default: break;
			};

			decoder->DoCode(Memory.Read32(CPU.pc));

			CPU.NextPc();
		}
	//}
}

SysThread System;