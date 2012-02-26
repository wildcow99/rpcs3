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
	G_04 = 0x04,
	MULLI = 0x07,
	SUBFIC = 0x08,
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
	XORI = 0x1a,
	XORIS = 0x1b,
	ANDI_ = 0x1c,
	ANDIS_ = 0x1d,
	G_1e = 0x1e,
	G_1f = 0x1f,
	LWZ = 0x20,
	LWZU = 0x21,
	LBZ = 0x22,
	LBZU = 0x23,
	STW = 0x24,
	STWU = 0x25,
	STB = 0x26,
	STBU = 0x27,
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
	STFS = 0x34,
	STFD = 0x36,
	LFQ = 0x38,
	LFQU = 0x39,
	G_3a = 0x3a,
	G_3b = 0x3b,
	G_3e = 0x3e,
	G_3f = 0x3f,
};


enum G_04Opcodes
{
	VXOR = 0x262,
};

enum G_13Opcodes //Field 21 - 30
{
	BCLR = 0x20,
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
	RLDICL = 0x0,
	RLDICR = 0x1,
	RLDIC = 0x2,
	RLDIMI = 0x3,
};

enum G_1fOpcodes //Field 21 - 30
{
	CMP = 0x00,
	ADDC = 0x0a,
	MFOCRF = 0x13,
	LWARX = 0x14,
	LDX = 0x15,
	LWZX = 0x17,
	SLW = 0x18,
	CNTLZW = 0x1a,
	SLD = 0x1b,
	AND = 0x1c,
	CMPL = 0x20,
	SUBF = 0x28,
	DCBST = 0x36,
	CNTLZD = 0x3a,
	ANDC = 0x3c,
	LDARX = 0x54,
	DCBF = 0x56,
	LBZX = 0x57,
	LVX = 0x67,
	NEG = 0x68,
	LBZUX = 0x77,
	NOR = 0x7c,
	ADDE = 0x8a,
	MTOCRF = 0x90,
	STWCX_ = 0x96,
	STWX = 0x97,
	ADDZE = 0xca,
	STVX = 0xe7,
	ADDME = 0xea,
	MULLW = 0xeb,
	DCBTST = 0xf6,
	DOZ = 0x108,
	ADD = 0x10a,
	DCBT = 0x116,
	LHZX = 0x117,
	EQV = 0x11c,
	ECIWX = 0x136,
	LHZUX = 0x137,
	XOR = 0x13c,
	DIV = 0x14b,
	MFSPR = 0x153,
	LHAX = 0x157,
	ABS = 0x168,
	DIVS = 0x16b,
	MFTB = 0x173,
	LHAUX = 0x177,
	ECOWX = 0x1b6,
	OR = 0x1bc,
	DIVDU = 0x1c9,
	DIVWU = 0x1cb,
	MTSPR = 0x1d3,
	DCBI = 0x1d6,
	DIVD = 0x1e9,
	DIVW = 0x1eb,
	LFSX = 0x217,
	SRW = 0x218,
	SRD = 0x21b,
	LFSUX = 0x237,
	LFDX = 0x257,
	LDUX = 0x28a,
	LFDUX = 0x277,
	LHBRX = 0x316,
	SRAWI = 0x338,
	SRADI1 = 0x33a, //sh_5 == 0
	SRADI2 = 0x33b, //sh_5 != 0
	EIEIO = 0x356,
	EXTSH = 0x39a,
	EXTSB = 0x3ba,
	STFIWX = 0x3d7,
	EXTSW = 0x3da,
	ICBI = 0x3d6,
	DCBZ = 0x3f6,
};

enum G_3aOpcodes //Field 30 - 31
{
	LD = 0x0,
	LDU = 0x1,
};

enum G_3bOpcodes //Field 26 - 30
{
	FDIVS = 0x12,
	FSUBS = 0x14,
	FADDS = 0x15,
	FSQRTS = 0x16,
	FRES = 0x18,
	FMULS = 0x19,
	FMSUBS = 0x1c,
	FMADDS = 0x1d,
	FNMSUBS = 0x1e,
	FNMADDS = 0x1f,
};

enum G_3eOpcodes //Field 30 - 31
{
	STD = 0x0,
	STDU = 0x1,
};

enum G_3fOpcodes //Field 21 - 30
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

	static u64 branchTarget(const u64 pc, const u64 imm)
	{
		return pc + (imm & ~0x3);
    }

	ADD_NULL_OPCODE(NOP);
	
	START_OPCODES_GROUP(G_04)
		ADD_OPCODE(VXOR,(OP_REG vrd, OP_REG vra, OP_REG vrb));
	END_OPCODES_GROUP(G_04);

	ADD_OPCODE(MULLI,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
	ADD_OPCODE(SUBFIC,(OP_REG rt, OP_REG ra, OP_sIMM simm16));
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
	ADD_OPCODE(XORI,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(XORIS,(OP_REG rs, OP_REG ra, OP_uIMM uimm16));
	ADD_OPCODE(ANDI_,(OP_REG ra, OP_REG rs, OP_uIMM uimm16));
	ADD_OPCODE(ANDIS_,(OP_REG ra, OP_REG rs, OP_uIMM uimm16));

	START_OPCODES_GROUP(G_1e)
		ADD_OPCODE(RLDICL,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc));
		ADD_OPCODE(RLDICR,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG me, bool rc));
		ADD_OPCODE(RLDIC,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc));
		ADD_OPCODE(RLDIMI,(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc));
	END_OPCODES_GROUP(G_1e);

	START_OPCODES_GROUP(G_1f)
		/*0x000*/ADD_OPCODE(CMP,(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb));
		/*0x00a*/ADD_OPCODE(ADDC,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x013*/ADD_OPCODE(MFOCRF,(OP_REG a, OP_REG fxm, OP_REG rt));
		/*0x014*/ADD_OPCODE(LWARX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x015*/ADD_OPCODE(LDX,(OP_REG ra, OP_REG rs, OP_REG rb));
		/*0x017*/ADD_OPCODE(LWZX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x018*/ADD_OPCODE(SLW,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x01a*/ADD_OPCODE(CNTLZW,(OP_REG rs, OP_REG ra, bool rc));
		/*0x01b*/ADD_OPCODE(SLD,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x01c*/ADD_OPCODE(AND,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x020*/ADD_OPCODE(CMPL,(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb));
		/*0x028*/ADD_OPCODE(SUBF,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x036*/ADD_OPCODE(DCBST,(OP_REG ra, OP_REG rb));
		/*0x03a*/ADD_OPCODE(CNTLZD,(OP_REG ra, OP_REG rs, bool rc));
		/*0x03c*/ADD_OPCODE(ANDC,(OP_REG rs, OP_REG ra, OP_REG rb, bool rc));
		/*0x054*/ADD_OPCODE(LDARX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x056*/ADD_OPCODE(DCBF,(OP_REG ra, OP_REG rb));
		/*0x057*/ADD_OPCODE(LBZX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x067*/ADD_OPCODE(LVX,(OP_REG vrd, OP_REG ra, OP_REG rb));
		/*0x068*/ADD_OPCODE(NEG,(OP_REG rt, OP_REG ra, OP_REG oe, bool rc));
		/*0x077*/ADD_OPCODE(LBZUX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x07c*/ADD_OPCODE(NOR,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x08a*/ADD_OPCODE(ADDE,(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x090*/ADD_OPCODE(MTOCRF,(OP_REG fxm, OP_REG rs));
		/*0x096*/ADD_OPCODE(STWCX_,(OP_REG rs, OP_REG ra, OP_REG rb));
		/*0x097*/ADD_OPCODE(STWX,(OP_REG rs, OP_REG ra, OP_REG rb));
		/*0x0ca*/ADD_OPCODE(ADDZE,(OP_REG rs, OP_REG ra, OP_REG oe, bool rc));
		/*0x0e7*/ADD_OPCODE(STVX,(OP_REG vrd, OP_REG ra, OP_REG rb));
		/*0x0ea*/ADD_OPCODE(ADDME,(OP_REG rs, OP_REG ra, OP_REG oe, bool rc));
		/*0x0eb*/ADD_OPCODE(MULLW,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x0f6*/ADD_OPCODE(DCBTST,(OP_REG th, OP_REG ra, OP_REG rb));
		/*0x108*///DOZ
		/*0x10a*/ADD_OPCODE(ADD,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x116*/ADD_OPCODE(DCBT,(OP_REG ra, OP_REG rb, OP_REG th));
		/*0x117*/ADD_OPCODE(LHZX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x11c*/ADD_OPCODE(EQV,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x136*/ADD_OPCODE(ECIWX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x14b*/ADD_OPCODE(DIV,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x137*/ADD_OPCODE(LHZUX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x13c*/ADD_OPCODE(XOR,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x153*/ADD_OPCODE(MFSPR,(OP_REG rd, OP_REG spr));
		/*0x157*/ADD_OPCODE(LHAX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x168*/ADD_OPCODE(ABS,(OP_REG rt, OP_REG ra, OP_REG oe, bool rc));
		/*0x16b*///DIVS
		/*0x173*/ADD_OPCODE(MFTB,(OP_REG rt));
		/*0x177*/ADD_OPCODE(LHAUX,(OP_REG rd, OP_REG ra, OP_REG rb));
		/*0x1b6*/ADD_OPCODE(ECOWX,(OP_REG rs, OP_REG ra, OP_REG rb));
		/*0x1bc*/ADD_OPCODE(OR,(OP_REG ra, OP_REG rs, OP_REG rb));
		/*0x1c9*/ADD_OPCODE(DIVDU,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x1cb*/ADD_OPCODE(DIVWU,(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc));
		/*0x1d3*/ADD_OPCODE(MTSPR,(OP_REG spr, OP_REG rs));
		/*0x1d6*///DCBI
		/*0x1e9*///DIVD
		/*0x1eb*///DIVW
		/*0x217*///LFSX
		/*0x218*/ADD_OPCODE(SRW,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x21b*/ADD_OPCODE(SRD,(OP_REG ra, OP_REG rs, OP_REG rb, bool rc));
		/*0x237*///LFSUX
		/*0x257*///LFDX
		/*0x28a*///LDUX
		/*0x277*///LFDUX
		/*0x316*///LHBRX
		/*0x338*/ADD_OPCODE(SRAWI,(OP_REG ra, OP_REG rs, OP_REG sh, bool rc));
		/*0x33a*/ADD_OPCODE(SRADI1,(OP_REG ra, OP_REG rs, OP_REG sh, bool rc));
		/*0x33b*/ADD_OPCODE(SRADI2,(OP_REG ra, OP_REG rs, OP_REG sh, bool rc));
		/*0x356*/ADD_OPCODE(EIEIO,());
		/*0x39a*/ADD_OPCODE(EXTSH,(OP_REG ra, OP_REG rs, bool rc));
		/*0x3ba*/ADD_OPCODE(EXTSB,(OP_REG ra, OP_REG rs, bool rc));
		/*0x3d7*/ADD_OPCODE(STFIWX,(OP_REG frs, OP_REG ra, OP_REG rb));
		/*0x3da*/ADD_OPCODE(EXTSW,(OP_REG ra, OP_REG rs, bool rc));
		/*0x3d6*///ICBI
		/*0x3f6*///DCBZ
	END_OPCODES_GROUP(G_1f);
	
	ADD_OPCODE(LWZ,(OP_REG rt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LWZU,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(LBZ,(OP_REG rt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LBZU,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STW,(OP_REG rs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(STWU,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STB,(OP_REG rs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(STBU,(OP_REG rs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LHZ,(OP_REG rs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LHZU,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	ADD_OPCODE(STH,(OP_REG rs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LFS,(OP_REG frt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LFSU,(OP_REG frt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LFD,(OP_REG frt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(LFDU,(OP_REG frt, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(STFS,(OP_REG frs, OP_REG ra, OP_sIMM d));
	ADD_OPCODE(STFD,(OP_REG frs, OP_REG ra, OP_sIMM d));

	START_OPCODES_GROUP(G_3a)
		ADD_OPCODE(LD,(OP_REG rt, OP_REG ra, OP_sIMM ds));
		ADD_OPCODE(LDU,(OP_REG rt, OP_REG ra, OP_sIMM ds));
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G_3b)
		ADD_OPCODE(FDIVS,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FADDS,(OP_REG frt, OP_REG fra, OP_REG frb, bool rc));
		ADD_OPCODE(FSQRTS,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FRES,(OP_REG frt, OP_REG frb, bool rc));
		ADD_OPCODE(FMULS,(OP_REG frt, OP_REG fra, OP_REG frc, bool rc));
		ADD_OPCODE(FMADDS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FMSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMSUBS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
		ADD_OPCODE(FNMADDS,(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc));
	END_OPCODES_GROUP(G_3b);
	
	START_OPCODES_GROUP(G_3e)
		ADD_OPCODE(STD,(OP_REG rs, OP_REG ra, OP_sIMM ds));
		ADD_OPCODE(STDU,(OP_REG rs, OP_REG ra, OP_sIMM ds));
	END_OPCODES_GROUP(G_3e);

	START_OPCODES_GROUP(G_3f)
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
	END_OPCODES_GROUP(G_3f);

	ADD_OPCODE(UNK,(const s32 code, const s32 opcode, const s32 gcode));
};

#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP