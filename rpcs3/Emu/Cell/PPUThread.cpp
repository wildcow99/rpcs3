#include "stdafx.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUInterpreter.h"
#include "Emu/Cell/PPUDisAsm.h"

PPUThread::PPUThread() : PPCThread(false)
{
	Reset();
}

PPUThread::~PPUThread()
{
	//~PPCThread();
}

void PPUThread::DoReset()
{
	//reset regs
	//memset(VPR,	 0, sizeof(VPR));
	memset(FPR,  0, sizeof(FPR));
	memset(GPR,  0, sizeof(GPR));
	memset(SPRG, 0, sizeof(SPRG));
	
	CR.CR	= 0;
	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TB		= 0;
	XER.XER	= 0;
	FPSCR.FPSCR	= 0;

	reserve32 = 0;
	has_reserve32 = false;
}

void PPUThread::_InitStack()
{
	GPR[1] = Stack.GetEndAddr();
	GPR[3] = m_arg;
}

u64 PPUThread::GetFreeStackSize() const
{
	return (GetStackAddr() + GetStackSize()) - GPR[1];
}

void PPUThread::DoRun()
{
	switch(Ini.m_DecoderMode.GetValue())
	{
	case 0: m_dec = new PPU_Decoder(*new PPU_DisAsm(*this)); break;
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

void PPUThread::DoCode(const s32 code)
{
	static u8 cycle = 0;
	if(++cycle > 220)
	{
		cycle = 0;
		TB++;
	}

	(*(PPU_Decoder*)m_dec).Decode(code);
}

bool FPRdouble::IsINF(double d)
{
	return wxFinite(d) ? 1 : 0;
}

bool FPRdouble::IsNaN(double d)
{
	return wxIsNaN(d) ? 1 : 0;
}

bool FPRdouble::IsQNaN(double d)
{
	FPRdouble x(d);

	return
		((x.i & DOUBLE_EXP) == DOUBLE_EXP) &&
		((x.i & 0x0007fffffffffffULL) == DOUBLE_ZERO) &&
		((x.i & 0x000800000000000ULL) == 0x000800000000000ULL);
}

bool FPRdouble::IsSNaN(double d)
{
	FPRdouble x(d);
	
	return
		((x.i & DOUBLE_EXP) == DOUBLE_EXP) &&
		((x.i & DOUBLE_FRAC) != DOUBLE_ZERO) &&
		((x.i & 0x0008000000000000ULL) == DOUBLE_ZERO);
}

int FPRdouble::Cmp(FPRdouble& f)
{
	if(d < f.d) return CR_LT;
	if(d > f.d) return CR_GT;
	if(d == f.d) return CR_EQ;
	return CR_SO;
}