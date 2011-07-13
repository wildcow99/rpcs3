#include "stdafx.h"
#include "CPU.h"

CPUCycle CPU;

void CPUCycle::Reset()
{
	cycle = PC = 0;
	nPC = PC + 4;
}

void CPUCycle::NextBranchPc()
{
	PC = nPC;
	nPC = PC + 4;
}

void CPUCycle::NextPc()
{
	PC += 4;
	nPC = PC + 4;
}

void CPUCycle::PrevPc()
{
	PC -= 4;
	nPC -= 4;
}

void CPUCycle::SetPc(const uint _pc)
{
	PC = _pc;
	nPC = PC + 4;
}