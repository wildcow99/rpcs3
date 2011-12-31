#pragma once

#define OP_REG const u32
#define OP_sIMM const s32
#define OP_uIMM const u32
#define START_OPCODES_GROUP(x)
#define ADD_OPCODE(name, regs) virtual void(##name##)##regs##=0
#define ADD_NULL_OPCODE(name) virtual void(##name##)()=0
#define END_OPCODES_GROUP(x) 

enum SPU_MainOpcodes
{
	AI = 0x0e,
};

class SPU_Opcodes
{
public:
	virtual void Exit()=0;
	
	ADD_NULL_OPCODE(NOP);

	ADD_OPCODE(AI,(OP_REG rt, OP_REG ra, OP_sIMM simm16));

	ADD_OPCODE(UNK,(const s32 code, const s32 opcode, const s32 gcode));
};

#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP