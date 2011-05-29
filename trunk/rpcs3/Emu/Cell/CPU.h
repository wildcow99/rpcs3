#pragma once

class CPUCycle
{
public:
	uint pc;
	uint npc;
	uint* gpr;

	CPUCycle()
	{
		Reset();
	}

	virtual void Reset();
	virtual void NextPc();
	virtual void SetPc(const uint _pc);
};

extern CPUCycle CPU;