#include "stdafx.h"
#include "Emu/Cell/PPU.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUInterpreter.h"
#include "Emu/Cell/PPUDisAsm.h"
#include "Gui/InterpreterDisAsm.h"

PPUThread::PPUThread(const u16 id) : CPUThread(false, id)
{
	Reset();

	if(Ini.Emu.m_DecoderMode.GetValue() == 1)
	{
		InterpreterDisAsmFrame* disasm = new InterpreterDisAsmFrame(StepThread::GetName(), this);
		disasm->Show();
	}
}

PPUThread::~PPUThread()
{
	//~CPUThread();
}

void PPUThread::DoReset()
{
	//reset regs
	memset(FPR,  0, sizeof(FPR));
	memset(GPR,  0, sizeof(GPR));
	memset(SPRG, 0, sizeof(SPRG));
	memset(BO,   0, sizeof(BO));
	memset(CR,   0, sizeof(CR));

	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TBU		= 0;
	TBL		= 0;
	XER		= 0;

	GPR[1] = Memory.MainRam.GetEndAddr(); //FIXME: Stack address
}

void PPUThread::DoRun()
{
	switch(Ini.Emu.m_DecoderMode.GetValue())
	{
	case 0: m_dec = new PPU_Decoder(*new PPU_DisAsm(this)); break;
	case 1:
	case 2:
		m_dec = new PPU_Decoder(*new PPU_Interpreter(*this)); break;
	}
}

void PPUThread::DoResume()
{
}

void PPUThread::DoPause()
{
}

void PPUThread::DoStop()
{
	if(m_dec)
	{
		(*(PPU_Decoder*)m_dec).~PPU_Decoder();
		safe_delete(m_dec);
	}
}

void PPUThread::DoSysResume()
{
}

void PPUThread::DoCode(const s32 code)
{
	(*(PPU_Decoder*)m_dec).DoCode(code);
}