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
	memset(XER, 0, sizeof(XER));
	memset(SPRG,0, sizeof(SPRG));
	memset(BO,  0, sizeof(BO));

	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TBU		= 0;
	TBL		= 0;

	GPR[0] = 66748;
	GPR[1] = -805238064;
	GPR[2] = 165072;
	GPR[3] = 1;
	GPR[4] = -805236816;
	GPR[5] = -805236800;
	GPR[6] = 268504880;
	GPR[7] = 79800000;
	GPR[8] = 268503584;
	GPR[9] = 268701108;
	GPR[10] = 16974000;
	GPR[12] = 268697880;
	GPR[13] = 536899680;
	GPR[26] = -805236800;
	GPR[28] = 1;
	GPR[29] = -805236800;
	GPR[30] = -805236816;
	GPR[31] = -805238064;
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