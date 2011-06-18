#pragma once

#include "Opcodes/Opcodes.h"

/*
G4:
---
7c 08 03 a6: Unknown G4 opcode! - (7c0803a6 - 1f) - rs: r0, rt: r8, rd: r0, sa: 0xe : 14, func: 0x26 : 38, imm s16: 0x3a6 : 934, imm u16: 0x3a6 : 934, imm u26: 0x803a6 : 525222
7c 08 03 a6 	mtlr    r0

mtlr rs (or rd?)
---

---
7f 83 07 b4: Unknown G4 opcode! - (7f8307b4 - 1f) - rs: r28, rt: r3, rd: r0, sa: 0x1e : 30, func: 0x34 : 52, imm s16: 0x7b4 : 1972, imm u16: 0x7b4 : 1972, imm u26: 0x38307b4 : 58918836
4d0:	7f 83 07 b4 	extsw   r3,r28

extsw rt, rs
---

---
7c 7d 1b 78: Unknown G4 opcode! - (7c7d1b78 - 1f) - rs: r3, rt: r29, rd: r3, sa: 0xd : 13, func: 0x38 : 56, imm s16: 0x1b78 : 7032, imm u16: 0x1b78 : 7032, imm u26: 0x7d1b78 : 8199032
7c 7d 1b 78 	mr      r29,r3

mr rt, rs (or rd?)
---

---
7c 09 03 a6: Unknown G4 opcode! - (7c0903a6 - 1f) - rs: r0, rt: r9, rd: r0, sa: 0xe : 14, func: 0x26 : 38, imm s16: 0x3a6 : 934, imm u16: 0x3a6 : 934, imm u26: 0x903a6 : 590758
7c 09 03 a6 	mtctr   r0

mtctr rs (or rd?)
---

---
7f 98 d8 00: Unknown G4 opcode! - (7f98d800 - 1f) - rs: r28, rt: r24, rd: r27, sa: 0x0 : 0, func: 0x0 : 0, imm s16: 0xffffd800 : -10240, imm u16: 0xd800 : 55296, imm u26: 0x398d800 : 60348416
7f 98 d8 00 	cmpw    cr7,r24,r27

cmpw rs/4, rt, rd
---

---
7d 3b f2 14: Unknown G4 opcode! - (7d3bf214 - 1f) - rs: r9, rt: r27, rd: r30, sa: 0x8 : 8, func: 0x14 : 20, imm s16: 0xfffff214 : -3564, imm u16: 0xf214 : 61972, imm u26: 0x13bf214 : 20705812
7d 3b f2 14 	add     r9,r27,r30

add rs, rt, rd
---

---
7d 64 02 78: Unknown G4 opcode! - (7d640278 - 1f) - rs: r11, rt: r4, rd: r0, sa: 0x9 : 9, func: 0x38 : 56, imm s16: 0x278 : 632, imm u16: 0x278 : 632, imm u26: 0x1640278 : 23331448
7d 64 02 78 	xor     r4,r11,r0

xor rt, rs, rd
---

---
7c 0b fe 70: Unknown G4 opcode! - (7c0bfe70 - 1f) - rs: r0, rt: r11, rd: r31, sa: 0x19 : 25, func: 0x30 : 48, imm s16: 0xfffffe70 : -400, imm u16: 0xfe70 : 65136, imm u26: 0xbfe70 : 786032
7c 0b fe 70 	srawi   r11,r0,31		# 1f

srawi rt, rs, rd (or opcode?)
---

---
7f bf 40 40: Unknown G4 opcode! - (7fbf4040 - 1f) - rs: r29, rt: r31, rd: r8, sa: 0x1 : 1, func: 0x0 : 0, imm s16: 0x4040 : 16448, imm u16: 0x4040 : 16448, imm u26: 0x3bf4040 : 62865472
7f bf 40 40 	cmpld   cr7,r31,r8

cmpld rs/4, rt, rd
---

---
7c 8b 20 50: Unknown G4 opcode! - (7c8b2050 - 1f) - rs: r4, rt: r11, rd: r4, sa: 0x1 : 1, func: 0x10 : 16, imm s16: 0x2050 : 8272, imm u16: 0x2050 : 8272, imm u26: 0x8b2050 : 9117776
7c 8b 20 50 	subf    r4,r11,r4

subf rs, rt, rd (or rd, rt, rs?)
---

---
7c 89 00 2e: Unknown G4 opcode! - (7c89002e - 1f) - rs: r4, rt: r9, rd: r0, sa: 0x0 : 0, func: 0x2e : 46, imm s16: 0x2e : 46, imm u16: 0x2e : 46, imm u26: 0x89002e : 8978478
7c 89 00 2e 	lwzx    r4,r9,r0

lwzx rs, rt, rd
---
*/

/*
7f a8 02 a6: Unknown G4 opcode! - (7fa802a6 - 1f) - rs: r29, rt: r8, rd: r0, sa: 0xa : 10, func: 0x26 : 38, imm s16: 0x2a6 : 678, imm u16: 0x2a6 : 678, imm u26: 0x3a802a6 : 61342374
7f a8 02 a6 	mflr    r29
0xa - mflr rs

7c 60 e9 d6: Unknown G4 opcode! - (7c60e9d6 - 1f) - rs: r3, rt: r0, rd: r29, sa: 0x7 : 7, func: 0x16 : 22, imm s16: 0xffffe9d6 : -5674, imm u16: 0xe9d6 : 59862, imm u26: 0x60e9d6 : 6351318
7c 60 e9 d6 	mullw   r3,r0,r29
0x7 - mullw rs,rt,rd

7d 90 81 20: Unknown G4 opcode! - (7d908120 - 1f) - rs: r12, rt: r16, rd: r16, sa: 0x4 : 4, func: 0x20 : 32, imm s16: 0xffff8120 : -32480, imm u16: 0x8120 : 33056, imm u26: 0x1908120 : 26247456
7d 90 81 20 	mtocrf  8,r12
7d 91 01 20: Unknown G4 opcode! - (7d910120 - 1f) - rs: r12, rt: r17, rd: r0, sa: 0x4 : 4, func: 0x20 : 32, imm s16: 0x120 : 288, imm u16: 0x120 : 288, imm u26: 0x1910120 : 26280224
7d 91 01 20 	mtocrf  16,r12

7f e1 49 ce: Unknown G4 opcode! - (7fe149ce - 1f) - rs: r31, rt: r1, rd: r9, sa: 0x7 : 7, func: 0xe : 14, imm s16: 0x49ce : 18894, imm u16: 0x49ce : 18894, imm u26: 0x3e149ce : 65096142
7f e1 49 ce 	stvx    v31,r1,r9
0x7 - stvx rs,rt,rd

7f e1 00 ce: Unknown G4 opcode! - (7fe100ce - 1f) - rs: r31, rt: r1, rd: r0, sa: 0x3 : 3, func: 0xe : 14, imm s16: 0xce : 206, imm u16: 0xce : 206, imm u26: 0x3e100ce : 65077454
7f e1 00 ce 	lvx     v31,r1,r0
0x3 - lvx rs,rt,rd

7c e9 e8 96: Unknown G4 opcode! - (7ce9e896 - 1f) - rs: r7, rt: r9, rd: r29, sa: 0x2 : 2, func: 0x16 : 22, imm s16: 0xffffe896 : -5994, imm u16: 0xe896 : 59542, imm u26: 0xe9e896 : 15329430
7c e9 e8 96 	mulhw   r7,r9,r29
0x2 - mulhw rs,rt,rd

7f c6 00 d0: Unknown G4 opcode! - (7fc600d0 - 1f) - rs: r30, rt: r6, rd: r0, sa: 0x3 : 3, func: 0x10 : 16, imm s16: 0xd0 : 208, imm u16: 0xd0 : 208, imm u26: 0x3c600d0 : 63307984
7f c6 00 d0 	neg     r30,r6
0x3 - neg rs,rt

7d 24 01 d2: Unknown G4 opcode! - (7d2401d2 - 1f) - rs: r9, rt: r4, rd: r0, sa: 0x7 : 7, func: 0x12 : 18, imm s16: 0x1d2 : 466, imm u16: 0x1d2 : 466, imm u26: 0x12401d2 : 19136978
7d 24 01 d2 	mulld   r9,r4,r0
0x7 - mulld r9,r4,r0

7c 8b 20 f8: Unknown G4 opcode! - (7c8b20f8 - 1f) - rs: r4, rt: r11, rd: r4, sa: 0x3 : 3, func: 0x38 : 56, imm s16: 0x20f8 : 8440, imm u16: 0x20f8 : 8440, imm u26: 0x8b20f8 : 9117944
7c 8b 20 f8 	not     r11,r4
0x3 - not rt,rs

7d 69 07 74: Unknown G4 opcode! - (7d690774 - 1f) - rs: r11, rt: r9, rd: r0, sa: 0x1d : 29, func: 0x34 : 52, imm s16: 0x774 : 1908, imm u16: 0x774 : 1908, imm u26: 0x1690774 : 23660404
7d 69 07 74 	extsb   r9,r11
0x1d - extsb rt,rs

7c e4 01 94: Unknown G4 opcode! - (7ce40194 - 1f) - rs: r7, rt: r4, rd: r0, sa: 0x6 : 6, func: 0x14 : 20, imm s16: 0x194 : 404, imm u16: 0x194 : 404, imm u26: 0xe40194 : 14942612
7c e4 01 94 	addze   r7,r4
0x6 - addze rs,rt

7c 64 50 92: Unknown G4 opcode! - (7c645092 - 1f) - rs: r3, rt: r4, rd: r10, sa: 0x2 : 2, func: 0x12 : 18, imm s16: 0x5092 : 20626, imm u16: 0x5092 : 20626, imm u26: 0x645092 : 6574226
7c 64 50 92 	mulhd   r3,r4,r10
0x2 - mulhd rs,rt,rd

7d 00 49 10: Unknown G4 opcode! - (7d004910 - 1f) - rs: r8, rt: r0, rd: r9, sa: 0x4 : 4, func: 0x10 : 16, imm s16: 0x4910 : 18704, imm u16: 0x4910 : 18704, imm u26: 0x1004910 : 16795920
7d 00 49 10 	subfe   r8,r0,r9
0x4 - subfe rs,rt,rd

7e e3 b0 78: Unknown G4_G1 opcode! - (7ee3b078 - 1f) - rs: r23, rt: r3, rd: r22, sa: 0x1 : 1, func: 0x38 : 56, imm s16: 0xffffb078 : -20360, imm u16: 0xb078 : 45176, imm u26: 0x2e3b078 : 48476280
7e e3 b0 78 	andc    r3,r23,r22
0x1 - andc rt,rs,rd
*/

class Decoder
{
	int m_code;
	Opcodes& m_op;

	const int RS()		const { return (m_code >> 21) & 0x1F; }
	const int RT()		const { return (m_code >> 16) & 0x1F; }
	const int RD()		const { return (m_code >> 11) & 0x1F; }
	const int SA()		const { return (m_code >>  6) & 0x1F; }
	const int FUNC()	const { return (m_code & 0x3F); }
	const int SYS()		const { return (m_code >> 6) & 0x000fffff; }

	const s32 imm_s16() const { return (s32)(s16)(m_code & 0xffff); }
	const s32 imm_u16() const { return (s32)(m_code & 0xffff); }
	const int imm_u26() const { return m_code & 0x03ffffff; }
	
public:
	Decoder(Opcodes& op) : m_op(op) {}

	~Decoder()
	{
		m_op.Exit();
		delete &m_op;
	}

	void DoCode(const int code)
	{
		m_op.Step();
		if(code == 0)
		{
			m_op.NOP();
			return;
		}

		//if(m_op.DoSysCall(code)) return;

		m_code = code;
		const u32 opcode = (m_code >> 26) & 0x3f;

		s32 temp;

		switch(opcode)
		{
		case SPECIAL:
			
			temp = FUNC();
			switch(temp)
			{
			case SYSCALL:
				m_op.DoSysCall(SYS());
			break;

			default:
				m_op.UNK_SPECIAL(code, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
			break;
			}
		break;
		case SPECIAL2:	m_op.SPECIAL2();					 break;
		//case XOR:		m_op.XOR	(RD(), RS(), RT());		 break;
		case MULLI:		m_op.MULLI	(RT(), RS(), imm_s16()); break;
		case SUBFIC:	m_op.SUBFIC	(RS(), RT(), imm_s16()); break;
		case CMPLWI:	m_op.CMPLWI	(RS(), RT(), imm_u16()); break;
		case CMPWI:		m_op.CMPWI	(RS(), RT(), RD());		 break;
		case ADDIC:		m_op.ADDIC	(RT(), RS(), imm_s16()); break;
		case ADDIC_:	m_op.ADDIC_	(RT(), RS(), imm_s16()); break;
		case G1:		m_op.G1();							 break;
		case ADDIS:		m_op.ADDIS	(RT(), RS(), imm_s16()); break;
		case G2:		m_op.G2();							 break;
		case SC:		m_op.SC();							 break;
		case G3:		m_op.G3();							 break;
		case BLR:		m_op.BLR();							 break;
		case RLWINM:	m_op.RLWINM();						 break;
		case ROTLW:		m_op.ROTLW	(RT(), RS(), RD());		 break;
		case ORI:		m_op.ORI	(RT(), RS(), imm_u16()); break;
		case ORIS:		m_op.ORIS	(RT(), RS(), imm_u16()); break;
		case XORI:		m_op.XORI	(RT(), RS(), imm_u16()); break;
		case XORIS:		m_op.XORIS	(RT(), RS(), imm_u16()); break;
		case CLRLDI:	m_op.CLRLDI	(RT(), RS(), imm_u16()); break;
		case G4:
			temp = SA();
			switch(temp)
			{
			case CMPW:	m_op.CMPW	(RS(), RT(), RD());		 break;
			case G4_G1:
				temp = FUNC();
				switch(temp)
				{
				case CMPLD: m_op.CMPLD	(RS(), RT(), RD());	 break;
				case LVEHX:  m_op.LVEHX	(RS(), RT(), RD());	 break;
				case LVSR:  m_op.LVSR	(RS(), RT(), RD());	 break;
				case SUBF:	m_op.SUBF	(RS(), RT(), RD());	 break;
				case SUBF_:	m_op.SUBF_	(RS(), RT(), RD());	 break;
				case LDUX:	m_op.LDUX	(RS(), RT(), RD());	 break;
				case LWZUX:	m_op.LWZUX	(RS(), RT(), RD());	 break;
				case CNTLZD:m_op.CNTLZD	(RS(), RT());		 break;
				case ANDC:	m_op.ANDC	(RT(), RS(), RD());	 break;

				default:
					m_op.UNK_G4_G1(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case MULHW: m_op.MULHW	(RS(), RT(), RD());		 break;
			case G4_G2:
				temp = FUNC();
				switch(temp)
				{
				case LVX:		m_op.LVX	(RS(), RT(), RD());		 break;
				case NEG:		m_op.NEG	(RS(), RT());			 break;
				case MTSRDIN:	m_op.MTSRDIN(RS(), RD());			 break;
				case NOT:		m_op.NOT	(RT(), RD());			 break;

				default:
					m_op.UNK_G4_G2(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case G4_G3:
				temp = FUNC();
				switch(temp)
				{
				case WRTEE: m_op.WRTEE	(RS());				 break;
				case STVEBX: m_op.STVEBX(RS(), RT(), RD());	 break;
				case SUBFE:  m_op.SUBFE	(RS(), RT(), RD());	 break;
				case SUBFE_: m_op.SUBFE_(RS(), RT(), RD());	 break;
				case ADDE:   m_op.ADDE	(RS(), RT(), RD());	 break;
				case ADDE_:  m_op.ADDE_	(RS(), RT(), RD());	 break;
				case MTOCRF: m_op.MTOCRF(RS());				 break;
				case STDX:	 m_op.STDX	(RS(), RT(), RD());	 break;
				case STWCX_: m_op.STWCX_(RS(), RT(), RD());	 break;
				case STWX:   m_op.STWX	(RS(), RT(), RD());	 break;
				case SLQ:	 m_op.SLQ	(RT(), RS(), RD());	 break;
				case SLQ_:   m_op.SLQ_	(RT(), RS(), RD());	 break;
				case SLE_:   m_op.SLE_	(RT(), RS(), RD());	 break;

				default:
					m_op.UNK_G4_G3(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case STDUX: m_op.STDUX	(RS(), RT(), RD());		 break;
			case ADDZE: m_op.ADDZE	(RS(), RT());			 break;
			case G4_G4:
				temp = FUNC();
				switch(temp)
				{
				case STVX:   m_op.STVX	(RS(), RT(), RD());	 break;
				case MULLD:  m_op.MULLD	(RS(), RT(), RD());	 break;
				case ADDME_: m_op.ADDME_(RS(), RT());		 break;
				case MULLW:	 m_op.MULLW	(RS(), RT(), RD());	 break;
				case MULLW_: m_op.MULLW_(RS(), RT(), RD());	 break;
				case DCBTST: m_op.DCBTST(RT(), RD());	     break;
				case SLLIQ:  m_op.SLLIQ	(RT(), RS(), RD());	 break;

				default:
					m_op.UNK_G4_G4(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case ADD:	m_op.ADD	(RS(), RT(), RD());		 break;
			case XOR:	m_op.XOR	(RT(), RS(), RD());		 break;
			case SRW:	m_op.SRW	(RT(), RS(), RD());		 break;
			case MFLR:	m_op.MFLR	(RS());					 break;
			case G4_G5:
				temp = FUNC();
				switch(temp)
				{
				case STHX:	m_op.STHX	(RS(), RT(), RD());	 break;
				case ORC:	m_op.ORC	(RT(), RS(), RD());	 break;
				case ORC_:	m_op.ORC_	(RT(), RS(), RD());	 break;

				default: 
					m_op.UNK_G4_G5(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case LVXL:	m_op.LVXL	(RS(), RT(), RD());		 break;
			case MR:	m_op.MR		(RT(), RS());			 break;
			case G4_G6:
				temp = FUNC();
				switch(temp)
				{
				case MTDCR:	m_op.MTDCR	(RS());				 break;
				case DCCCI:	m_op.DCCCI	(RT(), RD());		 break;
				case DIVDU:	m_op.DIVDU	(RS(), RT(), RD());  break;
				case DIVDU_:m_op.DIVDU_	(RS(), RT(), RD());	 break;
				case DIVWU:	m_op.DIVWU	(RS(), RT(), RD());  break;
				case DCBI:	m_op.DCBI	(RT(), RD());		 break;
				case NAND:	m_op.NAND	(RT(), RS(), RD());  break;
				case NAND_:	m_op.NAND_	(RT(), RS(), RD());  break;
				case G4_G6_G1:
					temp = RT();
					switch(temp)
					{
					case MTSPR:	m_op.MTSPR	(RS());			 break;
					case MTLR:	m_op.MTLR	(RS());			 break;
					case MTCTR: m_op.MTCTR	(RS());			 break;

					default:
						m_op.UNK_G4_G6_G1(code, opcode, RS(), temp, RD(), SA(), FUNC(), imm_s16(), imm_u16(), imm_u26());
					break;
					};
				break;

				default:
					m_op.UNK_G4_G6(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case DIVW:	 m_op.DIVW	(RS(), RT(), RD());		 break;
			case SUBFO:	 m_op.SUBFO	(RS(), RT(), RD());		 break;
			case LWSYNC: m_op.LWSYNC();						 break;
			
			case G4_G7:
				temp = FUNC();
				switch(temp)
				{
				case NEGO:  m_op.NEGO();					 break;
				case MULO:	m_op.MULO	(RS(), RT(), RD());	 break;
				case MULO_:	m_op.MULO_	(RS(), RT(), RD());	 break;
				case MFSRI: m_op.MFSRI();					 break;
				default:
					m_op.UNK_G4_G7(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case STFSX:	m_op.STFSX	(RS(), RT(), RD());		 break;
			case G4_G8:
				temp = FUNC();
				switch(temp)
				{
				case STSWI: m_op.STSWI	(RS(), RT(), RD());	 break;
				case STFDX:	m_op.STFDX	(RS(), RT(), RD());	 break;
				case SRLQ:	m_op.SRLQ	(RT(), RS(), RD());	 break;
				case SREQ:	m_op.SREQ	(RT(), RS(), RD());	 break;

				default:
					m_op.UNK_G4_G8(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case G4_G9:
				temp = FUNC();
				switch(temp)
				{
				case MULLDO:	m_op.MULLDO	(RS(), RT(), RD());	break;
				case MULLWO:	m_op.MULLWO	(RS(), RT(), RD());	break;
				case MULLWO_:	m_op.MULLWO_(RS(), RT(), RD());	break;

				default:
					m_op.UNK_G4_G9(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			
			case SRAW:	m_op.SRAW	(RT(), RS(), RD());		 break;
			case SRAWI: m_op.SRAWI	(RT(), RS(), RD());		 break;
			case EIEIO: m_op.EIEIO();						 break;
			case G4_G10:
				temp = FUNC();
				switch(temp)
				{
				case ABSO_:	m_op.ABSO_();					 break;
				case DIVSO_:m_op.DIVSO_	(RT(), RS(), RD());	 break;

				default:
					m_op.UNK_G4_G10(code, opcode, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
				break;
				};
			break;
			case EXTSH: m_op.EXTSH	(RT(), RS());			 break;
			case EXTSB: m_op.EXTSB	(RT(), RS());			 break;
			case EXTSW: m_op.EXTSW	(RT(), RS());			 break;
			case DCBZ:	m_op.DCBZ	(RT(), RD());			 break;
			case LWZX:	m_op.LWZX	(RS(), RT(), RD());		 break;

			default:
				m_op.UNK_G4(code, opcode, RS(), RT(), RD(), temp, FUNC(), imm_s16(), imm_u16(), imm_u26());
			break;
			};
		break;
		case LWZ:		m_op.LWZ	(RT(), RS(), imm_s16()); break;
		case LWZU:		m_op.LWZU	(RT(), RS(), imm_s16()); break;
		case LBZ:		m_op.LBZ	(RT(), RS(), imm_s16()); break;
		case LBZU:		m_op.LBZU	(RT(), RS(), imm_s16()); break;
		case STW:		m_op.STW	(RS(), RT(), imm_s16()); break;
		case STWU:		m_op.STWU	(RS(), RT(), imm_s16()); break;
		case STB:		m_op.STB	(RS(), RT(), imm_s16()); break;
		case STBU:		m_op.STBU	(RS(), RT(), imm_s16()); break;
		case LHZ:		m_op.LHZ	(RS(), RT(), imm_s16()); break;
		case LHZU:		m_op.LHZU	(RS(), RT(), imm_s16()); break;
		case STH:		m_op.STH	(RS(), RT(), imm_s16()); break;
		case STHU:		m_op.STHU	(RS(), RT(), imm_s16()); break;
		case LFS:		m_op.LFS	(RS(), RT(), imm_s16()); break;
		case LFSU:		m_op.LFSU	(RS(), RT(), imm_s16()); break;
		case LFD:		m_op.LFD	(RS(), RT(), imm_s16()); break;
		case STFS:		m_op.STFS	(RS(), RT(), imm_s16()); break;
		case STFSU:		m_op.STFSU	(RS(), RT(), imm_s16()); break;
		case STFD:		m_op.STFD	(RS(), RT(), imm_s16()); break;
		case LD:		m_op.LD		(RS(), RT(), imm_s16()); break;
		case FDIVS:		m_op.FDIVS	(RS(), RT(), RD());		 break;
		case STD: 		m_op.STD	(RS(), RT(), imm_s16()); break;
		case G5: 		m_op.G5();							 break;

		default: m_op.UNK(code, opcode, RS(), RT(), RD(), SA(), FUNC(), imm_s16(), imm_u16(), imm_u26()); break;
		};
	}
};