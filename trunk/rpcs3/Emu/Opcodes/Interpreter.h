#include "Emu/Opcodes/Opcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(x)

static const u32 cmp_masks[8] =
{
	0xfffffff0,
	0xffffff0f,
	0xfffff0ff,
	0xffff0fff,
	0xfff0ffff,
	0xff0fffff,
	0xf0ffffff,
	0x0fffffff,
};

static const s32 ext_zero_masks[5] =
{
	0x00000000,
	0x000000ff,
	0x0000ffff,
	0x00ffffff,
	0xffffffff,
};

class InterpreterOpcodes 
	: public Opcodes
	, public SysCalls
{
private:
	//Display* display;
	CPUThread& CPU;

public:
	InterpreterOpcodes(CPUThread& cpu)
		: CPU(cpu)
		, SysCalls(cpu)
	{
		//display = new Display();
	}

private:
	virtual void Exit()
	{
		//delete display;
	}

	virtual void Step()
	{
		/*
		static const u64 max_cycle = (PS3_CLK * 1000000ULL) / (1000 * 60);

		if(++CPU.cycle >= max_cycle)
		{
			display->Flip();
			CPU.cycle = 0;
		}
		*/
	}

	virtual void SysCall()
	{
		CPU.GPR[3] = DoSyscall(CPU.GPR[11]);
		ConLog.Write("SysCall[%d] done with code [%d]!", CPU.GPR[11], CPU.GPR[3]);
	}

	virtual void NOP()
	{
		//__asm nop
	}

	void Compare(const u32 bf, const u32 a, const u32 b)
	{
		u32 c;
		if		(a < b) c = 0x8;
		else if (a > b) c = 0x4;
		else			c = 0x2;

		if(CPU.IsXER_SO()) c |= 0x1;

		const u8 cr = 7 - (bf >> 2);
		CPU.CR &= cmp_masks[cr];
		CPU.CR |= c << (cr * 4);
	}

	s32& GetRegBySPR(OP_REG spr)
	{
		const u32 n = (spr & 0x1f) + ((spr >> 5) & 0x1f);

		switch(n)
		{
		case 1: return CPU.XER;
		case 8: return CPU.LR;
		case 9: return CPU.CTR;
		default: break;
		}

		UNK(wxString::Format("GetRegBySPR error: Unknown spr %d! #pc: 0x%x", n, CPU.PC));
		return CPU.XER;
	}

	virtual void MFSPR(OP_REG rd, OP_REG spr)
	{
		CPU.GPR[rd] = GetRegBySPR(spr);
	}

	virtual void CMPLI(OP_REG bf, OP_REG l, OP_REG ra, OP_uIMM uimm16)
	{
		Compare(bf, CPU.GPR[ra], uimm16);
	}
	virtual void CMPI(OP_REG bf, OP_REG l, OP_REG ra, OP_sIMM simm16)
	{
		Compare(bf, CPU.GPR[ra], simm16);
	}
	virtual void ADDIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const s64 _ra = CPU.GPR[ra];
		CPU.GPR[rt] = CPU.GPR[ra] + simm16;
		CPU.UpdateXER_CA(CPU.GPR[rt] < _ra);
	}
	virtual void ADDIC_(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const s64 _ra = CPU.GPR[ra];
		CPU.GPR[rt] = CPU.GPR[ra] + simm16;
		CPU.UpdateXER_CA(CPU.GPR[rt] < _ra);
		CPU.UpdateCR0(CPU.GPR[rt]);
	}
	virtual void ADDI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra != 0 ? CPU.GPR[ra] + simm16 : simm16;
	}
	virtual void ADDIS(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra != 0 ? CPU.GPR[ra] + (simm16 << 16) : (simm16 << 16);
	}
	virtual void BC(OP_REG bf, OP_REG bi, OP_REG bd, OP_REG aa, OP_REG lk)
	{
		bool b2 = (bf & 2) > 0;
		bool b4 = (bf & 4) > 0;
		bool b8 = (bf & 8) > 0;
		bool cr = (CPU.CR & (1 << (31 - bi))) > 0;

		if(!b4) CPU.CTR--;

		if ((b4 || (CPU.CTR != 0) ^ b2) && ((bf & 16) || !(cr ^ b8)))
		{
			if(lk) CPU.LR = CPU.PC + 4;
			CPU.SetBranch(aa == 1 ? CPU.PC + bd : bd);
		}
	}
	virtual void SC(const s32 sc_code)
	{
		switch(sc_code)
		{
		case 0x1: UNK(wxString::Format("HyperCall %d", CPU.GPR[0])); break;
		case 0x2: SysCall(); break;
		case 0x22: UNK("HyperCall LV1"); break;
		default: UNK(wxString::Format("Unknown sc: %x", sc_code));
		}
	}
	virtual void B(OP_sIMM ll, OP_REG aa, OP_REG lk)
	{
		if(lk) CPU.LR = CPU.PC + 4;
		CPU.SetBranch(branchTarget(aa ? 0 : CPU.PC, ll));
	}
	
	START_OPCODES_GROUP(G_13)
		virtual void BCLR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			bool b2 = (bo & 2) > 0;
			bool b4 = (bo & 4) > 0;
			bool b8 = (bo & 8) > 0;
			bool cr = (CPU.CR & (1 << (31 - bi))) > 0;

			if(!b4) CPU.CTR--;

			if ((b4 || (CPU.CTR != 0) ^ b2) && ((bo & 16) || !(cr ^ b8)))
			{
				if(lk) CPU.LR = CPU.PC + 4;
				CPU.SetBranch(bh);
			}
		}
		virtual void BLR()
		{
			CPU.SetBranch(CPU.LR);
		}
		virtual void CRNOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u32 t = 1 << (31 - bt);
			const u32 a = 1 << (31 - ba);
			const u32 b = 1 << (31 - bb);

			if(!(CPU.CR & (a | b)))
			{
				CPU.CR |= t;
			}
			else
			{
				CPU.CR &= ~t;
			}
		}
		virtual void CRANDC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("crandc");
		}
		virtual void CRXOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("crxor");
		}
		virtual void CRNAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("crnand");
		}
		virtual void CRAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("crand");
		}
		virtual void CREQV(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("creqv");
		}
		virtual void CROR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("cror");
		}
		virtual void CRORC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			UNK("crorc");
		}
		virtual void BCCTR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			switch(lk)
			{
				case 0: UNK("bcctr"); break;
				case 1: UNK("bcctrl"); break;
			}
		}
		virtual void BCTR()
		{
			ConLog.SkipLn();
			ConLog.Warning("bctr 0x%x #pc: 0x%x", CPU.CTR, CPU.PC);
			ConLog.SkipLn();

			CPU.SetBranch(CPU.CTR);
		}
	END_OPCODES_GROUP(G_13);
	
	virtual void ORI(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		if(rs == 0 && ra == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		CPU.GPR[rs] = CPU.GPR[ra] | uimm16;
	}
	virtual void ORIS(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		if(rs == 0 && ra == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		CPU.GPR[rs] = CPU.GPR[ra] | (uimm16 << 16);
	}
	virtual void ANDI_(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] & uimm16;
		CPU.UpdateCR0(CPU.GPR[ra]);
	}
	virtual void ANDIS_(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] & (uimm16 << 16);
		CPU.UpdateCR0(CPU.GPR[ra]);
	}

	START_OPCODES_GROUP(G_1e)
		virtual void CLRLDI(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
		{
			if(CPU.GPR[ra] != 0 || CPU.GPR[rs] != 0)
			{
				UNK(wxString::Format("clrldi r%d,r%d,%d", rs, ra, uimm16));
			}
			return;

			if(uimm16 != 32 && uimm16 != 24 && uimm16 != 16 && uimm16 != 8)
			{
				UNK(wxString::Format("clrldi r%d,r%d,%d", rs, ra, uimm16));
			}

			const s64 _rs = CPU.GPR[ra] & ext_zero_masks[uimm16 / 8];
			
			ConLog.Warning("from: %x", (u32)_rs);
			CPU.GPR[rs] = 
				(_rs & 0x0000000f) << 28 |
				(_rs & 0x000000f0) << 24 |
				(_rs & 0x00000f00) << 20 |
				(_rs & 0x0000f000) << 16 |
				(_rs & 0x000f0000) >> 16 |
				(_rs & 0x00f00000) >> 20 |
				(_rs & 0x0f000000) >> 24 |
				(_rs & 0xf0000000) >> 28;

			ConLog.Warning("to: %x", (u32)CPU.GPR[rs]);

		}
	END_OPCODES_GROUP(G_1e);
	
	START_OPCODES_GROUP(G_1f)
		virtual void CMP(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			Compare(bf, CPU.GPR[ra], CPU.GPR[rb]);
		}
		virtual void ADDC(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[ra], CPU.GPR[rb]));
			CPU.UpdateXER_CA(CPU.IsCarryGen(CPU.GPR[ra], CPU.GPR[rb]));
			CPU.GPR[rt] = CPU.GPR[ra] + CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void CNTLZW(OP_REG rs, OP_REG ra, bool rc)
		{
			for(u32 i=0, m=0x80000000; i<32; m >>= 1)
			{
				if(m & CPU.GPR[rs] || ++i == 32)
				{
					CPU.GPR[ra] = i;
					if(rc) CPU.UpdateCR0(CPU.GPR[i]);
					break;
				}
			}
		}
		virtual void AND(OP_REG rs, OP_REG ra, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] & CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void CMPL(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb, bool rc)
		{
			if(rc) UNK("cmpl.");
			Compare(bf, (s32)CPU.GPR[ra], (s32)CPU.GPR[rb]);
		}
		virtual void CMPLD(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb, bool rc)
		{
			if(rc) UNK("cmpld.");
			Compare(bf, CPU.GPR[ra], CPU.GPR[rb]);
		}
		virtual void DCBST(OP_REG ra, OP_REG rb)
		{
			UNK("dcbst");
		}
		virtual void CNTLZD(OP_REG ra, OP_REG rs, bool rc)
		{
			UNK("cntlzd");
		}
		virtual void ANDC(OP_REG rs, OP_REG ra, OP_REG rb, bool rc)
		{
			CPU.UpdateXER_CA(CPU.IsCarryGen(CPU.GPR[rs], CPU.GPR[rb]));
			CPU.GPR[ra] = CPU.GPR[rs] & CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void DCBF(OP_REG ra, OP_REG rb)
		{
			UNK("dcbf");
		}
		virtual void ADDE(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[ra], CPU.GPR[rb]));
			CPU.UpdateXER_CA(CPU.GPR[ra], CPU.GPR[rb], CPU.IsXER_CA());
			CPU.GPR[rt] = CPU.GPR[ra] + CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void ADDZE(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			const s32 a = CPU.GPR[ra];
			const s32 c = CPU.IsXER_CA();
			CPU.GPR[ra] = a + c;
			CPU.UpdateXER_CA(a == 0xffffffff && c);
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(a, c));
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void ADDME(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			switch(oe)
			{
				case 0: UNK("addme");  break;
				case 1: UNK("addmeo"); break;
			}
		}
		virtual void DCBTST(OP_REG th, OP_REG ra, OP_REG rb)
		{
			UNK("dcbtst");
		}
		virtual void ADD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[ra], CPU.GPR[rb]));
			CPU.GPR[rt] = CPU.GPR[ra] + CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void DIV(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(CPU.GPR[rb] == 0) return;
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[ra], CPU.GPR[rb]));
			CPU.GPR[rt] = CPU.GPR[ra] / CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void MFLR(OP_REG rt)
		{
			CPU.GPR[rt] = CPU.LR;
		}
		virtual void ABS(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			if(oe) CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[rt], CPU.GPR[ra]));
			CPU.GPR[rt] = abs(CPU.GPR[ra]);
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void MR(OP_REG ra, OP_REG rb)
		{
			CPU.GPR[ra] = CPU.GPR[rb];
		}

		START_OPCODES_GROUP(G_1f_1d3)
			virtual void MTLR(OP_REG rt)
			{
				CPU.LR = CPU.GPR[rt];
			}
			virtual void MTCTR(OP_REG rt)
			{
				CPU.CTR = CPU.GPR[rt];
			}
		END_OPCODES_GROUP(G_1f_1d3);

		virtual void DCBI(OP_REG ra, OP_REG rb)
		{
			UNK("dcbi");
		}
		virtual void EXTSW(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s32)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void DCLST(OP_REG ra, OP_REG rb, bool rc)
		{
			UNK("dclst");
		}
		virtual void DCBZ(OP_REG ra, OP_REG rb)
		{
			UNK("dcbz");
		}
	END_OPCODES_GROUP(G_1f);
	
	virtual void LWZ(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		ConLog.SkipLn();
		ConLog.Warning("lwz r%d,r%d,%x!", rt, ra, ds);
		ConLog.Warning("r0 = %x", CPU.GPR[rt]);
		ConLog.Warning("r1 = %x", CPU.GPR[ra]);
		ConLog.Warning("Read32(0x%x)", ra != 0 ? CPU.GPR[ra] + ds : ds);
		CPU.GPR[rt] = Memory.Read32(ra != 0 ? CPU.GPR[ra] + ds : ds);
		ConLog.Warning("Result: 0x%x", CPU.GPR[rt]);
		ConLog.Warning("pc: %x", CPU.PC);
		ConLog.SkipLn();
	}
	virtual void STH(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		ConLog.SkipLn();
		ConLog.Warning("sth r%d,r%d,%x!", rs, ra, ds);
		ConLog.Warning("r0 = %x", CPU.GPR[rs]);
		ConLog.Warning("r1 = %x", CPU.GPR[ra]);
		ConLog.Warning("Write16(0x%x)", ra!=0 ? CPU.GPR[ra] + ds : ds);
		ConLog.Warning("Value: 0x%x", CPU.GPR[rs]);
		ConLog.Warning("pc: %x", CPU.PC);
		ConLog.SkipLn();

		Memory.Write16(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
	}
	virtual void STW(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		ConLog.SkipLn();
		ConLog.Warning("stw r%d,r%d,%x!", rs, ra, ds);
		ConLog.Warning("r0 = %x", CPU.GPR[rs]);
		ConLog.Warning("r1 = %x", CPU.GPR[ra]);
		ConLog.Warning("Write32(0x%x)", ra!=0 ? CPU.GPR[ra] + ds : ds);
		ConLog.Warning("Value: 0x%x", CPU.GPR[rs]);
		ConLog.Warning("pc: %x", CPU.PC);
		ConLog.SkipLn();

		Memory.Write32(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
	}
	
	START_OPCODES_GROUP(G_3a)
		virtual void LD(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			ConLog.SkipLn();
			ConLog.Warning("ld r%d,r%d,%x!", rt, ra, ds);
			ConLog.Warning("Read64(0x%x)", ra != 0 ? CPU.GPR[ra] + ds : ds);
			CPU.GPR[rt] = Memory.Read64(ra != 0 ? CPU.GPR[ra] + ds : ds);
			ConLog.Warning("Result: 0x%x", CPU.GPR[rt]);
			ConLog.Warning("pc: %x", CPU.PC);
			ConLog.SkipLn();
		}
		virtual void LDU(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rt == ra) return;
			ConLog.SkipLn();
			ConLog.Warning("ld r%d,r%d,%x!", rt, ra, ds);
			ConLog.Warning("Read64(0x%x)", CPU.GPR[ra] + ds);
			CPU.GPR[rt] = Memory.Read64(CPU.GPR[ra] + ds);
			ConLog.Warning("Result: 0x%x", CPU.GPR[rt]);
			ConLog.Warning("pc: %x", CPU.PC);
			ConLog.SkipLn();

			CPU.GPR[ra] = CPU.GPR[ra] + ds;
		}
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G4_S)
		virtual void FSUBS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] - CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FADDS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] + CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FSQRTS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = sqrt(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FRES(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb] == 0) return;
			CPU.FPR[frt] = 1.0f/CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMULS(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] * CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FNMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FNMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
	END_OPCODES_GROUP(G4_S);
	
	START_OPCODES_GROUP(G_3a)
		virtual void STD(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			ConLog.SkipLn();
			ConLog.Warning("std r%d,r%d,%x!", rs, ra, ds);
			ConLog.Warning("r0 = %x", CPU.GPR[rs]);
			ConLog.Warning("r1 = %x", CPU.GPR[ra]);
			ConLog.Warning("Write64(0x%x, %d)", ra!=0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
			ConLog.Warning("pc: %x", CPU.PC);
			ConLog.SkipLn();

			Memory.Write64(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
		}
		virtual void STDU(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rs == ra) return;

			ConLog.SkipLn();
			ConLog.Warning("stdu r%d,r%d,%x!", rs, ra, ds);
			ConLog.Warning("r0 = %x", CPU.GPR[rs]);
			ConLog.Warning("r1 = %x", CPU.GPR[ra]);
			ConLog.Warning("Write64(0x%x, %d)", CPU.GPR[ra] + ds, CPU.GPR[rs]);
			ConLog.Warning("pc: %x", CPU.PC);
			ConLog.SkipLn();

			Memory.Write64(CPU.GPR[ra] + ds, CPU.GPR[rs]);
			CPU.GPR[ra] = CPU.GPR[ra] + ds;
		}
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G5)
		virtual void MTFSB1(OP_REG bt, bool rc)
		{
			UNK("mtfsb1");
		}
		virtual void MCRFS(OP_REG bf, OP_REG bfa)
		{
			UNK("mcrfs");
		}
		virtual void MTFSB0(OP_REG bt, bool rc)
		{
			UNK("mtfsb0");
		}
		virtual void MTFSFI(OP_REG bf, OP_REG i, bool rc)
		{
			UNK("mtfsfi");
		}
		virtual void MFFS(OP_REG frt, bool rc)
		{
			UNK("mffs");
		}
		virtual void MTFSF(OP_REG flm, OP_REG frb, bool rc)
		{
			UNK("mtfsf");
		}
		virtual void FCMPU(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			UNK("fcmpu");
		}
		virtual void FRSP(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("frsp");
		}
		virtual void FCTIW(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("fctiw");
		}
		virtual void FCTIWZ(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("fctiwz");
		}
		virtual void FDIV(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb] == 0) return;
			CPU.FPR[frt] = CPU.FPR[fra] * CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FSUB(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] - CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FADD(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] + CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FSQRT(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = sqrt(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FSEL(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] < 0.0f ? CPU.FPR[frb] : CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMUL(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] * CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FRSQRTE(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("frsqrte");
		}
		virtual void FMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FNMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FNMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FCMPO(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			UNK("fcmpo");
		}
		virtual void FNEG(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = -CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FMR(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frb];
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FNABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = -abs(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = abs(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR0(CPU.FPR[frt]);
		}
		virtual void FCTID(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("fctid");
		}
		virtual void FCTIDZ(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("fctidz");
		}
		virtual void FCFID(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("fcfid");
		}
	END_OPCODES_GROUP(G5);

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		if(Memory.MemFlags.IsFlag(code))
		{
			ConLog.Warning("Flag: %x", code);
			CPU.SetBranch(code);
			return;
		}

		switch(code)
		{
		case 0x1573dc3f: ConLog.Warning("lwmutex lock #pc: 0x%x", CPU.PC); break;
		case 0x1bc200f4: ConLog.Warning("lwmutex unlock #pc: 0x%x", CPU.PC); break;
		case 0xe6f2c1e7: ConLog.Warning("process exit #pc: 0x%x", CPU.PC); break;

		default:
			UNK(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
		break;
		}
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));

		Emu.Pause();

		for(uint i=0; i<32; ++i) ConLog.Write("r%d = 0x%x", i, CPU.GPR[i]);
		for(uint i=0; i<32; ++i) ConLog.Write("f%d = %f", i, CPU.FPR[i]);

		ConLog.Write("LR = 0x%x", CPU.LR);
		ConLog.Write("CTR = 0x%x", CPU.CTR);
		ConLog.Write("XER = 0x%x", CPU.XER);
		ConLog.Write("CR = 0x%x", CPU.CR);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP