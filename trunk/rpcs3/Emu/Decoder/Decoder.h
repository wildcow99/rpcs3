#pragma once

#include "Opcodes/Opcodes.h"

/*
	rD = rt
	rA = rs
	rB = rd
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

		if(m_op.DoSysCall(code)) return;

		m_code = code;
		const u32 opcode = (m_code >> 26) & 0x3f;

		s32 temp;

		switch(opcode)
		{
		case SPECIAL:
			
			temp = FUNC();
			/*
			switch(temp)
			{
			default:
			*/
				m_op.UNK_SPECIAL(code, RS(), RT(), RD(), SA(), temp, imm_s16(), imm_u16(), imm_u26());
			/*
			break;
			}
			*/
		break;
		case SPECIAL2:	m_op.SPECIAL2();					 break;
		case XOR:		m_op.XOR	(RD(), RS(), RT());		 break;
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
		case G4:		m_op.G4();							 break;
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