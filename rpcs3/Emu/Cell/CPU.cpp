#include "stdafx.h"
#include "CPU.h"
#include "Emu/Decoder/Decoder.h"
#include "Emu/Opcodes/Interpreter.h"
#include "Emu/Opcodes/DisAsm.h"

CPUThread::CPUThread(const u8 id)
	: m_id(id)
	, StepThread(wxString::Format("CPU[%d] Thread", id))
{
	m_dec = NULL;
	Reset();
	StepThread::Start();
}

CPUThread::~CPUThread()
{
	Reset();
	StepThread::Exit();
}

void CPUThread::Reset()
{
	SetPc(0);
	cycle = 0;

	isBranch = false;

	m_status = Stoped;
	m_error = 0;

	//reset regs
	memset(FPR,  0, sizeof(FPR));
	memset(GPR,  0, sizeof(GPR));
	memset(SPRG, 0, sizeof(SPRG));
	memset(BO,   0, sizeof(BO));

	CR		= 0;
	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TBU		= 0;
	TBL		= 0;
	XER		= 0;

	GPR[1] = 0x3ffd0; //Stack address
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
	case 0x39800000: ConLog.Warning("Initialize TLS #pc: %x", PC); break;

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

	switch(Ini.Emu.m_DecoderMode.GetValue())
	{
	case 0: m_dec = new Decoder(*new DisAsmOpcodes()); break;
	case 1: m_dec = new Decoder(*new InterpreterOpcodes(*this)); break;
	}

	StepThread::DoStep();
}

void CPUThread::Resume()
{
	if(!IsPaused()) return;
	StepThread::DoStep();
}

void CPUThread::Pause()
{
	if(!IsRunned()) return;
	m_status = Paused;
}

void CPUThread::Stop()
{
	if(IsStoped()) return;
	m_status = Stoped;
	Reset();

	if(m_dec)
	{
		(*(Decoder*)m_dec).~Decoder();
		safe_delete(m_dec);
	}
}

void CPUThread::SysResume()
{
	if(!IsRunned()) return;
	StepThread::DoStep();
}

void CPUThread::Step()
{
	m_status = Runned;
	Decoder& dec = *(Decoder*)m_dec;

	while(IsRunned() && Emu.IsRunned())
	{
		dec.DoCode(Memory.Read32(PC));
		NextPc();
	}

	if(!IsOk())
	{
		ConLog.Error("CpuThread[%d] stoped with error! Error code: %x", m_id, GetError());
		SetError(0);
	}
}