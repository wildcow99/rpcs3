#include "stdafx.h"
#include "Emu/Cell/SPU.h"
#include "Emu/Cell/SPUDecoder.h"
#include "Emu/Cell/SPUInterpreter.h"
#include "Emu/Cell/SPUDisAsm.h"

SPUThread::SPUThread(const u16 id) : CPUThread(true, id)
{
	Reset();
}

SPUThread::~SPUThread()
{
	//~CPUThread();
}

void SPUThread::DoReset()
{
	//reset regs
	memset(GPR,  0, sizeof(GPR));
	
	GPR[1] = 0x3ffd0; //Stack address
}

void SPUThread::DoRun()
{
	switch(Ini.Emu.m_DecoderMode.GetValue())
	{
	case 0: m_dec = new SPU_Decoder(*new SPU_DisAsm(this)); break;
	case 1:
	case 2:
		m_dec = new SPU_Decoder(*new SPU_Interpreter(*this)); break;
	}
}

void SPUThread::DoResume()
{
}

void SPUThread::DoPause()
{
}

void SPUThread::DoStop()
{
	if(m_dec)
	{
		(*(SPU_Decoder*)m_dec).~SPU_Decoder();
		safe_delete(m_dec);
	}
}

void SPUThread::DoSysResume()
{
}

void SPUThread::DoCode(const s32 code)
{
	(*(SPU_Decoder*)m_dec).DoCode(code);
}