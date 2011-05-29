#pragma once

enum MainOpcodes
{
	SPECIAL = 0x00,
	SPECIAL2 = 0x01,
	VXOR = 0x04,
	MULLI = 0x07,
	SUBFIC = 0x08,
	CMPLWI = 0x0a,
	CMPWI = 0x0b,
	G1 = 0x0e,
	ADDIS = 0x0f,
	G2 = 0x10,
	SC = 0x11,
	G3 = 0x12,
	BLR = 0x13,
	RLWINM = 0x15,
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

class Opcodes
{
public:
	virtual void SPECIAL()=0;
	virtual void SPECIAL2()=0;
	virtual void VXOR(const int rs, const int rt, const int rd)=0;
	virtual void MULLI(const int rs, const int rt, const int imm_s16)=0;
	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)=0;
	virtual void CMPLWI(const int rs, const int rt, const int imm_u16)=0;
	virtual void CMPWI(const int rs, const int rt, const int imm_u16)=0;
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
	virtual void ORI(const int rt, const int rs, const int imm_u16)=0;
	virtual void ORIS(const int rt, const int rs, const int imm_s16)=0;
	virtual void XORI(const int rt, const int rs, const int imm_u16)=0;
	virtual void XORIS(const int rt, const int rs, const int imm_s16)=0;
	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)=0;
	//g4 - 1f
	virtual void G4()=0;
	//virtual void MFLR(const int rs)=0;
	//
	virtual void LWZ(const int rs, const int rt, const int imm_s16)=0;
	virtual void LWZU(const int rs, const int rt, const int imm_u16)=0;
	virtual void LBZ(const int rs, const int rt, const int imm_s16)=0;
	virtual void LBZU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STW(const int rs, const int rt, const int imm_s16)=0;
	virtual void STWU(const int rs, const int rt, const int imm_u16)=0;
	virtual void STB(const int rs, const int rt, const int imm_s16)=0;
	virtual void STBU(const int rs, const int rt, const int imm_u16)=0;
	virtual void LHZ(const int rs, const int rt, const int imm_s16)=0;
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
	//g5 - 1f
	virtual void G5()=0;
	//virtual void FCFID(const int rs, const int rd)=0;
	//
	
	virtual void UNK(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
};