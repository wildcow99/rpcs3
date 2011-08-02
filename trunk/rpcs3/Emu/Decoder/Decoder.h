#pragma once

#include "Opcodes/Opcodes.h"

#define START_OPCODES_GROUP(group, reg) \
	case(##group##): \
		temp=##reg##();\
		switch(temp)\
		{

#define END_OPCODES_GROUP(group) \
		default:\
			m_op.UNK_##group##(code, opcode, RS(), RT(), RD(), SA(), FUNC(),\
			CRFS(), CRFT(), CRM(), BD(), LK(), MS(), MT(), MFM(), LS(), LT(),\
			imm_m(), imm_s16(), imm_u16(), imm_u26());\
		break;\
		}\
	break

#define ADD_OPCODE(name, regs) case(##name##):m_op.##name####regs##; break
#define ADD_NULL_OPCODE(name) ADD_OPCODE(##name##, ())

class Decoder
{
	int m_code;
	Opcodes& m_op;

	OP_REG RS()			const { return (m_code >> 21) & 0x1F; }
	OP_REG RT()			const { return (m_code >> 16) & 0x1F; }
	OP_REG RD()			const { return (m_code >> 11) & 0x1F; }
	OP_REG SA()			const { return (m_code >>  6) & 0x1F; }
	OP_REG FUNC()		const { return (m_code & 0x3F); }
	OP_REG SYS()		const { return (m_code & 0x00ffffff); }

	OP_REG SPR()		const { return (m_code & 0x001FF800) >> 11; }
	OP_REG SPRVAL()		const { return (SPR() & 0x1F) + ((SPR() >> 5) & 0x1F); }
	OP_REG CRFS()		const { return (m_code & 0x03800000) >> 23; }
	OP_REG CRFT()		const { return (m_code & 0x001C0000) >> 18; }
	OP_REG CRM()		const { return (m_code >> 12) & 0xff; }
	OP_REG BD()			const { return (m_code >> 2) & 0x3fff; }
	OP_REG LK()			const { return (m_code & 0x1); }
	OP_REG LS()			const { return (m_code >> 16) & 0x1; }
	OP_REG LT()			const { return (m_code >> 8) & 0x1; }
	OP_REG MS()			const { return (m_code & 0x7C0) >> 6; }
	OP_REG MT()			const { return (m_code & 0x3E) >> 1; }
	OP_REG MFM()		const { return (m_code >> 17 ) & 0xff; }
	const s32 imm_m()	const { return (m_code >> 12) & 0xf; }

	const s32 imm_s16()	const { return (s32)(s16)(m_code & 0xffff); }
	const u32 imm_u16()	const { return m_code & 0xffff; }
	const u32 imm_u26()	const { return m_code & 0x3fffffc; }
	
public:
	Decoder(Opcodes& op) : m_op(op) {}

	~Decoder()
	{
		m_op.Exit();
	}

	void DoCode(const int code)
	{
		m_op.Step();

		const u32 opcode = (code >> 26) & 0x3f;

		if(code == 0 || opcode == 0) //Skip special
		{
			m_op.NOP();
			return;
		}

		m_code = code;

		s32 temp;

		switch(opcode)
		{
		//START_OPCODES_GROUP(SPECIAL, FUNC)
		//END_OPCODES_GROUP(SPECIAL);

		START_OPCODES_GROUP(SPECIAL2, SA)
		END_OPCODES_GROUP(SPECIAL2);

//		ADD_OPCODE(XOR,		(RD(), RS(), RT()));
		ADD_OPCODE(MULLI,	(RT(), RS(), imm_s16()));
		ADD_OPCODE(SUBFIC,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(CMPLDI,	(RS(), RT(), imm_s16()));
		//ADD_OPCODE(CMPDI,	(RS(), RT(), RD()));
		ADD_OPCODE(ADDIC,	(RT(), RS(), imm_s16()));
		ADD_OPCODE(ADDIC_,	(RT(), RS(), imm_s16()));

		START_OPCODES_GROUP(G_0x04, SA)
			START_OPCODES_GROUP(G_0x04_0x0, FUNC)
				ADD_OPCODE(MULHHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MULHHWU_,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x0);

			START_OPCODES_GROUP(G_0x04_0x1, FUNC)
				ADD_OPCODE(MULHHW,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHHW_,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHW,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACHHW,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x1);

			START_OPCODES_GROUP(G_0x04_0x2, FUNC)
				ADD_OPCODE(MACHHWSU,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWSU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x2);

			START_OPCODES_GROUP(G_0x04_0x3, FUNC)
				ADD_OPCODE(MACHHWS,		(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWS_,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACHHWS,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x3);

			START_OPCODES_GROUP(G_0x04_0x4, FUNC)
				ADD_OPCODE(MULCHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MULCHWU_,	(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x4);

			START_OPCODES_GROUP(G_0x04_0x5, FUNC)
				ADD_OPCODE(MULCHW,	(RS(), RT(), RD()));
				ADD_OPCODE(MACCHW,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACCHW,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x5);

			START_OPCODES_GROUP(G_0x04_0x6, FUNC)
				ADD_OPCODE(MACCHWSU,	(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWSU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x6);

			START_OPCODES_GROUP(G_0x04_0x7, FUNC)
				ADD_OPCODE(MACCHWS,		(RS(), RT(), RD()));
				ADD_OPCODE(NMACCHWS,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACCHWS_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x7);

			START_OPCODES_GROUP(G_0x04_0xc, FUNC)
				ADD_OPCODE(MULLHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MULLHWU_,	(RS(), RT(), RD()));
				ADD_OPCODE(MACLHWU,		(RS(), RT(), RD()));
				ADD_OPCODE(MACLHWU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0xc);

			START_OPCODES_GROUP(G_0x04_0xd, FUNC)
				ADD_OPCODE(NMACLHW,		(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0xd);

			START_OPCODES_GROUP(G_0x04_0xe, FUNC)
				ADD_OPCODE(MACLHWSU,	(RS(), RT(), RD()));
				ADD_OPCODE(MACLHWSU_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0xe);

			START_OPCODES_GROUP(G_0x04_0xf, FUNC)
				ADD_OPCODE(MACLHWS,		(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0xf);

			START_OPCODES_GROUP(G_0x04_0x10, FUNC)
				ADD_OPCODE(MACHHWUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x10);

			START_OPCODES_GROUP(G_0x04_0x12, FUNC)
				ADD_OPCODE(MACHHWSUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWSUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x12);

			START_OPCODES_GROUP(G_0x04_0x13, FUNC)
				ADD_OPCODE(MACHHWSO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACHHWSO_,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACHHWSO,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x13);

			START_OPCODES_GROUP(G_0x04_0x14, FUNC)
				ADD_OPCODE(MACCHWUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x14);

			START_OPCODES_GROUP(G_0x04_0x15, FUNC)
				ADD_OPCODE(MACCHWO,		(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWO_,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACCHWO,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x15);

			START_OPCODES_GROUP(G_0x04_0x16, FUNC)
				ADD_OPCODE(MACCHWSUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACCHWSUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x16);

			START_OPCODES_GROUP(G_0x04_0x17, FUNC)
				ADD_OPCODE(NMACCHWSO,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x17);

			START_OPCODES_GROUP(G_0x04_0x18, FUNC)
				ADD_OPCODE(MFVSCR,		(RS()));
			END_OPCODES_GROUP	(G_0x04_0x18);

			START_OPCODES_GROUP(G_0x04_0x1c, FUNC)
				ADD_OPCODE(MACLHWUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACLHWUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x1c);

			START_OPCODES_GROUP(G_0x04_0x1d, FUNC)
				ADD_OPCODE(NMACLHWO,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACLHWO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x1d);

			START_OPCODES_GROUP(G_0x04_0x1e, FUNC)
				ADD_OPCODE(MACLHWSUO,	(RS(), RT(), RD()));
				ADD_OPCODE(MACLHWSUO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x1e);

			START_OPCODES_GROUP(G_0x04_0x1f, FUNC)
				ADD_OPCODE(NMACLHWSO,	(RS(), RT(), RD()));
				ADD_OPCODE(NMACLHWSO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP	(G_0x04_0x1f);
		END_OPCODES_GROUP	(G_0x04);

		START_OPCODES_GROUP(G_0x0b, SA)
		END_OPCODES_GROUP(G_0x0b);

		START_OPCODES_GROUP(G1, LS)
			ADD_OPCODE(LI,		(RS(), imm_s16()));
			ADD_OPCODE(ADDI,	(RS(), RT(), imm_s16()));
		END_OPCODES_GROUP(G1);

		START_OPCODES_GROUP(G_0x0f, LT)
			
				ADD_OPCODE(ADDIS,	(RT(), RS(), imm_s16()));
				ADD_OPCODE(LIS,		(RS(), imm_s16()));
			
		END_OPCODES_GROUP(G_0x0f);

		START_OPCODES_GROUP(G2, SA)
		END_OPCODES_GROUP(G2);

		ADD_OPCODE(SC,			(SYS()));

		START_OPCODES_GROUP(BRANCH, LK)
			ADD_OPCODE(B,		(imm_u26()));
			ADD_OPCODE(BL,		(imm_u26()));
		END_OPCODES_GROUP(BRANCH);

		START_OPCODES_GROUP(G3_S, FUNC)
			START_OPCODES_GROUP(G3_S_0x0, FUNC)
				ADD_OPCODE(MCRF,	(CRFS(), CRFT()));
				START_OPCODES_GROUP(G3_S_0x0_0x20, RS)
				ADD_OPCODE(BLELR,	(CRFT()));
				ADD_NULL_OPCODE		(BEQLR);
				ADD_NULL_OPCODE		(BLR);
				END_OPCODES_GROUP(G3_S_0x0_0x20);
			END_OPCODES_GROUP(G3_S_0x0);
			START_OPCODES_GROUP(G3_S_0x10, FUNC)
				ADD_NULL_OPCODE		(BCTR);
				ADD_NULL_OPCODE		(BCTRL);
			END_OPCODES_GROUP(G3_S_0x10);
			
		END_OPCODES_GROUP(G3_S);

		START_OPCODES_GROUP(G3_S0, FUNC)
			ADD_NULL_OPCODE		(RLWINM_);
			ADD_NULL_OPCODE		(RLWINM);
			//
			//
			START_OPCODES_GROUP(G3_S0_0x3e, LT)
				ADD_OPCODE(ROTLWI,	(RT(), RS(), imm_u16()));
				ADD_OPCODE(CLRLWI,	(RT(), RS(), imm_u16()));
			END_OPCODES_GROUP(G3_S0_0x3e);
			START_OPCODES_GROUP(G3_S0_0x3f, LT)
				ADD_OPCODE(ROTLWI_,	(RT(), RS(), imm_u16()));
				ADD_OPCODE(CLRLWI_,	(RT(), RS(), imm_u16()));
			END_OPCODES_GROUP(G3_S0_0x3f);
		END_OPCODES_GROUP(G3_S0);
		
		
		ADD_OPCODE(ROTLW,	(RT(), RS(), RD()));
		ADD_OPCODE(ORI,		(RT(), RS(), imm_u16()));
		ADD_OPCODE(ORIS,	(RT(), RS(), imm_u16()));
		ADD_OPCODE(XORI,	(RT(), RS(), imm_u16()));
		ADD_OPCODE(XORIS,	(RT(), RS(), imm_u16()));
		ADD_OPCODE(ANDI_,	(RT(), RS(), imm_u16()));
		ADD_OPCODE(ANDIS_,	(RT(), RS(), imm_u16()));
		//ADD_OPCODE(LRLDIÑ,	(RT(), RS(), imm_u16()));
		START_OPCODES_GROUP(G_0x1e, FUNC)
				ADD_OPCODE(CLRLDI,	(RT(), RS(), imm_u16()));
				ADD_OPCODE(CLRLDI_,	(RT(), RS(), imm_u16()));
		END_OPCODES_GROUP(G_0x1e);

		START_OPCODES_GROUP(G4, SA)
			START_OPCODES_GROUP(G4_G0, FUNC)
				//ADD_OPCODE(CMPW,	(RS(), RT(), RD()));
				ADD_OPCODE(LVSL,	(RS(), RT(), RD()));
				ADD_OPCODE(LVEBX,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHDU,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHDU_,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDC,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHWU,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHWU_,	(RS(), RT(), RD()));
				//ADD_OPCODE(MFCR,	(RS()));

				START_OPCODES_GROUP(G4_G0_0x26, RT)
					ADD_OPCODE(MFCR,	(RS()));
					ADD_OPCODE(MFOCRF,	(RS(), imm_s16()));
				END_OPCODES_GROUP(G4_G0_0x26);

				ADD_OPCODE(LWARX,	(RS(), RD(), imm_s16()));
				ADD_OPCODE(LDX,		(RS(), RT(), RD()));
				ADD_OPCODE(LWZX,		(RS(), RT(), RD()));
				ADD_OPCODE(CNTLZD,	(RT(), RS()));
				ADD_OPCODE(AND,		(RT(), RS(), RD()));
				ADD_OPCODE(AND_,	(RT(), RS(), RD()));
				ADD_OPCODE(MASKG,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G0);

			START_OPCODES_GROUP(G4_G1, FUNC)
				//ADD_OPCODE(CMPLD,	(RS(), RT(), RD()));
				ADD_OPCODE(LVEHX,	(RS(), RT(), RD()));
				ADD_OPCODE(LVSR,	(RS(), RT(), RD()));
				ADD_OPCODE(SUBF,	(RS(), RT(), RD()));
				ADD_OPCODE(SUBF_,	(RS(), RT(), RD()));
				ADD_OPCODE(LDUX,	(RS(), RT(), RD()));
				ADD_OPCODE(LWZUX,	(RS(), RT(), RD()));
				ADD_OPCODE(CNTLZD,	(RS(), RT()));
				ADD_OPCODE(ANDC,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G1);

			//ADD_OPCODE(MULHW,		(RS(), RT(), RD()));

			START_OPCODES_GROUP	(G4_G_0x2, FUNC)
				ADD_OPCODE(LVEWX,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHD,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHD_,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHW,	(RS(), RT(), RD()));
				ADD_OPCODE(MULHW_,	(RS(), RT(), RD()));
				ADD_OPCODE(DLMZB,	(RT(), RS(), RD()));
				ADD_OPCODE(DLMZB_,	(RT(), RS(), RD()));
				ADD_OPCODE(LDARX,	(RT(), RS(), RD()));
				ADD_OPCODE(DCBF,	(RT(), RS(), RD()));
				ADD_OPCODE(LBZX,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G_0x2);

			START_OPCODES_GROUP	(G4_G2, FUNC)
				ADD_OPCODE(LVX,		(RS(), RT(), RD()));
				ADD_OPCODE(NEG,		(RS(), RT()));
				ADD_OPCODE(MUL,		(RS(), RT(), RD()));
				ADD_OPCODE(MUL_,	(RS(), RT(), RD()));
				ADD_OPCODE(MTSRDIN,	(RS(), RD()));
				//ADD_OPCODE(NOT,		(RT(), RD()));
			END_OPCODES_GROUP(G4_G2);

			START_OPCODES_GROUP(G4_G3, FUNC)
				ADD_OPCODE(WRTEE,	(RS()));
				ADD_OPCODE(STVEBX,	(RS(), RT(), RD()));
				ADD_OPCODE(SUBFE,	(RS(), RT(), RD()));
				ADD_OPCODE(SUBFE_,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDE,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDE_,	(RS(), RT(), RD()));
				//ADD_OPCODE(MTOCRF,	(RS()));
				ADD_OPCODE(STDX,	(RS(), RT(), RD()));
				ADD_OPCODE(STWCX_,	(RS(), RT(), RD()));
				ADD_OPCODE(STWX,	(RS(), RT(), RD()));
				ADD_OPCODE(SLQ,		(RT(), RS(), RD()));
				ADD_OPCODE(SLQ_,	(RT(), RS(), RD()));
				ADD_OPCODE(SLE_,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G3);

			ADD_OPCODE(STDUX,		(RS(), RT(), RD()));
			//ADD_OPCODE(ADDZE,		(RS(), RT()));

			START_OPCODES_GROUP(G4_G4, FUNC)
				ADD_OPCODE(STVX,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLD,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLD_,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDME,	(RS(), RT()));
				ADD_OPCODE(ADDME_,	(RS(), RT()));
				ADD_OPCODE(MULLW,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLW_,	(RS(), RT(), RD()));
				ADD_OPCODE(DCBTST,	(RT(), RD()));
				ADD_OPCODE(SLLIQ,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G4);

			START_OPCODES_GROUP(G4_G_ADD, FUNC)
				ADD_OPCODE(ICBT,	(RS(), RT(), RD()));
				ADD_OPCODE(DOZ,		(RS(), RT(), RD()));
				ADD_OPCODE(DOZ_,	(RS(), RT(), RD()));
				ADD_OPCODE(ADD,		(RS(), RT(), RD()));
				ADD_OPCODE(ADD_,	(RS(), RT(), RD()));
				ADD_OPCODE(LSCBX,	(RS(), RT(), RD()));
				ADD_OPCODE(LSCBX_,	(RS(), RT(), RD()));
				ADD_OPCODE(DCBT,	(RT(), RD()));
				ADD_OPCODE(LHZX,	(RS(), RT(), RD()));
				ADD_OPCODE(EQV,		(RS(), RT(), RD()));
				ADD_OPCODE(EQV_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_ADD);

			START_OPCODES_GROUP(G4_G_ADDZE, FUNC)
				ADD_OPCODE(ADDZE,		(RS(), RT()));
				ADD_OPCODE(ADDZE_,		(RS(), RT()));
			END_OPCODES_GROUP(G4_G_ADDZE);

			//ADD_OPCODE(XOR,		(RT(), RS(), RD()));
			//ADD_OPCODE(MFLR,		(RS()));

			START_OPCODES_GROUP(G4_G_0x9, FUNC)
				ADD_OPCODE(DOZI,	(RS(), RT(), imm_s16()));
				ADD_OPCODE(LHZUX,	(RS(), RT(), RD()));
				ADD_OPCODE(XOR,		(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G_0x9);

			START_OPCODES_GROUP(G4_G_0xa, FUNC)
				ADD_OPCODE(MFDCR,	(RS(), RT(), RD()));
				ADD_OPCODE(DIV,		(RS(), RT(), RD()));
				ADD_OPCODE(DIV_,	(RS(), RT(), RD()));
				START_OPCODES_GROUP(G4_G_0xa_0x26, RT)
					ADD_OPCODE(MFSPR,	(RS(), imm_s16()));
					ADD_OPCODE(MFLR,	(RS()));
					ADD_OPCODE(MFCTR,	(RS()));
				END_OPCODES_GROUP(G4_G_0xa_0x26);

				//ADD_OPCODE(MFCTR,		(RS()));
				ADD_OPCODE(LWAX,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_0xa);

			START_OPCODES_GROUP(G4_G_0xb, FUNC)
				ADD_OPCODE(LVXL,	(RS(), RT(), RD()));
				ADD_OPCODE(MFTB,	(RS()));
			END_OPCODES_GROUP(G4_G_0xb);

			START_OPCODES_GROUP(G4_G5, FUNC)
				ADD_OPCODE(STHX,	(RS(), RT(), RD()));
				ADD_OPCODE(ORC,		(RT(), RS(), RD()));
				ADD_OPCODE(ORC_,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G5);

			//ADD_OPCODE(MR,			(RT(), RS()));

			START_OPCODES_GROUP(G4_G6, FUNC)
				ADD_OPCODE(MTDCR,	(RS()));
				ADD_OPCODE(DCCCI,	(RT(), RD()));
				ADD_OPCODE(DIVDU,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVDU_,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVWU,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVWU_,	(RS(), RT(), RD()));
				ADD_OPCODE(DCBI,	(RT(), RD()));
				ADD_OPCODE(NAND,	(RT(), RS(), RD()));
				ADD_OPCODE(NAND_,	(RT(), RS(), RD()));

				START_OPCODES_GROUP(G4_G6_G1, RT)
//					ADD_OPCODE(MTSPR,		(RS()));
					ADD_OPCODE(MTLR,		(RS()));
					ADD_OPCODE(MTCTR,		(RS()));
				END_OPCODES_GROUP(G4_G6_G1);

			END_OPCODES_GROUP(G4_G6);

			//ADD_OPCODE(DIVW,		(RS(), RT(), RD()));

			START_OPCODES_GROUP(G4_G_0xf, FUNC)
				ADD_OPCODE(DIVD,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVD_,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVW,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_0xf);

			START_OPCODES_GROUP(G4_G_ADDCO, FUNC)
				//ADD_OPCODE(SRW,	(RT(), RS(), RD()));
				ADD_OPCODE(MCRXR,	(RS()));
				ADD_OPCODE(LVLX,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDCO,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDCO_,	(RS(), RT(), RD()));
				ADD_OPCODE(LSWX,	(RS(), RT(), RD()));
				ADD_OPCODE(LWBRX,	(RS(), RT(), RD()));
				ADD_OPCODE(LFSX,	(RS(), RT(), RD()));
				ADD_OPCODE(MASKIR,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G_ADDCO);

			//ADD_OPCODE(SUBFO,		(RS(), RT(), RD()));
			//ADD_NULL_OPCODE(LWSYNC);

			START_OPCODES_GROUP(G4_G_0x11, FUNC)
				ADD_OPCODE(LVRX,	(RS(), RT(), RD()));
				ADD_OPCODE(SUBFO,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_0x11);

			START_OPCODES_GROUP(G4_G_0x12, FUNC)

				START_OPCODES_GROUP(G4_G_0x12_0x26, RT)
					ADD_OPCODE(MFSR,	(RS(), imm_s16()));
				END_OPCODES_GROUP(G4_G_0x12_0x26);

				ADD_NULL_OPCODE(LWSYNC);
				ADD_OPCODE(LFDX,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_0x12);

			START_OPCODES_GROUP(G4_G7, FUNC)
				ADD_NULL_OPCODE(NEGO);
				ADD_OPCODE(MULO,	(RS(), RT(), RD()));
				ADD_OPCODE(MULO_,	(RS(), RT(), RD()));

				START_OPCODES_GROUP(G4_G7_0x26, RT)
					ADD_OPCODE(MFSRI,		(RS(), RT(), RD()));
				END_OPCODES_GROUP(G4_G7_0x26);

			END_OPCODES_GROUP(G4_G7);
			//ADD_OPCODE(STFSX,		(RS(), RT(), RD()));
			START_OPCODES_GROUP(G4_G_ADDEO, FUNC)
				ADD_OPCODE(ADDEO,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDEO_,	(RS(), RT(), RD()));

				START_OPCODES_GROUP(G4_G_ADDEO_0x26, RT)
					ADD_OPCODE(MFSRIN,		(RS(), RD()));
				END_OPCODES_GROUP(G4_G_ADDEO_0x26);

			END_OPCODES_GROUP(G4_G_ADDEO);

			START_OPCODES_GROUP(G4_G8, FUNC)
				ADD_OPCODE(ADDZEO_,	(RS(), RT(), RD())); //temp
				ADD_OPCODE(STSWI,	(RS(), RT(), RD()));
				ADD_OPCODE(STFDX,	(RS(), RT(), RD()));
				ADD_OPCODE(SRLQ,	(RT(), RS(), RD()));
				ADD_OPCODE(SREQ,	(RT(), RS(), RD()));
			END_OPCODES_GROUP(G4_G8);

			START_OPCODES_GROUP(G4_G_ADDO, FUNC)
				ADD_OPCODE(LVLXL,	(RS(), RT(), RD()));
				ADD_OPCODE(DOZO,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDO,	(RS(), RT(), RD()));
				ADD_OPCODE(ADDO_,	(RS(), RT(), RD()));
				ADD_OPCODE(LHBRX,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G_ADDO);

			START_OPCODES_GROUP(G4_G9, FUNC)
				ADD_OPCODE(MULLDO,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLDO_,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLWO,	(RS(), RT(), RD()));
				ADD_OPCODE(MULLWO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G9);

			//ADD_OPCODE(SRAW,		(RT(), RS(), RD()));
			ADD_OPCODE(SRAWI,		(RT(), RS(), RD()));
			//ADD_NULL_OPCODE(EIEIO);

			START_OPCODES_GROUP(G4_G_0x1a, FUNC)
				ADD_OPCODE(DIVO,	(RT(), RS(), RD()));
				ADD_OPCODE(DIVO_,	(RT(), RS(), RD()));
				ADD_NULL_OPCODE(EIEIO);
			END_OPCODES_GROUP(G4_G_0x1a);

			START_OPCODES_GROUP(G4_G10, FUNC)
				ADD_NULL_OPCODE(ABSO_);
				ADD_OPCODE(DIVSO,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVSO_,	(RS(), RT(), RD()));
			END_OPCODES_GROUP(G4_G10);

			START_OPCODES_GROUP(G4_G_0x1e, FUNC)
				ADD_OPCODE(ICCCI,	(RT(), RS(), RD()));
				ADD_OPCODE(RLDCL_,	(RT(), RS(), RD(), imm_s16()));
				ADD_OPCODE(DIVDUO,	(RT(), RS(), RD()));
				ADD_OPCODE(DIVDUO_,	(RT(), RS(), RD()));
				ADD_OPCODE(DIVWUO,	(RT(), RS(), RD()));
				ADD_OPCODE(DIVWUO_,	(RT(), RS(), RD()));
				ADD_OPCODE(ICBI,	(RT(), RS(), RD()));
				ADD_OPCODE(RLDCL,	(RT(), RS(), RD(), imm_s16()));
				ADD_OPCODE(EXTSW,	(RT(), RS()));
				ADD_OPCODE(EXTSW_,	(RT(), RS()));
			END_OPCODES_GROUP(G4_G_0x1e);

			START_OPCODES_GROUP(G4_G_0x1f, FUNC)
				ADD_OPCODE(ICREAD,	(RS(), RT(), RD()));
				ADD_OPCODE(DIVDO,	(RS(), RT(), RD()));
				ADD_OPCODE(DCBZ,	(RT(), RD()));
			END_OPCODES_GROUP(G4_G_0x1f);

			ADD_OPCODE(EXTSH,		(RT(), RS()));
			ADD_OPCODE(EXTSB,		(RT(), RS()));
			//ADD_OPCODE(EXTSW,		(RT(), RS()));
			//ADD_OPCODE(DCBZ,		(RT(), RD()));
			//ADD_OPCODE(LWZX,		(RS(), RT(), RD()));
		END_OPCODES_GROUP(G4);

		ADD_OPCODE(LWZ,		(RT(), RS(), imm_s16()));
		ADD_OPCODE(LWZU,	(RT(), RS(), imm_s16()));
		ADD_OPCODE(LBZ,		(RT(), RS(), imm_s16()));
		ADD_OPCODE(LBZU,	(RT(), RS(), imm_s16()));
		ADD_OPCODE(STW,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(STWU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(STB,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(STBU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(LHZ,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(LHZU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(LHA,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(LHAU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(STH,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(STHU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(LMW,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(LFS,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(LFSU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(LFD,		(RS(), RT(), imm_s16()));
		ADD_OPCODE(LFDU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(STFS,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(STFSU,	(RS(), RT(), imm_s16()));
		ADD_OPCODE(STFD,	(RS(), RT(), imm_s16()));
		//ADD_OPCODE(LD,	(RS(), RT(), imm_s16()));
		//ADD_OPCODE(FDIVS,	(RS(), RT(), RD()));

		START_OPCODES_GROUP(G_0x3a, FUNC)
			//ADD_OPCODE(LDU,	(RS(), RT(), imm_s16()));
			//ADD_OPCODE(LWA,	(RS(), RT(), imm_s16()));
			//ADD_OPCODE(LD,	(RS(), RT(), imm_s16()));
		END_OPCODES_GROUP(G_0x3a);

		START_OPCODES_GROUP(G4_S, FUNC)
			ADD_OPCODE(FSUBS,	(RS(), RT(), RD()));
			ADD_OPCODE(FSUBS_,	(RS(), RT(), RD()));
			ADD_OPCODE(FADDS,	(RS(), RT(), RD()));
			ADD_OPCODE(FSQRTS,	(RS(), RD()));
			ADD_OPCODE(FRES,	(RS(), RD()));
			ADD_OPCODE(FMULS,	(RS(), RT(), SA()));
			ADD_OPCODE(FMULS_,	(RS(), RT(), SA()));
			ADD_OPCODE(FMADDS,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMADDS_,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMSUBS,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMSUBS_,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMSUBS,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMSUBS_,(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMADDS,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMADDS_,(RS(), RT(), SA(), RD()));
		END_OPCODES_GROUP(G4_S);

		ADD_OPCODE(STD,		(RS(), RT(), imm_s16()));

		START_OPCODES_GROUP(G5, FUNC)
			START_OPCODES_GROUP(G5_G0x0, SA)
				ADD_OPCODE(FCMPU,	(RS(), RT(), RD()));
				ADD_OPCODE(FCMPO,	(RS(), RT(), RD()));
				ADD_OPCODE(MCRFS,	(RS(), RT()));
			END_OPCODES_GROUP(G5_G0x0);

			START_OPCODES_GROUP(G5_G0x10, SA)
				ADD_OPCODE(FNEG,	(RS(), RD()));
				ADD_OPCODE(FMR,	(RS(), RD()));
				ADD_OPCODE(FNABS,	(RS(), RD()));
				ADD_OPCODE(FABS,	(RS(), RD()));
			END_OPCODES_GROUP(G5_G0x10);
			
			START_OPCODES_GROUP(G5_G0x1c, SA)
				ADD_OPCODE(FCTIW,	(RS(), RD()));
				ADD_OPCODE(FCTID,	(RS(), RD()));
				ADD_OPCODE(FCFID,	(RS(), RD()));
			END_OPCODES_GROUP(G5_G0x1c);

			START_OPCODES_GROUP(G5_G0x1e, SA)
				ADD_OPCODE(FCTIWZ,	(RS(), RD()));
				ADD_OPCODE(FCTIDZ,	(RS(), RD()));
			END_OPCODES_GROUP(G5_G0x1e);

			//ADD_OPCODE(MFFS,	(RS(), RT(), RD()));
			ADD_OPCODE(FDIV,	(RS(), RT(), RD()));
			ADD_OPCODE(FDIV_,	(RS(), RT(), RD()));
			ADD_OPCODE(FSUB,	(RS(), RT(), RD()));
			ADD_OPCODE(FSUB_,	(RS(), RT(), RD()));
			ADD_OPCODE(FADD,	(RS(), RT(), RD()));
			ADD_OPCODE(FADD_,	(RS(), RT(), RD()));
			ADD_OPCODE(FSQRT,	(RS(), RD()));
			ADD_OPCODE(FSEL,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FSEL_,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMUL,	(RS(), RT(), SA()));
			ADD_OPCODE(FMUL_,	(RS(), RT(), SA()));
			ADD_OPCODE(FRSQRTE,	(RS(), RD()));
			ADD_OPCODE(FMSUB,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMSUB_,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FMADD,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMSUB,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMSUB_,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMADD,	(RS(), RT(), SA(), RD()));
			ADD_OPCODE(FNMADD_,	(RS(), RT(), SA(), RD()));
		END_OPCODES_GROUP(G5);

		default: m_op.UNK(code, opcode, RS(), RT(), RD(), SA(), FUNC(), imm_s16(), imm_u16(), imm_u26()); break;
		};
	}
};

#undef START_OPCODES_GROUP
#undef ADD_OPCODE
#undef ADD_NULL_OPCODE
#undef END_OPCODES_GROUP