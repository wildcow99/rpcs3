#pragma once

#include "Emu/Cell/SPUOpcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/SPUThread.h"
#include "Emu/SysCalls/SysCalls.h"

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(x)

class SPU_Interpreter : public SPU_Opcodes
{
private:
	SPUThread& CPU;

public:
	SPU_Interpreter(SPUThread& cpu) : CPU(cpu)
	{
	}

private:
	virtual void Exit(){}

	virtual void SysCall()
	{
	}

	virtual void NOP()
	{
		//__asm nop
	}

	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra != 0 ? CPU.GPR[ra] + simm16 : simm16;
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
	/*
		if(Memory.MemFlags.IsFlag(code))
		{
			ConLog.Warning("Flag: %x", code);
			CPU.SetBranch(code);
			return;
		}
	*/
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));
		Emu.Pause();
		for(uint i=0; i<128; ++i) ConLog.Write("r%d = 0x%x", i, CPU.GPR[i]);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP