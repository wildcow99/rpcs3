#pragma once

#include "Emu/Cell/SPUOpcodes.h"


#define START_OPCODES_GROUP_(group, reg) \
	case(##group##): \
		temp=##reg##;\
		switch(temp)\
		{

#define START_OPCODES_GROUP(group, reg) START_OPCODES_GROUP_(##group##, ##reg##())

#define END_OPCODES_GROUP(group) \
		default:\
			m_op.UNK(m_code, opcode, temp);\
		break;\
		}\
	break

#define ADD_OPCODE(name, regs) case(##name##):m_op.##name####regs##; break
#define ADD_NULL_OPCODE(name) ADD_OPCODE(##name##, ())

class SPU_Decoder
{
	int m_code;
	SPU_Opcodes& m_op;

	OP_REG RS()			const { return GetField(6, 10); }
	OP_REG RT()			const { return GetField(6, 10); }
	OP_REG RA()			const { return GetField(11, 15); }
	OP_REG RB()			const { return GetField(16, 20); }

	OP_sIMM simm16()	const { return (s32)(s16)(m_code & 0xffff); }
	OP_uIMM uimm16()	const { return m_code & 0xffff; }
	
	__forceinline u32 GetField(const u32 p) const
	{
		return (m_code >> (31 - p)) & 0x1;
	}
	
	__forceinline u32 GetField(const u32 from, const u32 to) const
	{
		return (m_code >> (31 - to)) & ((1 << ((to - from) + 1)) - 1);
	}
	
public:
	SPU_Decoder(SPU_Opcodes& op) : m_op(op)
	{
	}

	~SPU_Decoder()
	{
		m_op.Exit();
	}

	void DoCode(const int code)
	{
		m_op.Step();

		if(code == 0)
		{
			m_op.NOP();
			return;
		}

		m_code = code;
		const u32 opcode = (code >> 26) & 0x3f;

		//s32 temp;
		switch(opcode)
		{
		ADD_OPCODE(AI,(RT(), RA(), simm16()));
		default: m_op.UNK(m_code, opcode, opcode); break;
		}
	}
};

#undef START_OPCODES_GROUP_
#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP