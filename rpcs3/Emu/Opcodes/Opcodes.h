#pragma once

enum MainOpcodes
{
	SPECIAL = 0x00,
	SPECIAL2 = 0x01,
	//XOR = 0x04,
	MULLI = 0x07,
	SUBFIC = 0x08,
	CMPLWI = 0x0a,
	CMPWI = 0x0b,
	ADDIC = 0x0c,
	ADDIC_ = 0x0d,
	G1 = 0x0e,
	ADDIS = 0x0f,
	G2 = 0x10,
	SC = 0x11,
	G3 = 0x12,
	BLR = 0x13,
	RLWINM = 0x15,
	ROTLW = 0x17,
	ORI = 0x18,
	ORIS = 0x19,
	XORI = 0x1a,
	XORIS = 0x1b,
	CLRLDI = 0x1e,
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
	STH = 0x2c,
	STHU = 0x2d,
	LFS = 0x30,
	LFSU = 0x31,
	LFD = 0x32,
	STFS = 0x34,
	STFSU = 0x35,
	STFD = 0x36,
	LD = 0x3a,
	FDIVS = 0x3b,
	STD = 0x3e,
	G5 = 0x3f,
};

enum SpecialOpcodes //func
{
	SYSCALL = 0x0c,
};

enum G4Opcodes //sa
{
	CMPW = 0x0,
	G4_G1 = 0x1,
	MULHW = 0x2,
	G4_G2 = 0x3,
	G4_G3 = 0x4,
	STDUX = 0x5,
	ADDZE = 0x6,
	G4_G4 = 0x7,
	ADD = 0x8,
	XOR = 0x9,
	SRW = 0x10,
	MFLR = 0xa,
	G4_G5 = 0xc,
	LVXL = 0xb,
	MR = 0xd,
	G4_G6 = 0xe,
	DIVW = 0xf,
	SUBFO = 0x11,
	LWSYNC = 0x12, //FIXME
	G4_G7 = 0x13,
	STFSX = 0x14,
	G4_G8 = 0x16,
	G4_G9 = 0x17,
	SRAW = 0x18,
	SRAWI = 0x19,
	EIEIO = 0x1a, //FIXME
	G4_G10 = 0x1b,
	EXTSH = 0x1c,
	EXTSB = 0x1d,
	EXTSW = 0x1e,
	DCBZ = 0x1f,
	LWZX = 0x2e,
};

enum G4_G1Opcodes //func
{
	CMPLD = 0x0,
	LVSR = 0xc,
	LVEHX = 0xe,
	SUBF = 0x10,
	SUBF_ = 0x11,
	LDUX = 0x2a,
	LWZUX = 0x2e,
	CNTLZD = 0x34,
	ANDC = 0x38,
};

enum G4_G2Opcodes //func
{
	LVX = 0xe,
	NEG = 0x10,
	MTSRDIN = 0x24,
	NOT = 0x38,
};

enum G4_G3Opcodes //func
{
	WRTEE = 0x6,
	STVEBX = 0xe,
	SUBFE = 0x10,
	SUBFE_ = 0x11,
	ADDE = 0x14,
	ADDE_ = 0x15,
	MTOCRF = 0x20,
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
	ADDME_ = 0x15,
	MULLW = 0x16,
	MULLW_ = 0x17,
	DCBTST = 0x2c,
	SLLIQ = 0x30,
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
	G4_G6_G1 = 0x26,
	DCBI = 0x2c,
	NAND = 0x38,
	NAND_ = 0x39,
};

enum G4_G6_G1Opcodes //rt
{
	MTSPR = 0x0, //0x3?
	MTLR = 0x8,
	MTCTR = 0x9,
};

enum G4_G7Opcodes //func
{
	NEGO = 0x10,
	MULO = 0x16,
	MULO_ = 0x17,
	MFSRI = 0x26, //FIXME
};

enum G4_G8Opcodes //func
{
	STSWI = 0x2a,
	STFDX = 0x2e,
	SRLQ = 0x30,
	SREQ = 0x32,
};

enum G4_G9Opcodes //func
{
	MULLDO = 0x12,
	MULLWO = 0x16,
	MULLWO_ = 0x17,
};

enum G4_G10Opcodes //func
{
	ABSO_ = 0x11,
	DIVSO_ = 0x17,
};

class Opcodes
{
public:
	virtual void Exit()=0;
	virtual void Step()=0;
	virtual bool DoSysCall(const int code)=0;

	virtual void NOP()=0;
		//SPECIAL
		virtual void UNK_SPECIAL(const int code, const int rs, const int rt,
			const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
		//
		//SPECIAL 2
		virtual void SPECIAL2()=0;
		//
		//SPECIAL3???
		//virtual void SPECIAL3()=0;
		//virtual void XOR(const int rd, const int rs, const int rt)=0;
		//
	virtual void MULLI(const int rt, const int rs, const int imm_s16)=0;
	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)=0;
	virtual void CMPLWI(const int rs, const int rt, const int imm_u16)=0;
	virtual void CMPWI(const int rs, const int rt, const int rd)=0;
	virtual void ADDIC(const int rs, const int rt, const int imm_s16)=0;
	virtual void ADDIC_(const int rs, const int rt, const int imm_s16)=0;
		//g1 - 0e
		virtual void G1()=0;
		//
		//virtual void ADDI(const int rt, const int rs, const int imm_u16)=0;
		//
	virtual void ADDIS(const int rt, const int rs, const int imm_s16)=0;
		//g2 - 10
		virtual void G2()=0;
		//
	virtual void SC()=0;
		//g3 - 12
		virtual void G3()=0;
		//
	virtual void BLR()=0;
	virtual void RLWINM()=0;
	virtual void ROTLW(const int rt, const int rs, const int rd)=0;
	virtual void ORI(const int rt, const int rs, const int imm_u16)=0;
	virtual void ORIS(const int rt, const int rs, const int imm_u16)=0;
	virtual void XORI(const int rt, const int rs, const int imm_u16)=0;
	virtual void XORIS(const int rt, const int rs, const int imm_u16)=0;
	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)=0;
		//g4 - 1f
		virtual void CMPW(const int rs, const int rt, const int rd)=0;
			//g4_g1
			virtual void CMPLD(const int rs, const int rt, const int rd)=0;
			virtual void LVSR(const int rs, const int rt, const int rd)=0;
			virtual void LVEHX(const int rs, const int rt, const int rd)=0;
			virtual void SUBF(const int rs, const int rt, const int rd)=0;
			virtual void SUBF_(const int rs, const int rt, const int rd)=0;
			virtual void LDUX(const int rs, const int rt, const int rd)=0;
			virtual void LWZUX(const int rs, const int rt, const int rd)=0;
			virtual void CNTLZD(const int rs, const int rt)=0;
			virtual void ANDC(const int rt, const int rs, const int rd)=0;
			virtual void UNK_G4_G1(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g1
		virtual void MULHW(const int rs, const int rt, const int rd)=0;
			//g4_g2
		    virtual void LVX(const int rs, const int rt, const int rd)=0;
			virtual void NEG(const int rs, const int rt)=0;
			virtual void MTSRDIN(const int rs, const int rd)=0;
			virtual void NOT(const int rt, const int rd)=0;
			virtual void UNK_G4_G2(const int code, const int opcode, const int rs, const int rt,
					const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g2
			//g4_g3
			virtual void WRTEE(const int rs)=0;
			virtual void STVEBX(const int rs, const int rt, const int rd)=0;
			virtual void SUBFE(const int rs, const int rt, const int rd)=0;
			virtual void SUBFE_(const int rs, const int rt, const int rd)=0;
			virtual void ADDE(const int rs, const int rt, const int rd)=0;
			virtual void ADDE_(const int rs, const int rt, const int rd)=0;
			virtual void MTOCRF(const int rs)=0;
			virtual void STDX(const int rs, const int rt, const int rd)=0;
			virtual void STWCX_(const int rs, const int rt, const int rd)=0;
			virtual void STWX(const int rs, const int rt, const int rd)=0;
			virtual void SLQ(const int rt, const int rs, const int rd)=0;
			virtual void SLQ_(const int rt, const int rs, const int rd)=0;
			virtual void SLE_(const int rt, const int rs, const int rd)=0;
			virtual void UNK_G4_G3(const int code, const int opcode, const int rs, const int rt,
					const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g3
		virtual void STDUX(const int rs, const int rt, const int rd)=0;
		virtual void ADDZE(const int rs, const int rt)=0;
			//g4_g4
		    virtual void STVX(const int rs, const int rt, const int rd)=0;
			virtual void MULLD(const int rs, const int rt, const int rd)=0;
			virtual void ADDME_(const int rs, const int rt)=0;
			virtual void MULLW(const int rs, const int rt, const int rd)=0;
			virtual void MULLW_(const int rs, const int rt, const int rd)=0;
			virtual void DCBTST(const int rt, const int rd)=0;
			virtual void SLLIQ(const int rt, const int rs, const int rd)=0;
			virtual void UNK_G4_G4(const int code, const int opcode, const int rs, const int rt,
					const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g4
		virtual void ADD(const int rs, const int rt, const int rd)=0;
		virtual void XOR(const int rt, const int rs, const int rd)=0;
		virtual void SRW(const int rt, const int rs, const int rd)=0;
		virtual void MFLR(const int rs)=0;
			//g4_g5
			virtual void STHX(const int rs, const int rt, const int rd)=0;
			virtual void ORC(const int rt, const int rs, const int rd)=0;
			virtual void ORC_(const int rt, const int rs, const int rd)=0;
			virtual void UNK_G4_G5(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g5
		virtual void LVXL(const int rs, const int rt, const int rd)=0;
		virtual void MR(const int rt, const int rs)=0;
			//g4_g6
			virtual void MTDCR(const int rs)=0;
			virtual void DCCCI(const int rt, const int rd)=0;
			virtual void DIVDU(const int rs, const int rt, const int rd)=0;
			virtual void DIVDU_(const int rs, const int rt, const int rd)=0;
			virtual void DIVWU(const int rs, const int rt, const int rd)=0;
			virtual void DCBI(const int rt, const int rd)=0;
			virtual void NAND(const int rt, const int rs, const int rd)=0;
			virtual void NAND_(const int rt, const int rs, const int rd)=0;
				//g4_g6_g1
			    virtual void MTSPR(const int rs)=0;
				virtual void MTLR(const int rs)=0;
				virtual void MTCTR(const int rs)=0;
				virtual void UNK_G4_G6_G1(const int code, const int opcode, const int rs, const int rt,
					const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
				//end g4_g6_g1
			
			virtual void UNK_G4_G6(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g6
		virtual void DIVW(const int rs, const int rt, const int rd)=0;
		virtual void SUBFO(const int rs, const int rt, const int rd)=0;
		virtual void LWSYNC ()=0;
			//g4_g7
		    virtual void NEGO()=0;
			virtual void MULO(const int rs, const int rt, const int rd)=0;
			virtual void MULO_(const int rs, const int rt, const int rd)=0;
			virtual void MFSRI()=0;
			virtual void UNK_G4_G7(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g7
		virtual void STFSX(const int rs, const int rt, const int rd)=0;
			//g4_g8
			virtual void STSWI(const int rs, const int rt, const int rd)=0;
			virtual void STFDX(const int rs, const int rt, const int rd)=0;
			virtual void SRLQ(const int rt, const int rs, const int rd)=0;
			virtual void SREQ(const int rt, const int rs, const int rd)=0;
			virtual void UNK_G4_G8(const int code, const int opcode, const int rs, const int rt,
					const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g8
			//g4_g9
			virtual void MULLDO(const int rs, const int rt, const int rd)=0;
			virtual void MULLWO(const int rs, const int rt, const int rd)=0;
			virtual void MULLWO_(const int rs, const int rt, const int rd)=0;
			virtual void UNK_G4_G9(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g9
		
		virtual void SRAW(const int rt, const int rs, const int rd)=0;
		virtual void SRAWI(const int rt, const int rs, const int rd)=0;
		virtual void EIEIO()=0;
			//g4_g10
			virtual void ABSO_()=0;
			virtual void DIVSO_(const int rs, const int rt, const int rd)=0;
			virtual void UNK_G4_G10(const int code, const int opcode, const int rs, const int rt,
				const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
			//end g4_g10
		virtual void EXTSH(const int rt, const int rs)=0;
		virtual void EXTSB(const int rt, const int rs)=0;
		virtual void EXTSW(const int rt, const int rs)=0;
		virtual void DCBZ(const int rt, const int rd)=0;
		virtual void LWZX(const int rs, const int rt, const int rd)=0;
		
		virtual void UNK_G4(const int code, const int opcode, const int rs, const int rt,
			const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
		//end g4
	virtual void LWZ(const int rs, const int rt, const int imm_s16)=0;
	virtual void LWZU(const int rt, const int rs, const int imm_s16)=0;
	virtual void LBZ(const int rs, const int rt, const int imm_s16)=0;
	virtual void LBZU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STW(const int rs, const int rt, const int imm_s16)=0;
	virtual void STWU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STB(const int rs, const int rt, const int imm_s16)=0;
	virtual void STBU(const int rs, const int rt, const int imm_u16)=0;
	virtual void LHZ(const int rs, const int rt, const int imm_s16)=0;
	virtual void LHZU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STH(const int rs, const int rt, const int imm_s16)=0;
	virtual void STHU(const int rs, const int rt, const int imm_u16)=0;
	virtual void LFS(const int rs, const int rt, const int imm_s16)=0;
	virtual void LFSU(const int rs, const int rt, const int imm_u16)=0;
	virtual void LFD(const int rs, const int rt, const int imm_s16)=0;
	virtual void STFS(const int rs, const int rt, const int imm_s16)=0;
	virtual void STFSU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STFD(const int rs, const int rt, const int imm_s16)=0;
	virtual void LD(const int rs, const int rt, const int imm_s16)=0;
	virtual void FDIVS(const int rs, const int rt, const int rd)=0;
	virtual void STD(const int rs, const int rt, const int imm_s16)=0;
		//g5 - 3f
		virtual void G5()=0;
		//virtual void FCFID(const int rs, const int rd)=0;
		//
	
	virtual void UNK(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
};