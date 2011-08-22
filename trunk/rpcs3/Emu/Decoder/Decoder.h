#pragma once

#include "Emu/Opcodes/Opcodes.h"
//#include "PPCDecoder.h"


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

class Decoder
{
	int m_code;
	Opcodes& m_op;

	OP_REG RS()			const { return GetField(6, 10); }
	OP_REG RT()			const { return GetField(6, 10); }
	OP_REG RA()			const { return GetField(11, 15); }
	OP_REG RB()			const { return GetField(16, 20); }
	OP_REG NB()			const { return GetField(16, 20); }
	
	OP_REG BT()			const { return GetField(6, 10); }
	OP_REG BA()			const { return GetField(11, 15); }
	OP_REG BB()			const { return GetField(16, 20); }
	OP_REG BF()			const { return GetField(6, 10); }
	OP_REG BO()			const { return GetField(6, 10); }
	OP_REG BI()			const { return GetField(11, 15); }
	OP_REG BD()			const { return GetField(16, 31); }
	OP_REG BH()			const { return GetField(19, 20); }
	OP_REG BFA()		const { return GetField(11, 13); }
	
	OP_REG TH()			const { return GetField(6, 10); }
	
	OP_REG AA()			const { return GetField(30); }
	OP_sIMM LL() const
	{
		OP_sIMM ll = GetField(6, 31) & 0xfffffc;
		if(((s8)(ll >> 16)) < 0) return 0xff000000 | ll;
		return ll;
	}
	OP_REG LK()			const { return GetField(31); }
	
	OP_REG OE()			const { return GetField(21); }
	OP_REG L()			const { return GetField(10); }
	OP_REG I()			const { return GetField(16, 19); }
	OP_REG DQ()			const { return GetField(16, 27); }
	
	OP_REG FRT()		const { return GetField(6, 10); }
	OP_REG FLM()		const { return GetField(7, 14); }
	OP_REG FRA()		const { return GetField(11, 15); }
	OP_REG FRB()		const { return GetField(16, 20); }
	OP_REG FRC()		const { return GetField(21, 25); }

	const s32 SYS()		const { return GetField(6, 31); }
	
	OP_sIMM D()			const { return (s32)(s16)GetField(16, 31); }
	OP_sIMM DS() const
	{
		OP_sIMM d = D();
		if(d < 0) return d - 1;
		return d;
	}

	OP_sIMM simm16()	const { return (s32)(s16)(m_code & 0xffff); }
	OP_uIMM uimm16()	const { return m_code & 0xffff; }
	
	const bool RC()	const { return m_code & 0x1; }
	
	__forceinline u32 GetField(const u32 p) const
	{
		return (m_code >> (31 - p)) & 0x1;
	}
	
	__forceinline u32 GetField(const u32 from, const u32 to) const
	{
		return (m_code >> (31 - to)) & ((1 << ((to - from) + 1)) - 1);
	}
	
public:
	Decoder(Opcodes& op) : m_op(op)
	{
	}

	~Decoder()
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

		s32 temp;
		switch(opcode)
		{
		ADD_OPCODE(CMPLI,(BF(), L(), RA(), uimm16()));
		ADD_OPCODE(CMPI,(BF(), L(), RA(), simm16()));
		ADD_OPCODE(ADDIC,(RT(), RA(), simm16()));
		ADD_OPCODE(ADDIC_,(RT(), RA(), simm16()));
		ADD_OPCODE(ADDI,(RT(), RA(), simm16()));
		ADD_OPCODE(ADDIS,(RT(), RA(), simm16()));
		ADD_OPCODE(BC,(BO(), BI(), BD(), AA(), LK()));
		ADD_OPCODE(SC,(SYS()));
		ADD_OPCODE(B,(LL(), AA(), LK()));
	
		START_OPCODES_GROUP_(G_13, GetField(21, 31))
			ADD_OPCODE(BCLR,(BO(), BI(), BH(), LK()));
			ADD_OPCODE(BLR,());
			ADD_OPCODE(CRNOR,(BT(), BA(), BB()));
			ADD_OPCODE(CRANDC,(BT(), BA(), BB()));
			ADD_OPCODE(CRXOR,(BT(), BA(), BB()));
			ADD_OPCODE(CRNAND,(BT(), BA(), BB()));
			ADD_OPCODE(CRAND,(BT(), BA(), BB()));
			ADD_OPCODE(CREQV,(BT(), BA(), BB()));
			ADD_OPCODE(CROR,(BT(), BA(), BB()));
			ADD_OPCODE(CRORC,(BT(), BA(), BB()));
			ADD_OPCODE(BCCTR,(BO(), BI(), BH(), LK()));
			ADD_OPCODE(BCTR, ());
		END_OPCODES_GROUP(G_13);
	
		ADD_OPCODE(ORI,(RS(), RA(), uimm16()));
		ADD_OPCODE(ORIS,(RS(), RA(), uimm16()));
		ADD_OPCODE(ANDI_,(RS(), RA(), uimm16()));
		ADD_OPCODE(ANDIS_,(RS(), RA(), uimm16()));
	
		START_OPCODES_GROUP_(G_0x1e, GetField(22, 30))
			ADD_OPCODE(CLRLDI,(RS(), RA(), uimm16()));
		END_OPCODES_GROUP(G_0x1e);

		START_OPCODES_GROUP_(G_1f, GetField(22, 30))
			ADD_OPCODE(CMP,(BF(), L(), RA(), RB()));
			ADD_OPCODE(ADDC,(RT(), RA(), RB(), OE(), RC()));
			ADD_OPCODE(CNTLZW,(RS(), RA(), RC()));
			ADD_OPCODE(AND,(RS(), RA(), RB(), RC()));
			ADD_OPCODE(CMPL,(BF(), L(), RA(), RB(), RC()));
			ADD_OPCODE(CMPLD,(BF(), L(), RA(), RB(), RC()));
			ADD_OPCODE(DCBST,(RA(), RB()));
			ADD_OPCODE(CNTLZD,(RA(), RS(), RC()));
			ADD_OPCODE(ANDC,(RS(), RA(), RB(), RC()));
			ADD_OPCODE(DCBF,(RA(), RB()));
			ADD_OPCODE(ADDE,(RT(), RA(), RB(), OE(), RC()));
			ADD_OPCODE(ADDZE,(RS(), RA(), OE(), RC()));
			ADD_OPCODE(ADDME,(RS(), RA(), OE(), RC()));
			ADD_OPCODE(DCBTST,(TH(), RA(), RB()));
			ADD_OPCODE(ADD,(RT(), RA(), RB(), OE(), RC()));
			ADD_OPCODE(DIV,(RT(), RA(), RB(), OE(), RC()));
			ADD_OPCODE(MFLR,(RT()));
			ADD_OPCODE(ABS,(RT(), RA(), OE(), RC()));
			ADD_OPCODE(MR,(RA(), RB()));

			START_OPCODES_GROUP_(G_1f_1d3, GetField(11, 21))
				ADD_OPCODE(MTLR,(RT()));
				ADD_OPCODE(MTCTR,(RT()));
			END_OPCODES_GROUP(G_1f_1d3);

			ADD_OPCODE(DCBI,(RA(), RB()));
			ADD_OPCODE(EXTSW,(RA(), RS(), RC()));
			ADD_OPCODE(DCLST,(RA(), RB(), RC()));
			ADD_OPCODE(DCBZ,(RA(), RB()));
		END_OPCODES_GROUP(G_1f);
		
		ADD_OPCODE(LWZ,(RT(), RA(), D()));
		ADD_OPCODE(STH,(RS(), RA(), D()));
		ADD_OPCODE(STW,(RS(), RA(), D()));

		START_OPCODES_GROUP_(G_3a, GetField(30, 31))
			ADD_OPCODE(LD,(RT(), RA(), D()));
			ADD_OPCODE(LDU,(RT(), RA(), DS()));
		END_OPCODES_GROUP(G_3a);

		START_OPCODES_GROUP_(G4_S, GetField(21, 30))
			ADD_OPCODE(FSUBS,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FADDS,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FSQRTS,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FRES,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FMULS,	(FRT(), FRA(), FRC(), RC()));
			ADD_OPCODE(FMADDS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FMSUBS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMSUBS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMADDS,	(FRT(), FRA(), FRB(), FRC(), RC()));
		END_OPCODES_GROUP(G4_S);
		
		START_OPCODES_GROUP_(G_3e, GetField(30, 31))
			ADD_OPCODE(STD,(RS(), RA(), D()));
			ADD_OPCODE(STDU,(RS(), RA(), DS()));
		END_OPCODES_GROUP(G_3e);

		START_OPCODES_GROUP_(G5, GetField(21, 30))
			ADD_OPCODE(MTFSB1,	(BT(), RC()));
			ADD_OPCODE(MCRFS,	(BF(), BFA()));
			ADD_OPCODE(MTFSB0,	(BT(), RC()));
			ADD_OPCODE(MTFSFI,	(BF(), I(), RC()));
			ADD_OPCODE(MFFS,	(FRT(), RC()));
			ADD_OPCODE(MTFSF,	(FLM(), FRB(), RC()));
			
			ADD_OPCODE(FCMPU,	(BF(), FRA(), FRB()));
			ADD_OPCODE(FRSP,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FCTIW,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FCTIWZ,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FDIV,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FSUB,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FADD,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FSQRT,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FSEL,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FMUL,	(FRT(), FRA(), FRC(), RC()));
			ADD_OPCODE(FRSQRTE,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FMSUB,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FMADD,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMSUB,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMADD,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FCMPO,	(BF(), FRA(), FRB()));
			ADD_OPCODE(FNEG,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FMR,		(FRT(), FRB(), RC()));
			ADD_OPCODE(FNABS,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FABS,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FCTID,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FCTIDZ,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FCFID,	(FRT(), FRB(), RC()));
		END_OPCODES_GROUP(G5);

		default: m_op.UNK(m_code, opcode, opcode); break;
		}
	}
};

#undef START_OPCODES_GROUP_
#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP