#pragma once

#include "Emu/Cell/CPU.h"

#define OP_REG const unsigned int
#define OP_sREG const int
#define START_OPCODES_GROUP(x)
#define ADD_OPCODE(name, regs) virtual void(##name##)##regs##=0
#define ADD_NULL_OPCODE(name) virtual void(##name##)()=0
#define END_OPCODES_GROUP(x) \
	virtual void UNK_##x##( \
		const int code, const int opcode, OP_REG rs, OP_REG rt, OP_REG rd, OP_REG sa, const u32 func,\
		OP_REG crfs, OP_REG crft, OP_REG crm, OP_REG bd, OP_REG lk, OP_REG ms, OP_REG mt, OP_REG mfm,\
		OP_REG ls, OP_REG lt, const s32 imm_m, const s32 imm_s16, const u32 imm_u16, const u32 imm_u26)=0

enum MainOpcodes
{
	SPECIAL = 0x00,
	SPECIAL2 = 0x01,
	//XOR = 0x04,
	G_0x04 = 0x04,
	MULLI = 0x07,
	SUBFIC = 0x08,
	//0x09, //DOZI???
	CMPLDI = 0x0a,
	//CMPDI = 0x0b,
	G_0x0b = 0x0b,
	ADDIC = 0x0c,
	ADDIC_ = 0x0d,
	G1 = 0x0e,
	//ADDIS = 0x0f,
	G_0x0f = 0x0f,
	G2 = 0x10,
	SC = 0x11,
	BRANCH = 0x12,
	//BLR = 0x13,
	G3_S = 0x13,
	//RLWINM = 0x15,
	G3_S0 = 0x15,
	ROTLW = 0x17,
	ORI = 0x18,
	ORIS = 0x19,
	XORI = 0x1a,
	XORIS = 0x1b,
	ANDI_ = 0x1c,
	ANDIS_ = 0x1d,
	G_0x1e = 0x1e,
	//LRLDIÑ = 0x1e,
	G4 = 0x1f,
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
	STHU = 0x2d,
	LMW = 0x2e,
	LFS = 0x30,
	LFSU = 0x31,
	LFD = 0x32,
	LFDU = 0x33,
	STFS = 0x34,
	STFSU = 0x35,
	STFD = 0x36,
	//LD = 0x3a,
	G_0x3a = 0x3a,
	//FDIVS = 0x3b,
	G4_S = 0x3b,
	STD = 0x3e,
	G5 = 0x3f,
};

enum SpecialOpcodes //func
{
	SYSCALL = 0x0c,
};

enum Special2Opcodes //sa??
{
};

enum G_0x04 //sa???
{
	G_0x04_0x0 = 0x0,
	G_0x04_0x1 = 0x1,
	G_0x04_0x2 = 0x2,
	G_0x04_0x3 = 0x3,
	G_0x04_0x4 = 0x4,
	G_0x04_0x5 = 0x5,
	G_0x04_0x6 = 0x6,
	G_0x04_0x7 = 0x7,
	G_0x04_0xc = 0xc,
	G_0x04_0xd = 0xd,
	G_0x04_0xe = 0xe,
	G_0x04_0xf = 0xf,
	G_0x04_0x10 = 0x10,
	G_0x04_0x12 = 0x12,
	G_0x04_0x13 = 0x13,
	G_0x04_0x14 = 0x14,
	G_0x04_0x15 = 0x15,
	G_0x04_0x16 = 0x16,
	G_0x04_0x17 = 0x17,
	G_0x04_0x18 = 0x18,
	G_0x04_0x1c = 0x1c,
	G_0x04_0x1d = 0x1d,
	G_0x04_0x1e = 0x1e,
	G_0x04_0x1f = 0x1f,
};

enum G_0x04_0x0 //func
{
	MULHHWU = 0x10, //sa0x0
	MULHHWU_ = 0x11, //sa0x0
	MACHHWU = 0x18, //sa0x0
	MACHHWU_ = 0x19, //sa0x0
};

enum G_0x04_0x1 //func
{
	MULHHW = 0x10,
	MULHHW_ = 0x11,
	MACHHW = 0x18, //sa0x1
	NMACHHW = 0x1c,
};

enum G_0x04_0x2 //func
{
	MACHHWSU = 0x18, //sa0x2
	MACHHWSU_ = 0x19, //sa0x2
};

enum G_0x04_0x3 //func
{
	MACHHWS = 0x18, //sa0x3
	MACHHWS_ = 0x19, //sa0x3
	NMACHHWS = 0x1c,
};

enum G_0x04_0x4 //func
{
	MULCHWU = 0x10,
	MULCHWU_ = 0x11,
	MACCHWU = 0x18, //sa0x4
	MACCHWU_ = 0x19, //sa0x4
};

enum G_0x04_0x5 //func
{
	MULCHW = 0x10,
	MACCHW = 0x18, //sa0x5
	NMACCHW = 0x20,
};

enum G_0x04_0x6 //func
{
	MACCHWSU = 0x18, //sa0x6
	MACCHWSU_ = 0x19, //sa0x6
};

enum G_0x04_0x7 //func
{
	MACCHWS = 0x18, //sa0x7
	NMACCHWS = 0x1c,
	NMACCHWS_ = 0x1d,
};

enum G_0x04_0xc //func
{
	MULLHWU = 0x10, //sa0xc
	MULLHWU_ = 0x11, //sa0xc
	MACLHWU = 0x18, //sa0xc
	MACLHWU_ = 0x19, //sa0xc
};

enum G_0x04_0xd //func
{
	NMACLHW = 0x1c, //sa0xd,
};

enum G_0x04_0xe //func
{
	MACLHWSU = 0x18, //sa0xe
	MACLHWSU_ = 0x19, //sa0xe
};

enum G_0x04_0xf //func
{
	MACLHWS = 0x1c,
};


enum G_0x04_0x10 //func
{
	MACHHWUO = 0x18, //sa0x10
	MACHHWUO_ = 0x19, //sa0x10
};

enum G_0x04_0x12 //func
{
	MACHHWSUO = 0x18, //sa0x12
	MACHHWSUO_ = 0x19, //sa0x12
};

enum G_0x04_0x13 //func
{
	MACHHWSO = 0x18, //sa0x13
	MACHHWSO_ = 0x19, //sa0x13
	NMACHHWSO = 0x1c,
};

enum G_0x04_0x14 //func
{
	MACCHWUO = 0x18, //sa0x14
	MACCHWUO_ = 0x19, //sa0x14
};

enum G_0x04_0x15 //func
{
	MACCHWO = 0x18, //sa0x15
	MACCHWO_ = 0x19, //sa0x15
	NMACCHWO = 0x1c, 
};

enum G_0x04_0x16 //func
{
	MACCHWSUO = 0x18, //sa0x16
	MACCHWSUO_ = 0x19, //sa0x16
};

enum G_0x04_0x17 //func
{
	NMACCHWSO = 0x1c,
};

enum G_0x04_0x18 //func
{
	MFVSCR = 0x4,
};

enum G_0x04_0x1c //func
{
	MACLHWUO = 0x18, //sa0x1c
	MACLHWUO_ = 0x19, //sa0x1c
};

enum G_0x04_0x1d //func
{
	NMACLHWO = 0x1c,
	NMACLHWO_ = 0x1d,
};

enum G_0x04_0x1e //func
{
	MACLHWSUO = 0x18, //sa0x1e
	MACLHWSUO_ = 0x19, //sa0x1e
};

enum G_0x04_0x1f //func
{
	NMACLHWSO = 0x1c,
	NMACLHWSO_ = 0x1d,
};

enum G_0x0b //???
{
};

enum G1Opcodes //ls
{
	LI = 0x0,
	ADDI = 0x1,
};

enum G_0x0f //lt
{
	ADDIS = 0x0,
	LIS = 0x1,
};

enum G2Opcodes //sa??
{
};

enum BRANCHOpcodes //lk
{
	B = 0x0,
	BL = 0x1,
};

enum G3_SOpcodes //sa
{

	G3_S_0x0 = 0x0,
	G3_S_0x10 = 0x10,
};

enum G3_S_0x0 //func
{
	MCRF = 0x0,
	G3_S_0x0_0x20 = 0x20,
};

enum G3_S_0x0_0x20 //rs
{
	BLELR = 0x4, //rs=4
	BEQLR = 0xc, //rs=12
	BLR = 0x14, //rs=20
};

enum G3_S_0x10 //func
{
	BCTR = 0x20,
	BCTRL = 0x21,
};

enum G3_S0Opcodes //func
{
	RLWINM_ = 0x1b,
    RLWINM = 0x3a,
	G3_S0_0x3e = 0x3e,
	G3_S0_0x3f = 0x3f,
};

enum G3_S0_0x3e //lt
{
    ROTLWI = 0x0,
	CLRLWI = 0x1,
};

enum G3_S0_0x3f //lt
{
	ROTLWI_ = 0x0,
	CLRLWI_ = 0x1,
};

enum G_0x1e //func
{
	CLRLDI = 0x20,
	CLRLDI_ = 0x21,
};

enum G4Opcodes //sa
{
	G4_G0 = 0x0,
	G4_G1 = 0x1,
	G4_G_0x2 = 0x2,
	G4_G2 = 0x3,
	G4_G3 = 0x4,
	STDUX = 0x5,
	G4_G_ADDZE = 0x6,
	G4_G4 = 0x7,
	G4_G_ADD = 0x8,
	G4_G_0x9 = 0x9,
	G4_G_0xa = 0xa,
	G4_G_0xb = 0xb,
	G4_G5 = 0xc,
	//MR = 0xd,
	//OR = 0xd, ???
	G4_G6 = 0xe,
	G4_G_0xf = 0xf,
	G4_G_ADDCO = 0x10,
	G4_G_0x11 = 0x11,
	G4_G_0x12 = 0x12,
	G4_G7 = 0x13,
	G4_G_ADDEO = 0x14,
	G4_G8 = 0x16,
	G4_G9 = 0x17,
	G4_G_ADDO = 0x18,
	SRAWI = 0x19,
	G4_G_0x1a = 0x1a,
	G4_G10 = 0x1b,
	EXTSH = 0x1c,
	EXTSB = 0x1d,
	G4_G_0x1e = 0x1e,
	G4_G_0x1f = 0x1f,
	//LWZX = 0x2e,
};

enum G4_G0Opcodes //func
{
	//CMPW = 0x0,
	//CMPD = 0x0, ???
	//CMPWI = 0x0, ???
	LVSL = 0xc,
	LVEBX = 0xe,
	MULHDU = 0x12,
	MULHDU_ = 0x13,
	ADDC = 0x14,
	MULHWU = 0x16,
	MULHWU_ = 0x17,
	G4_G0_0x26 = 0x26,
	LWARX = 0x28,
	LDX = 0x2a,
	LWZX = 0x2e,
	CNTLZW = 0x34,
	AND = 0x38,
	AND_ = 0x39,
	MASKG = 0x3a,
};

enum G4_G0_0x26 //rt??
{
	MFCR = 0x0,
	MFOCRF = 0x18,
};

enum G4_G1Opcodes //func
{
	//CMPLD = 0x0,
	//CMPLW = 0x0, ???
	LVSR = 0xc,
	LVEHX = 0xe,
	SUBF = 0x10,
	SUBF_ = 0x11,
	LDUX = 0x2a,
	LWZUX = 0x2e,
	CNTLZD = 0x34,
	ANDC = 0x38,
};

enum G4_G_0x2 //func
{
	LVEWX = 0xe,
	MULHD = 0x12,
	MULHD_ = 0x13,
	MULHW = 0x16,
	MULHW_ = 0x17,
	DLMZB = 0x1c,
	DLMZB_ = 0x1d,
	//MTSRD = 0x24,
	LDARX = 0x28,
	DCBF = 0x2c,
	LBZX = 0x2e,
};

enum G4_G2Opcodes //func
{
	LVX = 0xe,
	NEG = 0x10,
	MUL = 0x16,
	MUL_ = 0x17,
	MTSRDIN = 0x24,
	//NOT = 0x38,
	//NOR = 0x38, ???
};

enum G4_G3Opcodes //func
{
	WRTEE = 0x6,
	STVEBX = 0xe,
	SUBFE = 0x10,
	SUBFE_ = 0x11,
	ADDE = 0x14,
	ADDE_ = 0x15,
	//MTOCRF = 0x20,
	//MTCRF = 0x20,
	//MTMSR = 0x24,
	STDX = 0x2a,
	STWCX_ = 0x2d,
	STWX = 0x2e,
	SLQ = 0x30,
	SLQ_ = 0x31,
	SLE_ = 0x33,
};

enum G4_G4Opcodes //func
{
	STVX = 0xe,
	MULLD = 0x12,
	MULLD_ = 0x13,
	ADDME = 0x14,
	ADDME_ = 0x15,
	MULLW = 0x16,
	MULLW_ = 0x17,
	DCBTST = 0x2c,
	SLLIQ = 0x30,
};

enum G4_G_ADD //func
{
	ICBT = 0xc,
	DOZ = 0x10,
	DOZ_ = 0x11,
	ADD = 0x14, //???
	ADD_= 0x15,
	LSCBX = 0x2a,
	LSCBX_ = 0x2b,
	DCBT = 0x2c,
	LHZX = 0x2e,
	EQV = 0x38,
	EQV_ = 0x39,
};

enum G4_G_ADDZE //func
{
	ADDZE = 0x14,
	ADDZE_ = 0x15,
	//MTSR = 0x24,
};

enum G4_G_0x9 //func
{
	DOZI = 0x1,
	LHZUX = 0x2e,
	XOR = 0x38,
};

enum G4_G_0xa //func
{
	MFDCR = 0x6,
	DIV = 0x16,
	DIV_ = 0x17,
	//MFCTR = 0x26,
	//MFLR = 0x26,
	G4_G_0xa_0x26 = 0x26,
	LWAX = 0x2a,
};

enum G4_G_0xa_0x26 //rt
{
	//MFVRSAVE = 0x0,
	MFSPR = 0x0,
	MFLR = 0x8,
	MFCTR = 0x9,
};

enum G4_G_0xb //func
{
	LVXL = 0xe,
	MFTB = 0x26,
};

enum G4_G5Opcodes //func
{
	STHX = 0x2e,
	ORC = 0x38,
	ORC_ = 0x39,
};

enum G4_G6Opcodes //func
{
	MTDCR = 0x6,
	DCCCI = 0xc,
	DIVDU = 0x12,
	DIVDU_ = 0x13,
	DIVWU = 0x16,
	DIVWU_ = 0x17,
	G4_G6_G1 = 0x26,
	DCBI = 0x2c,
	NAND = 0x38,
	NAND_ = 0x39,
};

enum G4_G6_G1Opcodes //rt
{
	//MTSPR = 0x0, //0x3?
	//MTMQ //sa0xe ; func0x26 ; rt0x0
	//MTVRSAVE //sa0xe ; func0x26; 
	MTLR = 0x8,
	MTCTR = 0x9,
};

enum G4_G_0xf //func
{
	DIVD = 0x12,
	DIVD_ = 0x13,
	DIVW = 0x16,
};

enum G4_G_ADDCO //func
{
	MCRXR = 0x0,
	LVLX = 0xe,
	ADDCO = 0x14,
	ADDCO_= 0x15,
	LSWX = 0x2a,
	LWBRX = 0x2c,
	LFSX = 0x2e,
	MASKIR = 0x3a,
};

enum G4_G_0x11 //func
{
	LVRX = 0xe,
	SUBFO = 0x10,
};

enum G4_G_0x12 //func
{
	//LSWI ???
	G4_G_0x12_0x26 = 0x26,
	LWSYNC = 0x2c,
	LFDX = 0x2e,
};

enum G4_G_0x12_0x26 //rt
{
	MFSR = 0x0,
};

enum G4_G7Opcodes //func
{
	NEGO = 0x10,
	MULO = 0x16,
	MULO_ = 0x17,
	//MFSRI = 0x26, //FIXME
	G4_G7_0x26 = 0x26,
};

enum G4_G7_0x26 //rt??
{
	MFSRI = 0x0,
};

enum G4_G_ADDEO //func
{
	ADDEO = 0x14,
	ADDEO_ = 0x15,
	G4_G_ADDEO_0x26 = 0x26,
};

enum G4_G_ADDEO_0x26 //rt??
{
	MFSRIN = 0x0,
};

enum G4_G8Opcodes //func
{
	ADDZEO_ = 0x15, //temp
	STSWI = 0x2a,
	STFDX = 0x2e,
	SRLQ = 0x30,
	SREQ = 0x32,
};

enum G4_G9Opcodes //func
{
	MULLDO = 0x12,
	MULLDO_ = 0x13,
	MULLWO = 0x16,
	MULLWO_ = 0x17,
};

enum G4_G_ADDO //func
{
	LVLXL = 0xe,
	DOZO = 0x10,
	ADDO = 0x14,
	ADDO_ = 0x15, //???
	LHBRX = 0x2c,
};

enum G4_G_0x1a //func
{
	DIVO = 0x16,
	DIVO_ = 0x17,
	EIEIO = 0x2c,
};

enum G4_G10Opcodes //func
{
	ABSO_ = 0x11,
	DIVSO = 0x16,
	DIVSO_ = 0x17,
};

enum G4_G_0x1e //func
{
	ICCCI = 0xc,
	RLDCL_ = 0x11,
	DIVDUO = 0x12,
	DIVDUO_ = 0x13,
	DIVWUO = 0x16,
	DIVWUO_ = 0x17,
	ICBI = 0x2c,
	RLDCL = 0x30,
	EXTSW = 0x34,
	EXTSW_ = 0x35, //???
};

enum G4_G_0x1f //func
{
	ICREAD = 0xc,
	DIVDO = 0x12,
	DCBZ = 0x2c,
	//DIVWO ???
	//DIVWO_ ???
};

enum G_0x3a //func
{
	//LDU = 0x9,
	LWA = 0x26,
	//LD = 0x28,
};

enum G4_SOpcodes //func
{
	FSUBS = 0x28,
	FSUBS_ = 0x29,
	FADDS = 0x2a,
	FSQRTS = 0x2c,
	FRES = 0x30,
	FMULS = 0x32,
	FMULS_ = 0x33,
	FMADDS = 0x3a,
	FMADDS_ = 0x3b,
	FMSUBS = 0x38,
	FMSUBS_ = 0x39,
	FNMSUBS = 0x3c,
	FNMSUBS_ = 0x3d,
	FNMADDS = 0x3e,
	FNMADDS_ = 0x3f,
};

enum G5Opcodes //func
{
	G5_G0x0 = 0x0,
	//MTFSB1 = 0xc, (mtfsb1  4*cr6+lt)
	//MTFSFI = 0xc,
	//MFFS = 0xe,
	//MTFSF = 0xe,
	G5_G0x10 = 0x10,
	G5_G0x1c = 0x1c,
	G5_G0x1e = 0x1e,
	FDIV = 0x24,
	FDIV_ = 0x25,
	FSUB = 0x28,
	FSUB_ = 0x29,
	FADD = 0x2a,
	FADD_ = 0x2b,
	FSQRT = 0x2c,
	FSEL = 0x2e,
	FSEL_ = 0x2f,
	FMUL = 0x32,
	FMUL_ = 0x33,
	FRSQRTE = 0x34,
	FMSUB = 0x38,
	FMSUB_ = 0x39,
	FMADD = 0x3a,
	FNMSUB = 0x3c,
	FNMSUB_ = 0x3d,
	FNMADD = 0x3e,
	FNMADD_ = 0x3f,
};

enum G5_G0x0Opcodes //sa
{
	FCMPU = 0x0,
	FCMPO = 0x1,
	MCRFS = 0x2,
};

enum G5_G0x10Opcodes //sa
{
	FNEG = 0x1,
	FMR = 0x2,
	FNABS = 0x4,
	FABS = 0x8,
};

enum G5_G0x1cOpcodes //sa
{
	FCTIW = 0x0,
	FCTID = 0x19,
	FCFID = 0x1a,
	
};

enum G5_G0x1eOpcodes //sa
{
	FCTIWZ = 0x0,
	FCTIDZ = 0x19,
};

//118

class Opcodes
{
public:
	virtual void Exit()=0;
	virtual void Step()=0;

	static int branchTarget(const int imm_u26)
	{
        return CPU.PC + ext_s26(imm_u26);
    }

	static int branchTarget(const u32 pc, const int imm_u26)
	{
        return pc + ext_s26(imm_u26);
    }

    static int jumpTarget(const int imm_u26)
	{
        return (CPU.PC & 0xf0000000) | ext_s26(imm_u26);
    }

	static int jumpTarget(const u32 pc, const int imm_u26)
	{
        return (pc & 0xf0000000) | ext_s26(imm_u26);
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

	START_OPCODES_GROUP(SPECIAL)
	END_OPCODES_GROUP(SPECIAL);

	START_OPCODES_GROUP(SPECIAL2)
	END_OPCODES_GROUP(SPECIAL2);

	ADD_OPCODE(MULLI,(OP_REG rt, OP_REG rs, OP_sREG imm_s16));
	ADD_OPCODE(SUBFIC,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(CMPLDI,(OP_REG rs, OP_REG rt, OP_REG imm_u16));
	//ADD_OPCODE(CMPDI,(OP_REG rs, OP_REG rt, OP_REG rd));
	ADD_OPCODE(ADDIC,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(ADDIC_,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));

	START_OPCODES_GROUP(G_0x04)
		START_OPCODES_GROUP(G_0x04_0x0)
			ADD_OPCODE(MULHHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x0);
		
		START_OPCODES_GROUP(G_0x04_0x1)
			ADD_OPCODE(MULHHW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHHW_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACHHW,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x1);

		START_OPCODES_GROUP(G_0x04_0x2)
			ADD_OPCODE(MACHHWSU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWSU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x2);

		START_OPCODES_GROUP(G_0x04_0x3)
			ADD_OPCODE(MACHHWS,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWS_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACHHWS,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x3);

		START_OPCODES_GROUP(G_0x04_0x4)
			ADD_OPCODE(MULCHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULCHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x4);

		START_OPCODES_GROUP(G_0x04_0x5)
			ADD_OPCODE(MULCHW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACCHW,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x5);

		START_OPCODES_GROUP(G_0x04_0x6)
			ADD_OPCODE(MACCHWSU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWSU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x6);

		START_OPCODES_GROUP(G_0x04_0x7)
			ADD_OPCODE(MACCHWS,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACCHWS,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACCHWS_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x7);

		START_OPCODES_GROUP(G_0x04_0xc)
			ADD_OPCODE(MULLHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACLHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACLHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0xc);

		START_OPCODES_GROUP(G_0x04_0xd)
			ADD_OPCODE(NMACLHW,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0xd);
		
		START_OPCODES_GROUP(G_0x04_0xe)
			ADD_OPCODE(MACLHWSU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACLHWSU_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0xe);

		START_OPCODES_GROUP(G_0x04_0xf)
			ADD_OPCODE(MACLHWS,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0xf);

		START_OPCODES_GROUP(G_0x04_0x10)
			ADD_OPCODE(MACHHWUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x10);
			
		START_OPCODES_GROUP(G_0x04_0x12)
			ADD_OPCODE(MACHHWSUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWSUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x12);

		START_OPCODES_GROUP(G_0x04_0x13)
			ADD_OPCODE(MACHHWSO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACHHWSO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACHHWSO,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x13);

		START_OPCODES_GROUP(G_0x04_0x14)
			ADD_OPCODE(MACCHWUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x14);

		START_OPCODES_GROUP(G_0x04_0x15)
			ADD_OPCODE(MACCHWO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACCHWO,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x15);

		START_OPCODES_GROUP(G_0x04_0x16)
			ADD_OPCODE(MACCHWSUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACCHWSUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x16);

		START_OPCODES_GROUP(G_0x04_0x17)
			ADD_OPCODE(NMACCHWSO,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x17);

		START_OPCODES_GROUP(G_0x04_0x18)
			ADD_OPCODE(MFVSCR,(OP_REG rs));
		END_OPCODES_GROUP(G_0x04_0x18);

		START_OPCODES_GROUP(G_0x04_0x1c)
			ADD_OPCODE(MACLHWUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACLHWUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x1c);

		START_OPCODES_GROUP(G_0x04_0x1d)
			ADD_OPCODE(NMACLHWO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACLHWO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x1d);

		START_OPCODES_GROUP(G_0x04_0x1e)
			ADD_OPCODE(MACLHWSUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MACLHWSUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x1e);

		START_OPCODES_GROUP(G_0x04_0x1f)
			ADD_OPCODE(NMACLHWSO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NMACLHWSO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G_0x04_0x1f);
	END_OPCODES_GROUP(G_0x04);

	START_OPCODES_GROUP(G_0x0b)
	END_OPCODES_GROUP(G_0x0b);

	START_OPCODES_GROUP(G1)
		ADD_OPCODE(LI,(OP_REG rs, OP_sREG imm_s16));
		ADD_OPCODE(ADDI,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	END_OPCODES_GROUP(G1);

	START_OPCODES_GROUP(G_0x0f)
		ADD_OPCODE(ADDIS,(OP_REG rt, OP_REG rs, OP_sREG imm_s16));
		ADD_OPCODE(LIS,(OP_REG rs, OP_sREG imm_s16));
	END_OPCODES_GROUP(G_0x0f);

	START_OPCODES_GROUP(G2)
	END_OPCODES_GROUP(G2);

	ADD_OPCODE(SC, (const u32 sys));

	START_OPCODES_GROUP(BRANCH)
		ADD_OPCODE(B, (const u32 imm_u26));
		ADD_OPCODE(BL, (const u32 imm_u26));
	END_OPCODES_GROUP(BRANCH);

	START_OPCODES_GROUP(G3_S)
		START_OPCODES_GROUP(G3_S_0x0)
			ADD_OPCODE(MCRF,(OP_REG crfs, OP_REG crft));
			START_OPCODES_GROUP(G3_S_0x0_0x20)
				ADD_OPCODE(BLELR,(OP_REG crft));
				ADD_NULL_OPCODE(BEQLR);
				ADD_NULL_OPCODE(BLR);
			END_OPCODES_GROUP(G3_S_0x0_0x20);
		END_OPCODES_GROUP(G3_S_0x0);
		START_OPCODES_GROUP(G3_S_0x10)
			ADD_NULL_OPCODE(BCTR);
			ADD_NULL_OPCODE(BCTRL);
		END_OPCODES_GROUP(G3_S_0x10);
	END_OPCODES_GROUP(G3_S);

	START_OPCODES_GROUP(G3_S0)
		ADD_NULL_OPCODE(RLWINM_);
		ADD_NULL_OPCODE(RLWINM);
		START_OPCODES_GROUP(G3_S0_0x3e)
			ADD_OPCODE(ROTLWI,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
			ADD_OPCODE(CLRLWI,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
		END_OPCODES_GROUP(G3_S0_0x3e);
		START_OPCODES_GROUP(G3_S0_0x3f)
			ADD_OPCODE(ROTLWI_,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
			ADD_OPCODE(CLRLWI_,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
		END_OPCODES_GROUP(G3_S0_0x3f);
	END_OPCODES_GROUP(G3_S0);

	
	ADD_OPCODE(ROTLW,(OP_REG rt, OP_REG rs, OP_REG rd));
	ADD_OPCODE(ORI,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	ADD_OPCODE(ORIS,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	ADD_OPCODE(XORI,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	ADD_OPCODE(XORIS,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	ADD_OPCODE(ANDI_,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	ADD_OPCODE(ANDIS_,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	//ADD_OPCODE(LRLDIÑ,(OP_REG rt, OP_REG rs, OP_REG imm_u16));

	START_OPCODES_GROUP(G_0x1e)
			ADD_OPCODE(CLRLDI,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
			ADD_OPCODE(CLRLDI_,(OP_REG rt, OP_REG rs, OP_REG imm_u16));
	END_OPCODES_GROUP(G_0x1e);

	START_OPCODES_GROUP(G4)
		START_OPCODES_GROUP(G4_G0)
			//ADD_OPCODE(CMPW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LVSL,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LVEBX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHDU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHDU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDC,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			//ADD_OPCODE(MFCR,(OP_REG rs));

			START_OPCODES_GROUP(G4_G0_0x26)
				ADD_OPCODE(MFCR,(OP_REG rs));
				ADD_OPCODE(MFOCRF,(OP_REG rs, OP_sREG imm_s16));
			END_OPCODES_GROUP(G4_G0_0x26);

			ADD_OPCODE(LWARX,(OP_REG rs, OP_REG rd, OP_sREG imm_s16));
			ADD_OPCODE(LDX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LWZX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(CNTLZW,(OP_REG rt, OP_REG rs));
			ADD_OPCODE(AND,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(AND_,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(MASKG,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G0);

		START_OPCODES_GROUP(G4_G1)
			//ADD_OPCODE(CMPLD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LVSR,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LVEHX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SUBF,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SUBF_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LDUX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LWZUX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(CNTLZD,(OP_REG rs, OP_REG rt));
			ADD_OPCODE(ANDC,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G1);
		
		//ADD_OPCODE(MULHW,(OP_REG rs, OP_REG rt, OP_REG rd));

		START_OPCODES_GROUP(G4_G_0x2)
			ADD_OPCODE(LVEWX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHD_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULHW_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DLMZB,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(DLMZB_,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(LDARX,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(DCBF,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(LBZX,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0x2);

		START_OPCODES_GROUP(G4_G2)
			ADD_OPCODE(LVX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(NEG,(OP_REG rs, OP_REG rt));
			ADD_OPCODE(MUL,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MUL_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MTSRDIN,(OP_REG rs, OP_REG rd));
			//ADD_OPCODE(NOT,(OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G2);

		START_OPCODES_GROUP(G4_G3)
			ADD_OPCODE(WRTEE,(OP_REG rs));
			ADD_OPCODE(STVEBX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SUBFE,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SUBFE_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDE,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDE_,(OP_REG rs, OP_REG rt, OP_REG rd));
			//ADD_OPCODE(MTOCRF,(OP_REG rs));
			ADD_OPCODE(STDX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(STWCX_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(STWX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SLQ,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(SLQ_,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(SLE_,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G3);

		ADD_OPCODE(STDUX,(OP_REG rs, OP_REG rt, OP_REG rd));
		//ADD_OPCODE(ADDZE,(OP_REG rs, OP_REG rt));

		START_OPCODES_GROUP(G4_G4)
			ADD_OPCODE(STVX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLD_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDME,(OP_REG rs, OP_REG rt));
			ADD_OPCODE(ADDME_,(OP_REG rs, OP_REG rt));
			ADD_OPCODE(MULLW,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLW_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DCBTST,(OP_REG rt, OP_REG rd));
			ADD_OPCODE(SLLIQ,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G4);
			
		START_OPCODES_GROUP(G4_G_ADD)
			ADD_OPCODE(ICBT,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DOZ,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DOZ_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADD_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LSCBX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LSCBX_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DCBT,(OP_REG rt, OP_REG rd));
			ADD_OPCODE(LHZX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(EQV,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(EQV_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_ADD);
			
		START_OPCODES_GROUP(G4_G_ADDZE)
			ADD_OPCODE(ADDZE,(OP_REG rs, OP_REG rt));
			ADD_OPCODE(ADDZE_,(OP_REG rs, OP_REG rt));
		END_OPCODES_GROUP(G4_G_ADDZE);
			
		//ADD_OPCODE(XOR,(OP_REG rt, OP_REG rs, OP_REG rd));
		ADD_OPCODE(SRW,(OP_REG rt, OP_REG rs, OP_REG rd));
		//ADD_OPCODE(MFLR,(OP_REG rs));

		START_OPCODES_GROUP(G4_G_0x9)
			ADD_OPCODE(DOZI,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
			ADD_OPCODE(LHZUX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(XOR,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0x9);

		START_OPCODES_GROUP(G4_G_0xa)
			ADD_OPCODE(MFDCR,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIV,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIV_,(OP_REG rs, OP_REG rt, OP_REG rd));
			
			START_OPCODES_GROUP(G4_G_0xa_0x26)
				ADD_OPCODE(MFSPR,(OP_REG rs, OP_sREG imm_s16));
				ADD_OPCODE(MFLR,(OP_REG rs));
				ADD_OPCODE(MFCTR,(OP_REG rs));
			END_OPCODES_GROUP(G4_G_0xa_0x26);
				
			//ADD_OPCODE(MFCTR,(OP_REG rs));
			ADD_OPCODE(LWAX,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0xa);
			
		START_OPCODES_GROUP(G4_G_0xb)
			ADD_OPCODE(LVXL,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MFTB,(OP_REG rs));
		END_OPCODES_GROUP(G4_G_0xb);
			
		START_OPCODES_GROUP(G4_G5)
			ADD_OPCODE(STHX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ORC,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(ORC_,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G5);
			
		//ADD_OPCODE(LVXL,(OP_REG rs, OP_REG rt, OP_REG rd));
		//ADD_OPCODE(MR,(OP_REG rt, OP_REG rs));
			
		START_OPCODES_GROUP(G4_G6)
			ADD_OPCODE(MTDCR,(OP_REG rs));
			ADD_OPCODE(DCCCI,(OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVDU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVDU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVWU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVWU_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DCBI,(OP_REG rt, OP_REG rd));
			ADD_OPCODE(NAND,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(NAND_,(OP_REG rt, OP_REG rs, OP_REG rd));
			
			START_OPCODES_GROUP(G4_G6_G1)
				//ADD_OPCODE(MTSPR,(OP_REG rs));
				ADD_OPCODE(MTLR,(OP_REG rs));
				ADD_OPCODE(MTCTR,(OP_REG rs));
			END_OPCODES_GROUP(G4_G6_G1);
		END_OPCODES_GROUP(G4_G6);
			
		//ADD_OPCODE(DIVW,(OP_REG rs, OP_REG rt, OP_REG rd));
			
		START_OPCODES_GROUP(G4_G_0xf)
			ADD_OPCODE(DIVD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVD_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVW,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0xf);
			
		START_OPCODES_GROUP(G4_G_ADDCO)
			ADD_OPCODE(MCRXR,(OP_REG rs));
			ADD_OPCODE(LVLX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDCO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDCO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LSWX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LWBRX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LFSX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MASKIR,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G_ADDCO);

		//ADD_OPCODE(SUBFO,(OP_REG rs, OP_REG rt, OP_REG rd));
		//ADD_NULL_OPCODE(LWSYNC);

		START_OPCODES_GROUP(G4_G_0x11)
			ADD_OPCODE(LVRX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SUBFO,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0x11);

		START_OPCODES_GROUP(G4_G_0x12)
			START_OPCODES_GROUP(G4_G_0x12_0x26)
				ADD_OPCODE(MFSR,(OP_REG rs, OP_sREG imm_s16));
			END_OPCODES_GROUP(G4_G_0x12_0x26);
				
			ADD_NULL_OPCODE(LWSYNC);
			ADD_OPCODE(LFDX,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0x12);

		START_OPCODES_GROUP(G4_G7)
			ADD_NULL_OPCODE(NEGO);
			ADD_OPCODE(MULO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULO_,(OP_REG rs, OP_REG rt, OP_REG rd));

			START_OPCODES_GROUP(G4_G7_0x26)
				ADD_OPCODE(MFSRI,(OP_REG rs, OP_REG rt, OP_REG rd));
			END_OPCODES_GROUP(G4_G7_0x26);
		END_OPCODES_GROUP(G4_G7);

		//ADD_OPCODE(STFSX,(OP_REG rs, OP_REG rt, OP_REG rd));
		
		START_OPCODES_GROUP(G4_G_ADDEO)
			ADD_OPCODE(ADDEO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDEO_,(OP_REG rs, OP_REG rt, OP_REG rd));

			START_OPCODES_GROUP(G4_G_ADDEO_0x26)
				ADD_OPCODE(MFSRIN,(OP_REG rs, OP_REG rd));
			END_OPCODES_GROUP(G4_G_ADDEO_0x26);
		END_OPCODES_GROUP(G4_G_ADDEO);
		
		START_OPCODES_GROUP(G4_G8)
			ADD_OPCODE(ADDZEO_,(OP_REG rs, OP_REG rt, OP_REG rd)); //temp
			ADD_OPCODE(STSWI,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(STFDX,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(SRLQ,(OP_REG rt, OP_REG rs, OP_REG rd));
			ADD_OPCODE(SREQ,(OP_REG rt, OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G4_G8);

		START_OPCODES_GROUP(G4_G_ADDO)
			ADD_OPCODE(LVLXL,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DOZO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ADDO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(LHBRX,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_ADDO);

		START_OPCODES_GROUP(G4_G9)
			ADD_OPCODE(MULLDO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLDO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLWO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MULLWO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G9);

		//ADD_OPCODE(SRAW,(OP_REG rt, OP_REG rs, OP_REG rd));
		ADD_OPCODE(SRAWI,(OP_REG rt, OP_REG rs, OP_REG rd));
		//ADD_NULL_OPCODE(EIEIO);

		START_OPCODES_GROUP(G4_G_0x1a)
			ADD_OPCODE(DIVO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_NULL_OPCODE(EIEIO);
		END_OPCODES_GROUP(G4_G_0x1a);

		START_OPCODES_GROUP(G4_G10)
			ADD_NULL_OPCODE(ABSO_);
			ADD_OPCODE(DIVSO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVSO_,(OP_REG rs, OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G10);
			
		START_OPCODES_GROUP(G4_G_0x1e)
			ADD_OPCODE(ICCCI,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(RLDCL_,(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16));
			ADD_OPCODE(DIVDUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVDUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVWUO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVWUO_,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(ICBI,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(RLDCL,(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16));
			ADD_OPCODE(EXTSW,(OP_REG rt, OP_REG rs));
		    ADD_OPCODE(EXTSW_,(OP_REG rt, OP_REG rs));
		END_OPCODES_GROUP(G4_G_0x1e);

		START_OPCODES_GROUP(G4_G_0x1f)
			ADD_OPCODE(ICREAD,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DIVDO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(DCBZ,(OP_REG rt, OP_REG rd));
		END_OPCODES_GROUP(G4_G_0x1f);

		ADD_OPCODE(EXTSH,(OP_REG rt, OP_REG rs));
		ADD_OPCODE(EXTSB,(OP_REG rt, OP_REG rs));
		//ADD_OPCODE(EXTSW,(OP_REG rt, OP_REG rs));
		//ADD_OPCODE(DCBZ,(OP_REG rt, OP_REG rd));
		//ADD_OPCODE(LWZX,(OP_REG rs, OP_REG rt, OP_REG rd));
	END_OPCODES_GROUP(G4);

	ADD_OPCODE(LWZ,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LWZU,(OP_REG rt, OP_REG rs, OP_sREG imm_s16));
	ADD_OPCODE(LBZ,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LBZU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STW,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STWU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STB,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STBU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LHZ,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LHZU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LHA,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LHAU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STH,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STHU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LMW,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LFS,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LFSU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LFD,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(LFDU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STFS,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STFSU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	ADD_OPCODE(STFD,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	//ADD_OPCODE(LD,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	
	START_OPCODES_GROUP(G_0x3a)
		//ADD_OPCODE(LDU,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
		//ADD_OPCODE(LWA,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
		//ADD_OPCODE(LD,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));
	END_OPCODES_GROUP(G_0x3a);

	START_OPCODES_GROUP(G4_S)
		ADD_OPCODE(FSUBS,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FSUBS_,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FADDS,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FSQRTS,(OP_REG rs, OP_REG rd));
		ADD_OPCODE(FRES,(OP_REG rs, OP_REG rd));
		ADD_OPCODE(FMULS,(OP_REG rs, OP_REG rt, OP_REG sa));
		ADD_OPCODE(FMULS_,(OP_REG rs, OP_REG rt, OP_REG sa));
		ADD_OPCODE(FMADDS,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMADDS_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMSUBS,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMSUBS_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMSUBS,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMSUBS_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMADDS,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMADDS_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
	END_OPCODES_GROUP(G4_S);

	ADD_OPCODE(STD,(OP_REG rs, OP_REG rt, OP_sREG imm_s16));

	START_OPCODES_GROUP(G5)
		START_OPCODES_GROUP(G5_G0x0)
			ADD_OPCODE(FCMPU,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(FCMPO,(OP_REG rs, OP_REG rt, OP_REG rd));
			ADD_OPCODE(MCRFS,(OP_REG rs, OP_REG rt));
		END_OPCODES_GROUP(G5_G0x0);

		START_OPCODES_GROUP(G5_G0x10)
			ADD_OPCODE(FNEG,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FMR,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FNABS,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FABS,(OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G5_G0x10);

		START_OPCODES_GROUP(G5_G0x1c)
			ADD_OPCODE(FCTIW,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FCTID,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FCFID,(OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G5_G0x1c);
		
		START_OPCODES_GROUP(G5_G0x1e)
			ADD_OPCODE(FCTIWZ,(OP_REG rs, OP_REG rd));
			ADD_OPCODE(FCTIDZ,(OP_REG rs, OP_REG rd));
		END_OPCODES_GROUP(G5_G0x1e);

		//ADD_OPCODE(MFFS,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FDIV,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FDIV_,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FSUB,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FSUB_,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FADD,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FADD_,(OP_REG rs, OP_REG rt, OP_REG rd));
		ADD_OPCODE(FSQRT,(OP_REG rs, OP_REG rd));
		ADD_OPCODE(FSEL,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FSEL_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMUL,(OP_REG rs, OP_REG rt, OP_REG sa));
		ADD_OPCODE(FMUL_,(OP_REG rs, OP_REG rt, OP_REG sa));
		ADD_OPCODE(FRSQRTE,(OP_REG rs, OP_REG rd));
		ADD_OPCODE(FMSUB,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMSUB_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FMADD,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMSUB,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMSUB_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMADD,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
		ADD_OPCODE(FNMADD_,(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd));
	END_OPCODES_GROUP(G5);

	ADD_OPCODE(UNK,(OP_REG code, OP_REG opcode, OP_REG rs, OP_REG rt,
		OP_REG rd, OP_REG sa, OP_REG func, OP_sREG imm_s16, OP_REG imm_u16, OP_REG imm_u26));
};

#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP