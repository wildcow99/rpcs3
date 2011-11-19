#pragma once

#define OP_REG const u32
#define OP_sIMM const s32
#define OP_uIMM const u32
#define START_OPCODES_GROUP(x) /*x*/
#define ADD_OPCODE(name, regs) virtual void(##name##)##regs##=0
#define ADD_NULL_OPCODE(name) virtual void(##name##)()=0
#define END_OPCODES_GROUP(x) /*x*/

enum PPU_MainOpcodes
{
	DOZI = 0x09,
	CMPLI = 0x0a,
	CMPI = 0x0b,
	ADDIC = 0x0c,
	ADDIC_ = 0x0d,
	ADDI = 0x0e,
	ADDIS = 0x0f,
	BC = 0x10,
	SC = 0x11,
	B = 0x12,
	G_13 = 0x13,
	RLWINM = 0x15,
	ORI = 0x18,
	ORIS = 0x19,
	ANDI_ = 0x1c,
	ANDIS_ = 0x1d,
	G_0x1e = 0x1e,
	G_1f = 0x1f,
	LWZ = 0x20,
	LBZ = 0x22,
	LBZU = 0x23,
	STW = 0x24,
	STB = 0x26,
	LHZ = 0x28,
	LHZU = 0x29,
	LHA = 0x2a,
	LHAU = 0x2b,
	STH = 0x2c,
/**/	LMW = 0x2e,
	LFS = 0x30,
	LFSU = 0x31,
	LFD = 0x32,
	LFDU = 0x33,
	LFQ = 0x38,
	LFQU = 0x39,
	G_3a = 0x3a,
	G4_S = 0x3b,
	G_3e = 0x3e,
	//STD = 0x3e,
	G5 = 0x3f,
};

enum G_13Opcodes //Field 21 - 30
{
	BCLR = 0x10,
	BLR = 0x20,
	CRNOR = 0x21,
	CRANDC = 0x81,
	ISYNC = 0x96,
	CRXOR = 0xc1,
	CRNAND = 0xe1,
	CRAND = 0x101,
	CREQV = 0x121,
	CROR = 0x1c1,
	CRORC = 0x1a1,
	BCCTR = 0x210,
	BCTR = 0x420,
	BCTRL = 0x421,
};

enum G_1eOpcodes //Field 27 - 29
{
	CLRLDI = 0x10,
	RLDICL = 0x81,
};

enum G_1fOpcodes //Field 22 - 30
{
	CMP = 0x0,
	ADDC = 0xa,
	LDX = 0x15,
	CNTLZW = 0x1a,
	AND = 0x1c,
	CMPL = 0x1f,
	CMPLD = 0x20,
	SUBF = 0x28,
	LDUX = 0x35,
	DCBST = 0x36,
	CNTLZD = 0x3a,
	ANDC = 0x3c,
	LDARX = 0x54,
	DCBF = 0x56,
	LBZX = 0x57,
	NEG = 0x68,
	LBZUX = 0x77,
	ADDE = 0x8a,
	ADDZE = 0xca,
	ADDME = 0xea,
	DCBTST = 0xf6,
	DOZ = 0x108,
	ADD = 0x10a,
	LHZX = 0x117,
	EQV = 0x11c,
	ECIWX = 0x136,
	XOR = 0x13c,
	DIV = 0x14b,
	//LHZUX = 0x14b, //ERROR!!!
	MFLR = 0x153,
	LHAX = 0x157,
	ABS = 0x168,
	DIVS = 0x16b,
	LHAUX = 0x177,
	EXTSH = 0x19a,
	ECOWX = 0x1b6,
	MR = 0x1bc,
	DIVDU = 0x1c9,
	DIVWU = 0x1cb,
	G_1f_1d3 = 0x1d3,
	DCBI = 0x1d6,
	EXTSW = 0x1da,
	DIVD = 0x1e9,
	DIVW = 0x1eb,
	LFSX = 0x217,
	LFSUX = 0x237,
	LFDX = 0x257,
	DCLST = 0x276,
	LFDUX = 0x277,
	LHBRX = 0x316,
	LFQX = 0x317,
	LFQUX = 0x337,
	EIEIO = 0x356,
	EXTSB = 0x3ba,
	ICBI = 0x3d6,
	DCBZ = 0x3f6,
};

enum G_1f_1d3Opcodes //Field 11 - 21
{
	MTLR = 0x200,
	MTCTR = 0x240,
};

enum G_3aOpcodes //Field 30 - 31
{
	LD = 0x0,
	LDU = 0x1,
};

enum G4_SOpcodes //Field 21 - 30
{
	FSUBS = 0x14,
	FADDS = 0x15,
	FSQRTS = 0x16,
	FRES = 0x18,
	FMULS = 0x19,
	FMADDS = 0x1d,
	FMSUBS = 0x1c,
	FNMSUBS = 0x1e,
	FNMADDS = 0x1f,
};

enum G_3eOpcodes //Field 30 - 31
{
	STD = 0x0,
	STDU = 0x1,
};

enum G5Opcodes //Field 21 - 30
{	
	MTFSB1 = 0x26,
	MCRFS = 0x40,
	MTFSB0 = 0x46,
	MTFSFI = 0x86,
	MFFS = 0x247,
	MTFSF = 0x303,

	FCMPU = 0x0,
	FRSP = 0xc,
	FCTIW = 0xe,
	FCTIWZ = 0xf,
	FDIV = 0x12,
	FSUB = 0x14,
	FADD = 0x15,
	FSQRT = 0x16,
	FSEL = 0x17,
	FMUL = 0x19,
	FRSQRTE = 0x1a,
	FMSUB = 0x1c,
	FMADD = 0x1d,
	FNMSUB = 0x1e,
	FNMADD = 0x1f,
	FCMPO = 0x20,
	FNEG = 0x28,
	FMR = 0x48,
	FNABS = 0x88,
	FABS = 0x108,
	FCTID = 0x32e,
	FCTIDZ = 0x32f,
	FCFID = 0x34e,
};

//118

class PPU_Opcodes
{
public:
	virtual void Exit()=0;
	virtual void Step()=0;

	static int branchTarget(const u32 pc, const int imm)
	{
        return pc + ext_s26(imm);
    }

	static int condBranchTarget(const u32 pc, const int imm_s16)
	{
        return pc + (imm_s16 & ~0x3);
    }

	static int ext_s26(const int val)
	{
		return val & 0x2000000 ? val | 0xfc000000 : val;
	}

	static int ext_s24(const int val)
	{
		return val & 0x800000 ? val | 0xff000000 : val;
	}

	static int ext_s16(const int val)
	{
		return val & 0x8000 ? val | 0xffff0000 : val;
	}

	static int ext_s8(const int val)
	{
		return val & 0x80 ? val | 0xffffff00 : val;
	}

	ADD_NULL_OPCODE(NOP);
	
	ADD_OPCODE(CMPLI,(OP_REG bf, OP_REG l, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(CMPI,(OP_REG bf, OP_REG l, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(ADDIC,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(ADDIC_,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(ADDI,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(ADDIS,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(BC,(OP_REG bo, OP_REG bi, OP_sIMM bd, OP_REG aa, OP_REG lk));
	ADD_OPCODE(SC,(const s32 sc_code));
	ADD_OPCODE(B,(OP_sIMM ll, OP_REG aa, OP_REG lk));
	
	START_OPCODES_GROUP(G_13)
		ADD_OPCODE(BCLR,(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk));
		ADD_OPCODE(BLR, ());
		ADD_OPCODE(CRNOR,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CRANDC,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CRXOR,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CRNAND,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CRAND,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CREQV,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CROR,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(CRORC,(OP_REG bt, OP_REG ba, OP_REG bb));
		ADD_OPCODE(BCCTR,(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk));
		ADD_OPCODE(BCTR, ());
		ADD_OPCODE(BCTRL, ());
	END_OPCODES_GROUP(G_13);
	
	ADD_OPCODE(RLWINM,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, OP_REG me, bool rc));
	ADD_OPCODE(ORI,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(ORIS,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(ANDI_,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(ANDIS_,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));

	START_OPCODES_GROUP(G_1e)
		ADD_OPCODE(CLRLDI,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
		ADD_OPCODE(RLDICL,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc));
	END_OPCODES_GROUP(G_1e);

	START_OPCODES_GROUP(G_1f)
		ADD_OPCODE(CMP,(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb));
		ADD_OPCODE(ADDC,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		
		//ADD_OPCODE(LDX,(OP_REG rt, OP_REG ra, OP_REG rb));//
		
		ADD_OPCODE(CNTLZW,(OP_REG rs, OP_REG ra, bool rc));
		ADD_OPCODE(AND,(OP_REG rs, OP_REG ra, OP_REG rb, bool rc));
		ADD_OPCODE(CMPL,(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb, bool rc));
		ADD_OPCODE(CMPLD,(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb, bool rc));
		ADD_OPCODE(SUBF,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		
		//ADD_OPCODE(LDUX,(OP_REG rt, OP_REG ra, OP_REG rb));//
		
		ADD_OPCODE(DCBST,(OP_REG ra, OP_REG rb));
		ADD_OPCODE(CNTLZD,(OP_REG ra, OP_REG rs, bool rc));
		ADD_OPCODE(ANDC,(OP_REG rs, OP_REG ra, OP_REG rb, bool rc));
		
		//ADD_OPCODE(LDARX,(OP_REG rd, OP_REG ra, OP_REG rb));//
		
		ADD_OPCODE(DCBF,(OP_REG ra, OP_REG rb));
		
		//ADD_OPCODE(LBZX,(OP_REG rt, OP_REG ra, OP_REG rb));//
		
		ADD_OPCODE(NEG,(OP_REG rt, OP_REG ra, OP_REG oe, bool rc));
		ADD_OPCODE(ADDE,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		ADD_OPCODE(ADDZE,(OP_REG rs, OP_REG ra, OP_REG oe, bool rc));
		ADD_OPCODE(ADDME,(OP_REG rs, OP_REG ra, OP_REG oe, bool rc));
		ADD_OPCODE(DCBTST,(OP_REG th, OP_REG ra, OP_REG rb));
		
		//ADD_OPCODE(DOZ,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));//
		
		ADD_OPCODE(ADD,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*
		ADD_OPCODE(LHZX,());//
		ADD_OPCODE(EQV,());//
		ADD_OPCODE(ECIWX,());//*/

		ADD_OPCODE(XOR,(OP_REG rt, OP_REG ra, OP_REG rb, bool rc));
		ADD_OPCODE(DIV,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		ADD_OPCODE(MFLR,(OP_REG rt));
		ADD_OPCODE(ABS,(OP_REG rt, OP_REG ra, OP_REG oe, bool rc));
		/*
		ADD_OPCODE(DIVS,());//
		ADD_OPCODE(LHAX,());//
		ADD_OPCODE(LHAUX,());//
		ADD_OPCODE(ECOWX,());//
		ADD_OPCODE(DIVDU,());//
		ADD_OPCODE(DIVWU,());//
		*/
		ADD_OPCODE(EXTSH,(OP_REG ra, OP_REG rs, bool rc));
		ADD_OPCODE(MR,(OP_REG ra, OP_REG rb));

		START_OPCODES_GROUP(G_1f_1d3)
				ADD_OPCODE(MTLR,(OP_REG rt));
				ADD_OPCODE(MTCTR,(OP_REG rt));
		END_OPCODES_GROUP(G_1f_1d3);
		

		ADD_OPCODE(DCBI,(OP_REG ra, OP_REG rb));
		ADD_OPCODE(EXTSW,(OP_REG ra, OP_REG rs, bool rc));
		/*
		ADD_OPCODE(DIVD,());//
		ADD_OPCODE(DIVW,());//
		ADD_OPCODE(LFSX,());//
		ADD_OPCODE(LFSUX,());//
		ADD_OPCODE(LFDX,());//
		*/
		ADD_OPCODE(DCLST,(OP_REG ra, OP_REG rb, bool rc));
		/*
		ADD_OPCODE(LFDUX,());//
		ADD_OPCODE(LHBRX,());//
		ADD_OPCODE(LFQX,());//
		ADD_OPCODE(LFQUX,());//
		ADD_OPCODE(EIEIO,());//
		ADD_OPCODE(EXTSH,());//
		ADD_OPCODE(EXTSB,());//
		ADD_OPCODE(ICBI,());//
		*/
		ADD_OPCODE(DCBZ,(OP_REG ra, OP_REG rb));
	END_OPCODES_GROUP(G_1f);
	
	ADD_OPCODE(LWZ,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(LBZ,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STH,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STW,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STB,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(LHZ,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(LHZU,(OP_REG rs, OP_REG ra, OP_sIMM ds));

	START_OPCODES_GROUP(G_3a)
		ADD_OPCODE(LD,(OP_REG rt, OP_REG ra, OP_sIMM ds));
		ADD_OPCODE(LDU,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G4_S)
		ADD_OPCODE(FSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FADDS,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FSQRTS,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FRES,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FMULS,(OP_REG frt, OP_REG fra, OP_REG frc, bool rc));
		ADD_OPCODE(FMADDS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FMSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMADDS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
	END_OPCODES_GROUP(G4_S);
	
	START_OPCODES_GROUP(G_3e)
		ADD_OPCODE(STD,(OP_REG rs, OP_REG ra, OP_sIMM ds));
		ADD_OPCODE(STDU,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	END_OPCODES_GROUP(G_3e);

	START_OPCODES_GROUP(G5)
		ADD_OPCODE(MTFSB1,(OP_REG bt, bool rc));
		ADD_OPCODE(MCRFS,(OP_REG bf, OP_REG bfa));
		ADD_OPCODE(MTFSB0,(OP_REG bt, bool rc));
		ADD_OPCODE(MTFSFI,(OP_REG bf, OP_REG i, bool rc));
		ADD_OPCODE(MFFS,(OP_REG frt, bool rc));
		ADD_OPCODE(MTFSF,(OP_REG flm, OP_REG frb, bool rc));

		ADD_OPCODE(FCMPU,(OP_REG bf, OP_REG fra, OP_REG frb));
		ADD_OPCODE(FRSP,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FCTIW,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FCTIWZ,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FDIV,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FSUB,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FADD,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FSQRT,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FSEL,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FMUL,(OP_REG frt, OP_REG fra, OP_REG frc, bool rc));
		ADD_OPCODE(FRSQRTE,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FMSUB,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FMADD,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMSUB,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMADD,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FCMPO,(OP_REG bf, OP_REG fra, OP_REG frb));
		ADD_OPCODE(FNEG,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FMR,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FNABS,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FABS,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FCTID,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FCTIDZ,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FCFID,(OP_REG frt, OP_REG frb, bool rc));
	END_OPCODES_GROUP(G5);

	ADD_OPCODE(UNK,(const s32 code, const s32 opcode, const s32 gcode));
};

#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP