#pragma once

#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/Cell/Decoder.h"


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

class PPU_Decoder : public Decoder
{
	u32 m_code;
	PPU_Opcodes& m_op;

	OP_REG VRD()		const { return GetField(6, 10); }
	OP_REG VRA()		const { return GetField(11, 15); }
	OP_REG VRB()		const { return GetField(16, 20); }

	OP_REG RD()			const { return (m_code >> 21) & 0x1f/*GetField(6, 10)*/; }
	OP_REG RS()			const { return (m_code >> 21) & 0x1f/*GetField(6, 10)*/; }
	OP_REG RT()			const { return (m_code >> 21) & 0x1f/*GetField(6, 10)*/; }
	OP_REG RA()			const { return (m_code >> 16) & 0x1f/*GetField(11, 15)*/; }
	OP_REG RB()			const { return GetField(16, 20); }
	OP_REG NB()			const { return GetField(16, 20); }
	
	OP_REG BT()			const { return GetField(6, 10); }
	OP_REG BA()			const { return GetField(11, 15); }
	OP_REG BB()			const { return GetField(16, 20); }
	OP_REG BF()			const { return GetField(6, 10); }
	OP_REG BO()			const { return GetField(6, 10); }
	OP_REG BI()			const { return GetField(11, 15); }
	OP_sIMM BD()		const { return (s32)(s16)GetField(16, 31); }
	OP_REG BH()			const { return GetField(19, 20); }
	OP_REG BFA()		const { return GetField(11, 13); }
	
	OP_REG TH()			const { return GetField(6, 10); }

	OP_REG mb()			const { return  GetField(21, 25) | (m_code & 0x20); }
	OP_REG me()			const { return  GetField(21, 25) | (m_code & 0x20); }
	OP_REG sh()			const { return  GetField(16, 20) | ((m_code & 0x2) << 4); }

	OP_REG MB()			const { return GetField(21, 25); }
	OP_REG ME()			const { return GetField(26, 30); }
	
	OP_REG SH()			const { return GetField(16, 20); }
	OP_REG AA()			const { return GetField(30); }
	OP_sIMM LL() const
	{
		OP_sIMM ll = m_code & 0x03fffffc;
		if (ll & 0x02000000) return ll - 0x04000000;
		return ll;
	}
	OP_REG LK()			const { return GetField(31); }
	
	OP_REG OE()			const { return GetField(21); }
	OP_REG L()			const { return GetField(10); }
	OP_REG I()			const { return GetField(16, 19); }
	OP_REG DQ()			const { return GetField(16, 27); }
	
	OP_REG FRT()		const { return GetField(6, 10); }
	OP_REG FRS()		const { return GetField(6, 10); }
	OP_REG FLM()		const { return GetField(7, 14); }
	OP_REG FRA()		const { return GetField(11, 15); }
	OP_REG FRB()		const { return GetField(16, 20); }
	OP_REG FRC()		const { return GetField(21, 25); }
	OP_REG FXM()		const { return GetField(12, 19); }

	OP_REG SPR()		const { return GetField(11, 20); }

	const s32 SYS()		const { return GetField(6, 31); }
	
	OP_sIMM D()			const { return (s32)(s16)GetField(16, 31); }
	OP_sIMM DS() const
	{
		OP_sIMM d = D();
		if(d < 0) return d - 1;
		return d;
	}

	OP_sIMM simm16()	const { return (s32)(s16)m_code; }
	OP_uIMM uimm16()	const { return (u32)(u16)m_code; }
	OP_uIMM uimm26()	const { return m_code & 0x3fffffc; }
	
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
	PPU_Decoder(PPU_Opcodes& op) : m_op(op)
	{
	}

	~PPU_Decoder()
	{
		m_op.Exit();
	}

	virtual void Decode(const u32 code)
	{
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
		START_OPCODES_GROUP_(G_04, (((m_code)>>1)&0x3ff))
			ADD_OPCODE(VXOR,(VRD(), VRA(), VRB()));
		END_OPCODES_GROUP(G_04);

		ADD_OPCODE(MULLI,(RT(), RA(), simm16()));
		ADD_OPCODE(SUBFIC,(RT(), RA(), simm16()));
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
			ADD_OPCODE(BCTRL, ());
		END_OPCODES_GROUP(G_13);
	
		ADD_OPCODE(RLWINM,(RA(), RS(), SH(), MB(), ME(), RC()));
		ADD_OPCODE(RLWNM,(RA(), RS(), RB(), MB(), ME(), RC()));
		ADD_OPCODE(ORI,(RS(), RA(), uimm16()));
		ADD_OPCODE(ORIS,(RS(), RA(), uimm16()));
		ADD_OPCODE(XORI,(RS(), RA(), uimm16()));
		ADD_OPCODE(XORIS,(RS(), RA(), uimm16()));
		ADD_OPCODE(ANDI_,(RA(), RS(), uimm16()));
		ADD_OPCODE(ANDIS_,(RA(), RS(), uimm16()));
	
		START_OPCODES_GROUP_(G_1e, GetField(28, 29))
			ADD_OPCODE(RLDICL,(RA(), RS(), sh(), mb(), RC()));
			ADD_OPCODE(RLDICR,(RA(), RS(), sh(), me(), RC()));
			ADD_OPCODE(RLDIC,(RA(), RS(), sh(), mb(), RC()));
			ADD_OPCODE(RLDIMI,(RA(), RS(), sh(), mb(), RC()));
		END_OPCODES_GROUP(G_1e);

		START_OPCODES_GROUP_(G_1f, GetField(21, 30))
			/*0x000*/ADD_OPCODE(CMP,(BF(), L(), RA(), RB()));
			/*0x00a*/ADD_OPCODE(ADDC,(RT(), RA(), RB(), OE(), RC()));
			/*0x013*/ADD_OPCODE(MFOCRF,(GetField(11), FXM(), RT()));
			/*0x014*/ADD_OPCODE(LWARX,(RD(), RA(), RB()));
			/*0x015*/ADD_OPCODE(LDX,(RA(), RS(), RB()));
			/*0x017*/ADD_OPCODE(LWZX,(RD(), RA(), RB()));
			/*0x018*/ADD_OPCODE(SLW,(RA(), RS(), RB(), RC()));
			/*0x01a*/ADD_OPCODE(CNTLZW,(RS(), RA(), RC()));
			/*0x01b*/ADD_OPCODE(SLD,(RA(), RS(), RB(), RC()));
			/*0x01c*/ADD_OPCODE(AND,(RA(), RS(), RB(), RC()));
			/*0x020*/ADD_OPCODE(CMPL,(BF(), L(), RA(), RB()));
			/*0x028*/ADD_OPCODE(SUBF,(RT(), RA(), RB(), OE(), RC()));
			/*0x036*/ADD_OPCODE(DCBST,(RA(), RB()));
			/*0x03a*/ADD_OPCODE(CNTLZD,(RA(), RS(), RC()));
			/*0x03c*/ADD_OPCODE(ANDC,(RS(), RA(), RB(), RC()));
			/*0x054*/ADD_OPCODE(LDARX,(RD(), RA(), RB()));
			/*0x056*/ADD_OPCODE(DCBF,(RA(), RB()));
			/*0x057*/ADD_OPCODE(LBZX,(RD(), RA(), RB()));
			/*0x067*/ADD_OPCODE(LVX,(VRD(), RA(), RB()));
			/*0x068*/ADD_OPCODE(NEG,(RT(), RA(), OE(), RC()));
			/*0x077*/ADD_OPCODE(LBZUX,(RD(), RA(), RB()));
			/*0x07c*/ADD_OPCODE(NOR,(RA(), RS(), RB(), RC()));
			/*0x08a*/ADD_OPCODE(ADDE,(RD(), RA(), RB(), OE(), RC()));
			/*0x090*/ADD_OPCODE(MTOCRF,(FXM(), RS()));
			/*0x096*/ADD_OPCODE(STWCX_,(RS(), RA(), RB()));
			/*0x097*/ADD_OPCODE(STWX,(RS(), RA(), RB()));
			/*0x0b5*/ADD_OPCODE(STDUX,(RS(), RA(), RB()));
			/*0x0ca*/ADD_OPCODE(ADDZE,(RS(), RA(), OE(), RC()));
			/*0x0e7*/ADD_OPCODE(STVX,(VRD(), RA(), RB()));
			/*0x0ea*/ADD_OPCODE(ADDME,(RS(), RA(), OE(), RC()));
			/*0x0eb*/ADD_OPCODE(MULLW,(RT(), RA(), RB(), OE(), RC()));
			/*0x0f6*/ADD_OPCODE(DCBTST,(TH(), RA(), RB()));
			/*0x108*///DOZ
			/*0x10a*/ADD_OPCODE(ADD,(RT(), RA(), RB(), OE(), RC()));
			/*0x116*/ADD_OPCODE(DCBT,(RA(), RB(), TH()));
			/*0x117*/ADD_OPCODE(LHZX,(RD(), RA(), RB()));
			/*0x11c*/ADD_OPCODE(EQV,(RA(), RS(), RB(), RC()));
			/*0x136*/ADD_OPCODE(ECIWX,(RD(), RA(), RB()));
			/*0x14b*/ADD_OPCODE(DIV,(RT(), RA(), RB(), OE(), RC()));
			/*0x137*/ADD_OPCODE(LHZUX,(RD(), RA(), RB()));
			/*0x13c*/ADD_OPCODE(XOR,(RA(), RS(), RB(), RC()));
			/*0x153*/ADD_OPCODE(MFSPR,(RD(), SPR()));
			/*0x157*/ADD_OPCODE(LHAX,(RD(), RA(), RB()));
			/*0x168*/ADD_OPCODE(ABS,(RT(), RA(), OE(), RC()));
			/*0x16b*///DIVS
			/*0x173*/ADD_OPCODE(MFTB,(RT()));
			/*0x177*/ADD_OPCODE(LHAUX,(RD(), RA(), RB()));
			/*0x1b6*/ADD_OPCODE(ECOWX,(RS(), RA(), RB()));
			/*0x1bc*/ADD_OPCODE(OR,(RA(), RS(), RB()));
			/*0x1c9*/ADD_OPCODE(DIVDU,(RD(), RA(), RB(), OE(), RC()));
			/*0x1cb*/ADD_OPCODE(DIVWU,(RD(), RA(), RB(), OE(), RC()));
			/*0x1d3*/ADD_OPCODE(MTSPR,(SPR(), RS()));
			/*0x1d6*///DCBI
			/*0x1e9*///DIVD
			/*0x1eb*///DIVW
			/*0x217*///LFSX
			/*0x218*/ADD_OPCODE(SRW,(RA(), RS(), RB(), RC()));
			/*0x21b*/ADD_OPCODE(SRD,(RA(), RS(), RB(), RC()));
			/*0x237*///LFSUX
			/*0x257*///LFDX
			/*0x28a*///LDUX
			/*0x277*///LFDUX
			/*0x316*///LHBRX
			/*0x318*/ADD_OPCODE(SRAW,(RA(), RS(), RB(), RC()));
			/*0x31A*/ADD_OPCODE(SRAD,(RA(), RS(), RB(), RC()));
			/*0x338*/ADD_OPCODE(SRAWI,(RA(), RS(), sh(), RC()));
			/*0x33a*/ADD_OPCODE(SRADI1,(RA(), RS(), sh(), RC()));
			/*0x33b*/ADD_OPCODE(SRADI2,(RA(), RS(), sh(), RC()));
			/*0x356*/ADD_OPCODE(EIEIO,());
			/*0x39a*/ADD_OPCODE(EXTSH,(RA(), RS(), RC()));
			/*0x3ba*/ADD_OPCODE(EXTSB,(RA(), RS(), RC()));
			/*0x3d7*/ADD_OPCODE(STFIWX,(FRS(), RA(), RB()));
			/*0x3da*/ADD_OPCODE(EXTSW,(RA(), RS(), RC()));
			/*0x3d6*///ICBI
			/*0x3f6*///DCBZ
		END_OPCODES_GROUP(G_1f);
		
		ADD_OPCODE(LWZ,(RT(), RA(), D()));
		ADD_OPCODE(LWZU,(RT(), RA(), DS()));
		ADD_OPCODE(LBZ,(RT(), RA(), D()));
		ADD_OPCODE(LBZU,(RT(), RA(), DS()));
		ADD_OPCODE(STW,(RS(), RA(), D()));
		ADD_OPCODE(STWU,(RS(), RA(), DS()));
		ADD_OPCODE(STB,(RS(), RA(), D()));
		ADD_OPCODE(STBU,(RS(), RA(), DS()));
		ADD_OPCODE(LHZ,(RS(), RA(), D()));
		ADD_OPCODE(LHZU,(RS(), RA(), DS()));
		ADD_OPCODE(STH,(RS(), RA(), D()));
		ADD_OPCODE(LFS,(FRT(), RA(), D()));
		ADD_OPCODE(LFSU,(FRT(), RA(), DS()));
		ADD_OPCODE(LFD,(FRT(), RA(), D()));
		ADD_OPCODE(LFDU,(FRT(), RA(), DS()));
		ADD_OPCODE(STFS,(FRS(), RA(), D()));
		ADD_OPCODE(STFD,(FRS(), RA(), D()));

		START_OPCODES_GROUP_(G_3a, GetField(30, 31))
			ADD_OPCODE(LD,(RT(), RA(), D()));
			ADD_OPCODE(LDU,(RT(), RA(), DS()));
		END_OPCODES_GROUP(G_3a);

		START_OPCODES_GROUP_(G_3b, GetField(26, 30))
			ADD_OPCODE(FDIVS,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FSUBS,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FADDS,	(FRT(), FRA(), FRB(), RC()));
			ADD_OPCODE(FSQRTS,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FRES,	(FRT(), FRB(), RC()));
			ADD_OPCODE(FMULS,	(FRT(), FRA(), FRC(), RC()));
			ADD_OPCODE(FMADDS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FMSUBS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMSUBS,	(FRT(), FRA(), FRB(), FRC(), RC()));
			ADD_OPCODE(FNMADDS,	(FRT(), FRA(), FRB(), FRC(), RC()));
		END_OPCODES_GROUP(G_3b);
		
		START_OPCODES_GROUP_(G_3e, GetField(30, 31))
			ADD_OPCODE(STD,(RS(), RA(), D()));
			ADD_OPCODE(STDU,(RS(), RA(), DS()));
		END_OPCODES_GROUP(G_3e);

		START_OPCODES_GROUP_(G_3f, GetField(21, 30))
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
		END_OPCODES_GROUP(G_3f);

		default: m_op.UNK(m_code, opcode, opcode); break;
		}
	}
};

#undef START_OPCODES_GROUP_
#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP