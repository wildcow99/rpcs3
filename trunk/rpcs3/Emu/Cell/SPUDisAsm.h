#pragma once

#include "Emu/Cell/SPUOpcodes.h"
#include "Emu/Cell/DisAsm.h"
#include "Emu/Cell/SPU.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

class SPU_DisAsm 
	: public SPU_Opcodes
	, public DisAsm
{
public:
	CPUThread& CPU;

	SPU_DisAsm()
		: DisAsm(*(CPUThread*)NULL, DumpMode)
		, CPU(*(CPUThread*)NULL)
	{
	}

	SPU_DisAsm(CPUThread& cpu, DisAsmModes mode = NormalMode)
		: DisAsm(cpu, mode)
		, CPU(cpu)
	{
	}

	~SPU_DisAsm()
	{
	}

private:
	virtual void Exit()
	{
		if(m_mode == NormalMode && !disasm_frame->exit)
		{
			disasm_frame->Close();
		}

		this->~SPU_DisAsm();
	}

	virtual void Step()
	{
	}

	virtual u32 DisAsmBranchTarget(const s32 imm)
	{
		return 0;
	}

private:
	virtual void NOP()
	{
		Write( "nop" );
	}
	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		if(ra == 0)
		{
			DisAsm_R1_IMM("li", rt, simm16);
		}
		else
		{
			DisAsm_R2_IMM("ai", rt, ra, simm16);
		}
	}
	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		Write(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP