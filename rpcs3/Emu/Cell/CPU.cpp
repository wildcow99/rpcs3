#include "stdafx.h"
#include "CPU.h"

CPUCycle CPU;

void CPUCycle::Reset()
{
	pc = 0;
	npc = pc + 4;
}

void CPUCycle::NextPc()
{
	pc = npc;
	npc = pc + 4;
}

void CPUCycle::SetPc(const uint _pc)
{
	pc = _pc;
	npc = pc + 4;
}