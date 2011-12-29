#pragma once

#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/Cell/DisAsm.h"
#include "Emu/Cell/PPCThread.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

class PPU_DisAsm
	: public PPU_Opcodes
	, public DisAsm
{
public:
	PPCThread& CPU;

	PPU_DisAsm()
		: DisAsm(*(PPCThread*)NULL, DumpMode)
		, CPU(*(PPCThread*)NULL)
	{
	}

	PPU_DisAsm(PPCThread& cpu, DisAsmModes mode = NormalMode)
		: DisAsm(cpu, mode)
		, CPU(cpu)
	{
	}

	~PPU_DisAsm()
	{
	}

private:
	virtual void Exit()
	{
		if(m_mode == NormalMode && !disasm_frame->exit)
		{
			disasm_frame->Close();
		}

		this->~PPU_DisAsm();
	}

	virtual void Step()
	{
	}

	virtual u32 DisAsmBranchTarget(const s32 imm)
	{
		if(m_mode != NormalMode) return branchTarget(dump_pc, imm);
		return branchTarget(CPU.PC, imm);
	}
	
private:
	virtual void NOP()
	{
		Write( "nop" );
	}

	START_OPCODES_GROUP(G_04)
		virtual void VXOR(OP_REG vrd, OP_REG vra, OP_REG vrb)
		{
			DisAsm_V3("vxor", vrd, vra, vrb);
		}
	END_OPCODES_GROUP(G_04);
	
	virtual void MULLI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		DisAsm_R2_IMM("mulli", rt, ra, simm16);
	}
	virtual void SUBFIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		DisAsm_R2_IMM("subfic", rt, ra, simm16);
	}
	virtual void CMPLI(OP_REG bf, OP_REG l, OP_REG ra, OP_uIMM uimm16)
	{
		DisAsm_CR1_R1_IMM(wxString::Format("cmpl%si", l ? "d" : "w"), bf, ra, uimm16);
	}
	virtual void CMPI(OP_REG bf, OP_REG l, OP_REG ra, OP_sIMM simm16)
	{
		DisAsm_CR1_R1_IMM(wxString::Format("cmp%si", l ? "d" : "w"), bf, ra, simm16);
	}
	virtual void ADDIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		DisAsm_R2_IMM("addic", rt, ra, simm16);
	}
	virtual void ADDIC_(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		DisAsm_R2_IMM("addic.", rt, ra, simm16);
	}
	virtual void ADDI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		if(ra == 0)
		{
			DisAsm_R1_IMM("li", rt, simm16);
		}
		else
		{
			DisAsm_R2_IMM("addi", rt, ra, simm16);
		}
	}
	virtual void ADDIS(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		if(ra == 0)
		{
			DisAsm_R1_IMM("lis", rt, simm16);
		}
		else
		{
			DisAsm_R2_IMM("addis", rt, ra, simm16);
		}
	}

	virtual void BC(OP_REG bo, OP_REG bi, OP_sIMM bd, OP_REG aa, OP_REG lk)
	{
		const u8 bo0 = (bo & 0x10) ? 1 : 0;
		const u8 bo1 = (bo & 0x08) ? 1 : 0;
		const u8 bo2 = (bo & 0x04) ? 1 : 0;
		const u8 bo3 = (bo & 0x02) ? 1 : 0;

		if(bo0)
		{
			if(bo1)
			{
				DisAsm_CR_BRANCH("bdnz+", bi, bd);
			}
			else
			{
				DisAsm_CR_BRANCH("bdnz", bi, bd);
			}
		}
		else if(bo2)
		{
			if(bo1)
			{
				if(bo3)
				{
					if(bd <= 0) //check me
					{
						switch(bi % 4)
						{
						case 0x0: DisAsm_CR_BRANCH("blt-", bi, bd); return;
						case 0x1: DisAsm_CR_BRANCH("bgt-", bi, bd); return;
						case 0x2: DisAsm_CR_BRANCH("beq-", bi, bd); return;
						}
					}
					else
					{
						switch(bi % 4)
						{
						case 0x0: DisAsm_CR_BRANCH("blt+", bi, bd); return;
						case 0x1: DisAsm_CR_BRANCH("bgt+", bi, bd); return;
						case 0x2: DisAsm_CR_BRANCH("beq+", bi, bd); return;
						}
					}
				}
				else
				{
					switch(bi % 4)
					{
					case 0x0: DisAsm_CR_BRANCH("blt", bi, bd); return;
					case 0x1: DisAsm_CR_BRANCH("bgt", bi, bd); return;
					case 0x2: DisAsm_CR_BRANCH("beq", bi, bd); return;
					}
				}
				
			}
			else
			{
				if(bo3)
				{
					if(bd <= 0)
					{
						switch(bi % 4)
						{
						case 0x0: DisAsm_CR_BRANCH("bge-", bi, bd); return;
						case 0x1: DisAsm_CR_BRANCH("ble-", bi, bd); return;
						case 0x2: DisAsm_CR_BRANCH("bne-", bi, bd); return;
						}
					}
					else
					{
						switch(bi % 4)
						{
						case 0x0: DisAsm_CR_BRANCH("bge+", bi, bd); return;
						case 0x1: DisAsm_CR_BRANCH("ble+", bi, bd); return;
						case 0x2: DisAsm_CR_BRANCH("bne+", bi, bd); return;
						}
					}
				}
				else
				{
					switch(bi % 4)
					{
					case 0x0: DisAsm_CR_BRANCH("bge", bi, bd); return;
					case 0x1: DisAsm_CR_BRANCH("ble", bi, bd); return;
					case 0x2: DisAsm_CR_BRANCH("bne", bi, bd); return;
					}
				}
			}
		}

		Write(wxString::Format("bc [%x:%x:%x:%x], cr%d[%x], 0x%x, %d, %d", bo0, bo1, bo2, bo3, bi/4, bi%4, bd, aa, lk));
	}
	virtual void SC(const s32 sc_code)
	{
		switch(sc_code)
		{
		case 0x1: Write("HyperCall"); break;
		case 0x2: Write("SysCall"); break;
		case 0x22: Write("HyperCall LV1"); break;
		default: Write(wxString::Format("Unknown sc: %x", sc_code));
		}
	}
	virtual void B(OP_sIMM ll, OP_REG aa, OP_REG lk)
	{
		switch(lk)
		{
			case 0:
				switch(aa)
				{
					case 0:	DisAsm_BRANCH("b", ll);		break;
					case 1:	DisAsm_BRANCH("ba", ll);	break;
				}
			break;
			
			case 1:
				switch(aa)
				{
					case 0: DisAsm_BRANCH("bl", ll);	break;
					case 1: DisAsm_BRANCH("bla", ll);	break;
				}
			break;
		}
	}
	
	START_OPCODES_GROUP(G_13)
		virtual void BCLR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			const u8 bo0 = (bo & 0x10) ? 1 : 0;
			const u8 bo1 = (bo & 0x08) ? 1 : 0;
			const u8 bo2 = (bo & 0x04) ? 1 : 0;
			const u8 bo3 = (bo & 0x02) ? 1 : 0;

			if(bo0 && !bo1 && bo2 && !bo3) {Write("blr"); return;}
			Write(wxString::Format("bclr [%x:%x:%x:%x], cr%d[%x], %d, %d", bo0, bo1, bo2, bo3, bi/4, bi%4, bh, lk));
		}
		virtual void CRNOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crnor", bt, ba, bb);
		}
		virtual void CRANDC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crandc", bt, ba, bb);
		}
		virtual void CRXOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crxor", bt, ba, bb);
		}
		virtual void CRNAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crnand", bt, ba, bb);
		}
		virtual void CRAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crand", bt, ba, bb);
		}
		virtual void CREQV(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("creqv", bt, ba, bb);
		}
		virtual void CROR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("cror", bt, ba, bb);
		}
		virtual void CRORC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			DisAsm_INT3("crorc", bt, ba, bb);
		}
		virtual void BCCTR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			switch(lk)
			{
				case 0: DisAsm_INT3("bcctr", bo, bi, bh); break;
				case 1: DisAsm_INT3("bcctrl", bo, bi, bh); break;
			}
		}
		virtual void BCTR()
		{
			Write("bctr");
		}
		virtual void BCTRL()
		{
			Write("bctrl");
		}
	END_OPCODES_GROUP(G_13);

	virtual void RLWINM(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, OP_REG me, bool rc)
	{
		DisAsm_R2_INT3_RC("rlwinm", ra, rs, sh, mb, me, rc);
	}
	
	virtual void ORI(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		if(rs == 0 && ra == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		DisAsm_R2_IMM("ori", rs, ra, uimm16);
	}
	virtual void ORIS(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		if(rs == 0 && ra == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		DisAsm_R2_IMM("oris", rs, ra, uimm16);
	}
	virtual void XORI(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		DisAsm_R2_IMM("xori", rs, ra, uimm16);
	}
	virtual void XORIS(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		DisAsm_R2_IMM("xoris", rs, ra, uimm16);
	}
	virtual void ANDI_(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		DisAsm_R2_IMM("andi.", ra, rs, uimm16);
	}
	virtual void ANDIS_(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		DisAsm_R2_IMM("andis.", ra, rs, uimm16);
	}

	START_OPCODES_GROUP(G_1e)
		virtual void RLDICL(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			if(sh == 0)
			{
				DisAsm_R2_INT1_RC("clrldi", ra, rs, mb, rc);
			}
			else if(mb == 0)
			{
				DisAsm_R2_INT1_RC("rotldi", ra, rs, sh, rc);
			}
			else if(mb == 64 - sh)
			{
				DisAsm_R2_INT1_RC("srdi", ra, rs, mb, rc);
			}
			else
			{
				DisAsm_R2_INT2_RC("rldicl", ra, rs, sh, mb, rc);
			}
		}
		virtual void RLDICR(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG me, bool rc)
		{
			DisAsm_R2_INT2_RC("rldicr", ra, rs, sh, me, rc);
		}
		virtual void RLDIC(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			DisAsm_R2_INT2_RC("rldic", ra, rs, sh, mb, rc);
		}
		virtual void RLDIMI(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			DisAsm_R2_INT2_RC("rldimi", ra, rs, sh, mb, rc);
		}
	END_OPCODES_GROUP(G_1e);
	
	START_OPCODES_GROUP(G_1f)
		virtual void CMP(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			DisAsm_CR1_R2(wxString::Format("cmp%s", l ? "d" : "w"), bf, ra, rb);
		}
		virtual void ADDC(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("addc", ra, rt, rb, oe, rc);
		}
		
		START_OPCODES_GROUP(G_1f_13)
			virtual void MFCR(OP_REG rt)
			{
				DisAsm_R1("mfcr", rt);
			}
		END_OPCODES_GROUP(G_1f_13);

		virtual void LWARX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			DisAsm_R3("lwarx", ra, rs, rb);
		}
		virtual void LDX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			DisAsm_R3("ldx", ra, rs, rb);
		}
		virtual void LWZX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			DisAsm_R3("lwzx", ra, rs, rb);
		}
		virtual void SLW(OP_REG ra, OP_REG rs, OP_REG rb, OP_REG a, bool rc)
		{
			if(a)
			{
				DisAsm_R3_RC("srw", ra, rs, rb, rc);
			}
			else
			{
				DisAsm_R3_RC("slw", ra, rs, rb, rc);
			}
		}
		virtual void CNTLZW(OP_REG rs, OP_REG ra, bool rc)
		{
			DisAsm_R2_RC("cntlzw", ra, rs, rc);
		}
		virtual void AND(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			DisAsm_R3_RC("and", ra, rs, rb, rc);
		}
		virtual void CMPL(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			DisAsm_CR1_R2(wxString::Format("cmpl%s", l ? "d" : "w"), bf, ra, rb);
		}
		virtual void CMPLD(OP_REG bf, OP_REG ra, OP_REG rb)
		{
			DisAsm_CR1_R2("cmpld", bf, ra, rb);
		}
		virtual void SUBF(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			 DisAsm_R3_OE_RC("subf", rt, ra, rb, oe, rc);
		}
		virtual void DCBST(OP_REG ra, OP_REG rb)
		{
			DisAsm_R2("dcbst", ra, rb);
		}
		virtual void CNTLZD(OP_REG ra, OP_REG rs, bool rc)
		{
			DisAsm_R2_RC("cntlzd", ra, rs, rc);
		}
		virtual void ANDC(OP_REG rs, OP_REG ra, OP_REG rb, bool rc)
		{
			DisAsm_R3_RC("andc", rs, ra, rb, rc);
		}
		virtual void DCBF(OP_REG ra, OP_REG rb)
		{
			DisAsm_R2("dcbf", ra, rb);
		}
		virtual void LVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			DisAsm_V1_R2("lvx", vrd, ra, rb);
		}
		virtual void NEG(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			DisAsm_R2_OE_RC("neg", rt, ra, oe, rc);
		}
		virtual void NOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			if(rs == rb)
			{
				DisAsm_R2_RC("not", ra, rs, rc);
			}
			else
			{
				DisAsm_R3_RC("nor", ra, rs, rb, rc);
			}
		}
		virtual void ADDE(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("adde", rt, ra, rb, oe, rc);
		}
		virtual void MTOCRF(OP_REG fxm, OP_REG rs)
		{
			DisAsm_INT1_R1("mtocrf", fxm, rs);
		}
		virtual void STWCX_(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			DisAsm_R3("stwcx.", rs, ra, rb);
		}
		virtual void STWX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			DisAsm_R3("stwx", rs, ra, rb);
		}
		virtual void ADDZE(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			DisAsm_R2_OE_RC("addze", rs, ra, oe, rc);
		}
		virtual void STVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			DisAsm_V1_R2("stvx", vrd, ra, rb);
		}
		virtual void ADDME(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			DisAsm_R2_OE_RC("addme", rs, ra, oe, rc);
		}
		virtual void MULLW(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("mullw", rt, ra, rb, oe, rc);
		}
		virtual void DCBTST(OP_REG th, OP_REG ra, OP_REG rb)
		{
			DisAsm_R3("dcbtst", th, ra, rb);
		}
		virtual void ADD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("add", rt, ra, rb, oe, rc);
		}
		virtual void DCBT(OP_REG ra, OP_REG rb, OP_REG th)
		{
			DisAsm_R2("dcbt", ra, rb);
		}
		virtual void SRAWI(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			DisAsm_R3_RC("srawi", ra, rs, sh, rc);
		}
		virtual void SRADI(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			DisAsm_R3_RC("sradi", ra, rs, sh, rc);
		}
		virtual void XOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			DisAsm_R3_RC("xor", ra, rs, rb, rc);
		}
		virtual void DIV(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("div", rt, ra, rb, oe, rc);
		}
		virtual void MFLR(OP_REG rt)
		{
			DisAsm_R1("mflr", rt);
		}
		virtual void ABS(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			DisAsm_R2_OE_RC("abs", rt, ra, oe, rc);
		}
		virtual void MFTB(OP_REG rt)
		{
			DisAsm_R1("mftb", rt);
		}
		virtual void DIVDU(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("divdu", rt, ra, rb, oe, rc);
		}
		virtual void DIVWU(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			DisAsm_R3_OE_RC("divwu", rt, ra, rb, oe, rc);
		}
		virtual void EXTSH(OP_REG ra, OP_REG rs, bool rc)
		{
			DisAsm_R2_RC("extsh", ra, rs, rc);
		}
		virtual void EXTSB(OP_REG ra, OP_REG rs, bool rc)
		{
			DisAsm_R2_RC("extsb", ra, rs, rc);
		}
		virtual void OR(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			if(rs==rb)
			{
				DisAsm_R2("mr", ra, rb);
			}
			else
			{
				DisAsm_R3("or", ra, rs, rb);
			}
			
		}

		START_OPCODES_GROUP(G_1f_1d3)
			virtual void MTLR(OP_REG rt)
			{
				DisAsm_R1("mtlr", rt);
			}
			virtual void MTCTR(OP_REG rt)
			{
				DisAsm_R1("mtctr", rt);
			}
		END_OPCODES_GROUP(G_1f_1d3);

		virtual void DCBI(OP_REG ra, OP_REG rb)
		{
			DisAsm_R2("dcbi", ra, rb);
		}
		virtual void STFIWX(OP_REG frs, OP_REG ra, OP_REG rb)
		{
			DisAsm_F1_R2("stfiwx", frs, ra, rb);
		}
		virtual void EXTSW(OP_REG ra, OP_REG rs, bool rc)
		{
			DisAsm_R2_RC("extsw", ra, rs, rc);
		}
		virtual void SRW(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			DisAsm_R3_RC("srw", ra, rs, rb, rc);
		}
		virtual void DCLST(OP_REG ra, OP_REG rb, bool rc)
		{
			DisAsm_R2_RC("dclst", ra, rb, rc);
		}
		virtual void DCBZ(OP_REG ra, OP_REG rb)
		{
			DisAsm_R2("dcbz", ra, rb);
		}
	END_OPCODES_GROUP(G_1f);
	
	virtual void LWZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("lwz", rt, ra, d);
	}
	virtual void LWZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_R2_IMM("lwzu", rt, ra, ds);
	}
	virtual void LBZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("lbz", rt, ra, d);
	}
	virtual void LBZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_R2_IMM("lbzu", rt, ra, ds);
	}
	virtual void STW(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("stw", rs, ra, d);
	}
	virtual void STWU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_R2_IMM("stwu", rs, ra, ds);
	}
	virtual void STB(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("stb", rs, ra, d);
	}
	virtual void LHZ(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("lhz", rs, ra, d);
	}
	virtual void LHZU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_R2_IMM("lhzu", rs, ra, ds);
	}
	virtual void STH(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_R2_IMM("sth", rs, ra, d);
	}
	virtual void LFS(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		DisAsm_F1_IMM_R1("lfs", frt, d, ra);
	}
	virtual void LFSU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_F1_IMM_R1("lfsu", frt, ds, ra);
	}
	virtual void LFD(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		DisAsm_F1_IMM_R1("lfd", frt, d, ra);
	}
	virtual void LFDU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		DisAsm_F1_IMM_R1("lfdu", frt, ds, ra);
	}
	virtual void STFS(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_F1_IMM_R1("stfs", frs, d, ra);
	}
	virtual void STFD(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		DisAsm_F1_IMM_R1("stfd", frs, d, ra);
	}
	
	START_OPCODES_GROUP(G_3a)
		virtual void LD(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			DisAsm_R2_IMM("ld", rt, ra, ds);
		}
		virtual void LDU(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			DisAsm_R2_IMM("ldu", rt, ra, ds);
		}
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G_3b)
		virtual void FDIVS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fdivs", frt, fra, frb, rc);
		}
		virtual void FSUBS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fsubs", frt, fra, frb, rc);
		}
		virtual void FADDS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fadds", frt, fra, frb, rc);
		}
		virtual void FSQRTS(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fsqrts", frt, frb, rc);
		}
		virtual void FRES(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fres", frt, frb, rc);
		}
		virtual void FMULS(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			DisAsm_F3_RC("fmuls", frt, fra, frc, rc);
		}
		virtual void FMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fmadds", frt, fra, frb, frc, rc);
		}
		virtual void FMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fmsubs", frt, fra, frb, frc, rc);
		}
		virtual void FNMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fnmsubs", frt, fra, frb, frc, rc);
		}
		virtual void FNMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fnmadds", frt, fra, frb, frc, rc);
		}
	END_OPCODES_GROUP(G_3b);
	
	START_OPCODES_GROUP(G_3e)
		virtual void STD(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			DisAsm_R2_IMM("std", rs, ra, ds);
		}
		virtual void STDU(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			DisAsm_R2_IMM("stdu", rs, ra, ds);
		}
	END_OPCODES_GROUP(G_3e);

	START_OPCODES_GROUP(G_3f)
		virtual void MTFSB1(OP_REG bt, bool rc)
		{
			DisAsm_F1_RC("mtfsb1", bt, rc);
		}
		virtual void MCRFS(OP_REG bf, OP_REG bfa)
		{
			DisAsm_F2("mcrfs", bf, bfa);
		}
		virtual void MTFSB0(OP_REG bt, bool rc)
		{
			DisAsm_F1_RC("mtfsb0", bt, rc);
		}
		virtual void MTFSFI(OP_REG bf, OP_REG i, bool rc)
		{
			DisAsm_F2_RC("mtfsfi", bf, i, rc);
		}
		virtual void MFFS(OP_REG frt, bool rc)
		{
			DisAsm_F1_RC("mffs", frt, rc);
		}
		virtual void MTFSF(OP_REG flm, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("mtfsf", flm, frb, rc);
		}
		virtual void FCMPU(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			DisAsm_CR1_F2("fcmpu", bf, fra, frb);
		}
		virtual void FRSP(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("frsp", frt, frb, rc);
		}
		virtual void FCTIW(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fctiw", frt, frb, rc);
		}
		virtual void FCTIWZ(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fctiwz", frt, frb, rc);
		}
		virtual void FDIV(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fdiv", frt, fra, frb, rc);
		}
		virtual void FSUB(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fsub", frt, fra, frb, rc);
		}
		virtual void FADD(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			DisAsm_F3_RC("fadd", frt, fra, frb, rc);
		}
		virtual void FSQRT(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fsqrt", frt, frb, rc);
		}
		virtual void FSEL(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fsel", frt, fra, frb, frc, rc);
		}
		virtual void FMUL(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			DisAsm_F3_RC("fmul", frt, fra, frc, rc);
		}
		virtual void FRSQRTE(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("frsqrte", frt, frb, rc);
		}
		virtual void FMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fmsub", frt, fra, frb, frc, rc);
		}
		virtual void FMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fmadd", frt, fra, frb, frc, rc);
		}
		virtual void FNMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fnmsub", frt, fra, frb, frc, rc);
		}
		virtual void FNMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			DisAsm_F4_RC("fnmadd", frt, fra, frb, frc, rc);
		}
		virtual void FCMPO(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			DisAsm_F3("fcmpo", bf, fra, frb);
		}
		virtual void FNEG(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fneg", frt, frb, rc);
		}
		virtual void FMR(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fmr", frt, frb, rc);
		}
		virtual void FNABS(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fnabs", frt, frb, rc);
		}
		virtual void FABS(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fabs", frt, frb, rc);
		}
		virtual void FCTID(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fctid", frt, frb, rc);
		}
		virtual void FCTIDZ(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fctidz", frt, frb, rc);
		}
		virtual void FCFID(OP_REG frt, OP_REG frb, bool rc)
		{
			DisAsm_F2_RC("fcfid", frt, frb, rc);
		}
	END_OPCODES_GROUP(G_3f);

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		Write(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP