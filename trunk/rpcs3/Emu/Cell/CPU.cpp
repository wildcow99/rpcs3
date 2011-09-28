#include "stdafx.h"
#include "CPU.h"

CPUThread::CPUThread(bool _isSPU, const u16 id)
	: StepThread(wxString::Format("%s[%d] Thread",(_isSPU ? "SPU" : "PPU"), id))
	, m_id(id)
	, isSPU(_isSPU)
{
	m_dec = NULL;
	StepThread::Start();
}

CPUThread::~CPUThread()
{
}

void CPUThread::Close()
{
	Pause();
	Stop();
	StepThread::Exit();
	this->~CPUThread();
}

void CPUThread::Reset()
{
	SetPc(0);
	cycle = 0;

	isBranch = false;

	m_status = Stoped;
	m_error = 0;
	
	DoReset();
}

void CPUThread::NextBranchPc()
{
	SetPc(nPC);
}

void CPUThread::NextPc()
{
	if(isBranch)
	{
		NextBranchPc();
		isBranch = false;
		return;
	}

	SetPc(PC + 4);
}

void CPUThread::PrevPc()
{
	SetPc(PC - 4);
}

void CPUThread::SetPc(const u32 pc)
{
	PC = pc;
	nPC = PC + 4;
}

void CPUThread::SetBranch(const u32 pc)
{
	switch(pc)
	{
	case 0x39800000: ConLog.Warning("Initialize TLS #pc: %x", PC); break; //CHECK ME!

	default:
		nPC = pc;
		isBranch = true;
	break;
	};
}

void CPUThread::SetError(const u32 error)
{
	if(error == 0)
	{
		m_error = 0;
	}
	else
	{
		m_error |= error;
	}
}

void CPUThread::Run()
{
	if(IsRunned()) Stop();
	if(IsPaused())
	{
		Resume();
		return;
	}
	
	DoRun();
	if(Ini.Emu.m_DecoderMode.GetValue() != 1) StepThread::DoStep();
	else
	{
		m_status = Runned;
		Pause();
	}
}

void CPUThread::Resume()
{
	if(!IsPaused()) return;
	DoResume();
	StepThread::DoStep();
}

void CPUThread::Pause()
{
	if(!IsRunned()) return;
	m_status = Paused;
	DoPause();
}

void CPUThread::Stop()
{
	if(IsStoped()) return;
	m_status = Stoped;
	Reset();
	DoStop();
}

void CPUThread::SysResume()
{
	if(!IsRunned()) return;
	DoSysResume();
	StepThread::DoStep();
}

void CPUThread::Step()
{
	m_status = Runned;

	if(Ini.Emu.m_DecoderMode.GetValue() == 1)
	{
		DoCode(Memory.Read32(PC));
		NextPc();
		m_status = Paused;
	}
	else
	{
		while(IsRunned() && Emu.IsRunned())
		{
			DoCode(Memory.Read32(PC));
			NextPc();
		}

		if(!IsOk())
		{
			ConLog.Error("CpuThread[%d] stoped with error! Error code: %x", m_id, GetError());
			SetError(0);
		}
	}
}