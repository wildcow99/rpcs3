#pragma once

/*
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      | SPECIAL | ====== |SPECIAL 2| ====== |  ????   | ====== |  ????   | ====== |  VXOR   |
      |   00    | ====== |    01   | ====== |   02    | ====== |   03    | ====== |   04    |
      |_________|        |_________|        |_________|        |_________|        |_________|
        
          |||                |||                |||                |||                |||
          |||                |||                |||                |||                ||| 
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      |  ????   | ====== |  ????   | ====== |  MULLI  | ====== | SUBFIC  | ====== |  ????   |
      |   05    | ====== |   06    | ====== |   07    | ====== |   08    | ====== |   09    |
      |_________|        |_________|        |_________|        |_________|        |_________|
        
          |||                |||                |||                |||                |||
          |||                |||                |||                |||                ||| 
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      | CMPLWI  | ====== | CMPWI   | ====== |  ????   | ====== |  ????   | ====== |   G1    |
      |   0A    | ====== |   0B    | ====== |   0C    | ====== |   0D    | ====== |   0E    |
      |_________|        |_________|        |_________|        |_________|        |_________|
        
          |||                |||                |||                |||                |||
          |||                |||                |||                |||                ||| 
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      |  ADDIS  | ====== |   G2    | ====== |   SC    | ====== |   G3    | ====== |   BLR   |
      |   0F    | ====== |   10    | ====== |   11    | ====== |   12    | ====== |   13    |
      |_________|        |_________|        |_________|        |_________|        |_________|
        
          |||                |||                |||                |||                |||
          |||                |||                |||                |||                ||| 
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      |  ????   | ====== | RLWINM  | ====== |  ????   | ====== |  ????   | ====== |   ORI   |
      |   14    | ====== |   15    | ====== |   16    | ====== |   17    | ====== |   18    |
      |_________|        |_________|        |_________|        |_________|        |_________|

          |||                |||                |||                |||                |||
          |||                |||                |||                |||                ||| 
       _________          _________          _________          _________          _________
      |         |        |         |        |         |        |         |        |         |
      |  ORIS   | ====== |  XORI   | ====== |  XORIS  | ====== |  ????   | ====== |  ????   |
      |   19    | ====== |   1A    | ====== |   1B    | ====== |   1C    | ====== |   1D    |
      |_________|        |_________|        |_________|        |_________|        |_________|

*/

/*
Unknown opcode! - (3508ffff - 0d) - rs: r8, rt: r8, rd: r31, sa: 0x1f : 63, func: 0x3f : 31, imm s16: 0xffffffff : -1, imm u16: 0xffff : 65535, imm s26: 0x108ffff : 17367039
  1c5b88:	35 08 ff ff 	addic.  r8,r8,-1
Unknown opcode! - (354affff - 0d) - rs: r10, rt: r10, rd: r31, sa: 0x1f : 63, func: 0x3f : 31, imm s16: 0xffffffff : -1, imm u16: 0xffff : 65535, imm s26: 0x14affff : 21692415
  26d28c:	35 4a ff ff 	addic.  r10,r10,-1
Unknown opcode! - (356bffff - 0d) - rs: r11, rt: r11, rd: r31, sa: 0x1f : 63, func: 0x3f : 31, imm s16: 0xffffffff : -1, imm u16: 0xffff : 65535, imm s26: 0x16bffff : 23855103
  302fdc:	35 6b ff ff 	addic.  r11,r11,-1
Unknown opcode! - (3465ffff - 0d) - rs: r3, rt: r5, rd: r31, sa: 0x1f : 63, func: 0x3f : 31, imm s16: 0xffffffff : -1, imm u16: 0xffff : 65535, imm s26: 0x65ffff : 6684671
 7d3c04:	34 65 ff ff 	addic.  r3,r5,-1
Unknown opcode! - (34c0ffff - 0d) - rs: r6, rt: r0, rd: r31, sa: 0x1f : 63, func: 0x3f : 31, imm s16: 0xffffffff : -1, imm u16: 0xffff : 65535, imm s26: 0xc0ffff : 12648447
  fd46b4:	34 c0 ff ff 	addic.  r6,r0,-1

Unknown opcode! - (a4a9000c - 29) - rs: r5, rt: r9, rd: r0, sa: 0x0 : 12, func: 0xc : 0, imm s16: 0xc : 12, imm u16: 0xc : 12, imm s26: 0xa9000c : 11075596
  21da2c:	a4 a9 00 0c 	lhzu    r5,12(r9)
Unknown opcode! - (a4c90018 - 29) - rs: r6, rt: r9, rd: r0, sa: 0x0 : 24, func: 0x18 : 0, imm s16: 0x18 : 24, imm u16: 0x18 : 24, imm s26: 0xc90018 : 13172760
  21da3c:	a4 c9 00 18 	lhzu    r6,24(r9)		# 18
Unknown opcode! - (a4e90024 - 29) - rs: r7, rt: r9, rd: r0, sa: 0x0 : 36, func: 0x24 : 0, imm s16: 0x24 : 36, imm u16: 0x24 : 36, imm s26: 0xe90024 : 15269924
  21da4c:	a4 e9 00 24 	lhzu    r7,36(r9)		# 24
Unknown opcode! - (a5090030 - 29) - rs: r8, rt: r9, rd: r0, sa: 0x0 : 48, func: 0x30 : 0, imm s16: 0x30 : 48, imm u16: 0x30 : 48, imm s26: 0x1090030 : 17367088
  21da5c:	a5 09 00 30 	lhzu    r8,48(r9)		# 30
Unknown opcode! - (a569003c - 29) - rs: r11, rt: r9, rd: r0, sa: 0x0 : 60, func: 0x3c : 0, imm s16: 0x3c : 60, imm u16: 0x3c : 60, imm s26: 0x169003c : 23658556
  21da6c:	a5 69 00 3c 	lhzu    r11,60(r9)		# 3c
Unknown opcode! - (a4df000c - 29) - rs: r6, rt: r31, rd: r0, sa: 0x0 : 12, func: 0xc : 0, imm s16: 0xc : 12, imm u16: 0xc : 12, imm s26: 0xdf000c : 14614540
  224a04:	a4 df 00 0c 	lhzu    r6,12(r31)

rotlw:
Unknown opcode! - (5d7ef83e - 17) - rs: r11, rt: r30, rd: r31, sa: 0x0 : 0, func: 0x3e : 62, imm s16: 0x1f : -1986, imm u16: 0xfffff83e : 63550, imm s26: 0xf83e : 25098302
  e9287c:	5d 7e f8 3e 	rotlw   r30,r11,r31
Unknown opcode! - (5c0a203e - 17) - rs: r0, rt: r10, rd: r4, sa: 0x0 : 0, func: 0x3e : 62, imm s16: 0x4 : 8254, imm u16: 0x203e : 8254, imm s26: 0x203e : 663614
  ec72a0:	5c 0a 20 3e 	rotlw   r10,r0,r4

addic:
Unknown opcode! - (3160ffff - 0c) - rs: r11, rt: r0, rd: r31, sa: 0x1f : 31, func: 0x3f : 63, imm s16: 0x1f : -1, imm u16: 0xffffffff : 65535, imm s26: 0xffff : 23134207
  f7ea78:	31 60 ff ff 	addic   r11,r0,-1
Unknown opcode! - (3003ffff - 0c) - rs: r0, rt: r3, rd: r31, sa: 0x1f : 31, func: 0x3f : 63, imm s16: 0x1f : -1, imm u16: 0xffffffff : 65535, imm s26: 0xffff : 262143
  fee008:	30 03 ff ff 	addic   r0,r3,-1
Unknown opcode! - (31600010 - 0c) - rs: r11, rt: r0, rd: r0, sa: 0x0 : 0, func: 0x10 : 16, imm s16: 0x0 : 16, imm u16: 0x10 : 16, imm s26: 0x10 : 23068688
 11ff678:	31 60 00 10 	addic   r11,r0,16
Unknown opcode! - (3138e632 - 0c) - rs: r9, rt: r24, rd: r28, sa: 0x18 : 24, func: 0x32 : 50, imm s16: 0x1c : -6606, imm u16: 0xffffe632 : 58930, imm s26: 0xe632 : 20506162
 1214e2c:	31 38 e6 32 	addic   r9,r24,-6606		# ffffe632
Unknown opcode! - (32267a31 - 0c) - rs: r17, rt: r6, rd: r15, sa: 0x8 : 8, func: 0x31 : 49, imm s16: 0xf : 31281, imm u16: 0x7a31 : 31281, imm s26: 0x7a31 : 36076081
 1214f00:	32 26 7a 31 	addic   r17,r6,31281		# 7a31

 stfdu:
Unknown opcode! - (dfbf0010 - 37) - rs: r29, rt: r31, rd: r0, sa: 0x0 : 0, func: 0x10 : 16, imm s16: 0x0 : 16, imm u16: 0x10 : 16, imm s26: 0x10 : 62849040
 102e410:	df bf 00 10 	stfdu   f29,16(r31)
Unknown opcode! - (ddc81b5a - 37) - rs: r14, rt: r8, rd: r3, sa: 0xd : 13, func: 0x1a : 26, imm s16: 0x3 : 7002, imm u16: 0x1b5a : 7002, imm s26: 0x1b5a : 29891418
 1214f30:	dd c8 1b 5a 	stfdu   f14,7002(r8)		# 1b5a
Unknown opcode! - (dd3b27ac - 37) - rs: r9, rt: r27, rd: r4, sa: 0x1e : 30, func: 0x2c : 44, imm s16: 0x4 : 10156, imm u16: 0x27ac : 10156, imm s26: 0x27ac : 20653996
 1215178:	dd 3b 27 ac 	stfdu   f9,10156(r27)		# 27ac
Unknown opcode! - (df751f04 - 37) - rs: r27, rt: r21, rd: r3, sa: 0x1c : 28, func: 0x4 : 4, imm s16: 0x3 : 7940, imm u16: 0x1f04 : 7940, imm s26: 0x1f04 : 58007300
 1222e24:	df 75 1f 04 	stfdu   f27,7940(r21)		# 1f04
Unknown opcode! - (df983e7a - 37) - rs: r28, rt: r24, rd: r7, sa: 0x19 : 25, func: 0x3a : 58, imm s16: 0x7 : 15994, imm u16: 0x3e7a : 15994, imm s26: 0x3e7a : 60309114
 1223b34:	df 98 3e 7a 	stfdu   f28,15994(r24)		# 3e7a

rlwimi.:
Unknown opcode! - (53797375 - 14) - rs: r27, rt: r25, rd: r14, sa: 0xd : 13, func: 0x35 : 53, imm s16: 0xe : 29557, imm u16: 0x7375 : 29557, imm s26: 0x7375 : 58291061
 1214d00:	53 79 73 75 	rlwimi. r25,r27,14,13,26		# 1a
Unknown opcode! - (52a6b523 - 14) - rs: r21, rt: r6, rd: r22, sa: 0x14 : 20, func: 0x23 : 35, imm s16: 0x16 : -19165, imm u16: 0xffffb523 : 46371, imm s26: 0xb523 : 44479779
 1215094:	52 a6 b5 23 	rlwimi. r6,r21,22,20,17		# 11
Unknown opcode! - (50625291 - 14) - rs: r3, rt: r2, rd: r10, sa: 0xa : 10, func: 0x11 : 17, imm s16: 0xa : 21137, imm u16: 0x5291 : 21137, imm s26: 0x5291 : 6443665
 12229b0:	50 62 52 91 	rlwimi. r2,r3,10,10,8
Unknown opcode! - (5356522f - 14) - rs: r26, rt: r22, rd: r10, sa: 0x8 : 8, func: 0x2f : 47, imm s16: 0xa : 21039, imm u16: 0x522f : 21039, imm s26: 0x522f : 55988783
 12254b4:	53 56 52 2f 	rlwimi. r22,r26,10,8,23		# 17
Unknown opcode! - (524e412f - 14) - rs: r18, rt: r14, rd: r8, sa: 0x4 : 4, func: 0x2f : 47, imm s16: 0x8 : 16687, imm u16: 0x412f : 16687, imm s26: 0x412f : 38682927
 1225654:	52 4e 41 2f 	rlwimi. r14,r18,8,4,23		# 17

 andis.:
Unknown opcode! - (74696c00 - 1d) - rs: r3, rt: r9, rd: r13, sa: 0x10 : 16, func: 0x0 : 0, imm s16: 0xd : 27648, imm u16: 0x6c00 : 27648, imm s26: 0x6c00 : 6908928
 1214d04:	74 69 6c 00 	andis.  r9,r3,27648		# 6c00
Unknown opcode! - (746c0000 - 1d) - rs: r3, rt: r12, rd: r0, sa: 0x0 : 0, func: 0x0 : 0, imm s16: 0x0 : 0, imm u16: 0x0 : 0, imm s26: 0x0 : 7077888
 1214d94:	74 6c 00 00 	andis.  r12,r3,0
Unknown opcode! - (74696c41 - 1d) - rs: r3, rt: r9, rd: r13, sa: 0x11 : 17, func: 0x1 : 1, imm s16: 0xd : 27713, imm u16: 0x6c41 : 27713, imm s26: 0x6c41 : 6908993
 1214da4:	74 69 6c 41 	andis.  r9,r3,27713		# 6c41
Unknown opcode! - (76636f6e - 1d) - rs: r19, rt: r3, rd: r13, sa: 0x1d : 29, func: 0x2e : 46, imm s16: 0xd : 28526, imm u16: 0x6f6e : 28526, imm s26: 0x6f6e : 40071022
 1214da8:	76 63 6f 6e 	andis.  r3,r19,28526		# 6f6e
Unknown opcode! - (7603d3db - 1d) - rs: r16, rt: r3, rd: r26, sa: 0xf : 15, func: 0x1b : 27, imm s16: 0x1a : -11301, imm u16: 0xffffd3db : 54235, imm s26: 0xd3db : 33805275
 1214e74:	76 03 d3 db 	andis.  r3,r16,54235		# d3db

 twi:
Unknown opcode! - (0e2939e5 - 03) - rs: r17, rt: r9, rd: r7, sa: 0x7 : 7, func: 0x25 : 37, imm s16: 0x7 : 14821, imm u16: 0x39e5 : 14821, imm s26: 0x39e5 : 36256229
 1214df4:	0e 29 39 e5 	twi     17,r9,14821		# 39e5
Unknown opcode! - (0d3c22ce - 03) - rs: r9, rt: r28, rd: r4, sa: 0xb : 11, func: 0xe : 14, imm s16: 0x4 : 8910, imm u16: 0x22ce : 8910, imm s26: 0x22ce : 20718286
 12150f8:	0d 3c 22 ce 	twi     9,r28,8910		# 22ce
Unknown opcode! - (0f6e2d8f - 03) - rs: r27, rt: r14, rd: r5, sa: 0x16 : 22, func: 0xf : 15, imm s16: 0x5 : 11663, imm u16: 0x2d8f : 11663, imm s26: 0x2d8f : 57552271
 1223830:	0f 6e 2d 8f 	twi     27,r14,11663		# 2d8f
Unknown opcode! - (0fff1fff - 03) - rs: r31, rt: r31, rd: r3, sa: 0x1f : 31, func: 0x3f : 63, imm s16: 0x3 : 8191, imm u16: 0x1fff : 8191, imm s26: 0x1fff : 67051519
 1223b9c:	0f ff 1f ff 	twi     31,r31,8191		# 1fff
Unknown opcode! - (0c0d0e0f - 03) - rs: r0, rt: r13, rd: r1, sa: 0x18 : 24, func: 0xf : 15, imm s16: 0x1 : 3599, imm u16: 0xe0f : 3599, imm s26: 0xe0f : 855567
 1223ca4:	0c 0d 0e 0f 	twi     0,r13,3599		# e0f

 lmw:
Unknown opcode! - (ba901fe6 - 2e) - rs: r20, rt: r16, rd: r3, sa: 0x1f : 31, func: 0x26 : 38, imm s16: 0x3 : 8166, imm u16: 0x1fe6 : 8166, imm s26: 0x1fe6 : 42999782
 1214e10:	ba 90 1f e6 	lmw     r20,8166(r16)		# 1fe6
Unknown opcode! - (b8474eff - 2e) - rs: r2, rt: r7, rd: r9, sa: 0x1b : 27, func: 0x3f : 63, imm s16: 0x9 : 20223, imm u16: 0x4eff : 20223, imm s26: 0x4eff : 4673279
 1214fb8:	b8 47 4e ff 	lmw     r2,20223(r7)		# 4eff
Unknown opcode! - (b9bc6207 - 2e) - rs: r13, rt: r28, rd: r12, sa: 0x8 : 8, func: 0x7 : 7, imm s16: 0xc : 25095, imm u16: 0x6207 : 25095, imm s26: 0x6207 : 29123079
 1214fbc:	b9 bc 62 07 	lmw     r13,25095(r28)		# 6207
Unknown opcode! - (babf714b - 2e) - rs: r21, rt: r31, rd: r14, sa: 0x5 : 5, func: 0xb : 11, imm s16: 0xe : 29003, imm u16: 0x714b : 29003, imm s26: 0x714b : 46100811
 121503c:	ba bf 71 4b 	lmw     r21,29003(r31)		# 714b
Unknown opcode! - (b9f93bbb - 2e) - rs: r15, rt: r25, rd: r7, sa: 0xe : 14, func: 0x3b : 59, imm s16: 0x7 : 15291, imm u16: 0x3bbb : 15291, imm s26: 0x3bbb : 33110971
 12150b0:	b9 f9 3b bb 	lmw     r15,15291(r25)		# 3bbb
*/

enum MainOpcodes
{
	SPECIAL = 0x00,
	SPECIAL2 = 0x01,
	XOR = 0x04,
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

enum SpecialOpcodes
{
	SYSCODE = 0x0c,
};

class Opcodes
{
public:
	virtual void Exit()=0;
	virtual void Step()=0;
	virtual bool DoSysCall(const int code)=0;

	virtual void NOP()=0;
	//SPECIAL OPCODES:
	virtual void UNK_SPECIAL(const int code, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)=0;
	//
	virtual void SPECIAL2()=0;

	//SPECIAL3???
	//virtual void SPECIAL3()=0;
	virtual void XOR(const int rd, const int rs, const int rt)=0;
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
	virtual void G4()=0;
	//virtual void MFLR(const int rs)=0;
	//
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