#include "stdafx.h"
#include "CPU.h"

CPUCycle CPU;

void CPUCycle::Reset()
{
	cycle = PC = 0;
	nPC = PC + 4;

	isBranch = false;

	//reset regs
	memset(FPR, 0, sizeof(FPR));
	memset(GPR, 0, sizeof(GPR));
	memset(CR,  0, sizeof(CR));
	memset(SPRG,0, sizeof(SPRG));
	memset(BO,  0, sizeof(BO));

	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TBU		= 0;
	TBL		= 0;
	XER		= 0;

	GPR[1] = 0x3ffd0; //Stack address
}

void CPUCycle::NextBranchPc()
{
	PC = nPC;
	nPC = PC + 4;
}

void CPUCycle::NextPc()
{
	if(isBranch)
	{
		NextBranchPc();
		isBranch = false;
		return;
	}

	PC += 4;
	nPC = PC + 4;
}

void CPUCycle::PrevPc()
{
	PC -= 4;
	nPC -= 4;
}

void CPUCycle::SetPc(const u32 pc)
{
	PC = pc;
	nPC = PC + 4;
}

void CPUCycle::SetBranch(const u32 pc)
{
	nPC = pc;
	isBranch = true;
}