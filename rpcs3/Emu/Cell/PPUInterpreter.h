#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

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

static u32 rotate_mask[64][64];
void InitRotateMask()
{
	static bool inited = false;
	if(inited) return;

	for(u32 mb=0; mb<32; mb++)
	{
		for(u32 me=0; me<32; me++)
		{
			const u32 mask = ((u32)0xFFFFFFFF >> mb) ^ ((me >= 31) ? 0 : ((u32)0xFFFFFFFF >> (me + 1)));
			rotate_mask[mb][me] = mb > me ? ~mask : mask;
		}
	}

	//CheckMe!
	for( u32 mb=32; mb<64; mb++ )
	{
		for( u32 me=32; me<64; me++ )
		{
			const u64 mask = 
				((u64)0xFFFFFFFFFFFFFFFF >> mb) ^ ((me >= 63) ? 0 : ((u64)0xFFFFFFFFFFFFFFFF >> (me + 1)));
			rotate_mask[mb][me] = mb > me ? ~mask : mask;
		}
	}

	inited = true;
}

class PPU_Interpreter
	: public PPU_Opcodes
	, public SysCalls
{
private:
	PPUThread& CPU;

public:
	PPU_Interpreter(PPUThread& cpu)
		: CPU(cpu)
		, SysCalls(cpu)
	{
		InitRotateMask();
		TestOpcodes();
	}

private:
	virtual void Exit()
	{
	}

	virtual void TestOpcodes()
	{
		static bool tested = false;
		if(tested) return;

		u64 gpr4 = CPU.GPR[4];
		u64 gpr6 = CPU.GPR[6];

		CPU.GPR[4] = 0x90003000;
		RLWINM(6, 4, 2, 0, 0x1D, false);
		if(CPU.GPR[6] != 0x4000C000) ConLog.Error("Test RLWINM is filed! [0x%x]", CPU.GPR[6]);

		CPU.GPR[4] = gpr4;
		CPU.GPR[6] = gpr6;
		
		ConLog.Write("Opcodes test done.");
		tested = true;
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

	bool CheckCondition(OP_REG bo, OP_REG bi)
	{
		const u8 bo0 = (bo & 0x10) ? 1 : 0;
		const u8 bo1 = (bo & 0x08) ? 1 : 0;
		const u8 bo2 = (bo & 0x04) ? 1 : 0;
		const u8 bo3 = (bo & 0x02) ? 1 : 0;

		if(!bo2) --CPU.CTR;

		const u8 ctr_ok = bo2 | ((CPU.CTR != 0) ^ bo3);
		const u8 cond_ok = bo0 | (CPU.IsCR(bi) ^ (~bo1 & 0x1));

		//ConLog.Write("bo0: 0x%x, bo1: 0x%x, bo2: 0x%x, bo3: 0x%x", bo0, bo1, bo2, bo3);

		return ctr_ok && cond_ok;
	}

	u64& GetRegBySPR(OP_REG spr)
	{
		const u32 n = (spr & 0x1f) + ((spr >> 5) & 0x1f);

		switch(n)
		{
		case 0x001: return CPU.XER;
		case 0x008: return CPU.LR;
		case 0x009: return CPU.CTR;
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
		CPU.UpdateCRn(bf/4, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], uimm16);
	}
	virtual void CMPI(OP_REG bf, OP_REG l, OP_REG ra, OP_sIMM simm16)
	{
		CPU.UpdateCRn(bf/4, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], simm16);
	}
	virtual void ADDIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const s64 _ra = CPU.GPR[ra];
		CPU.GPR[rt] = CPU.GPR[ra] + simm16;
		CPU.UpdateXER_CA(CPU.IsADD_CA(CPU.GPR[rt], _ra, simm16));
	}
	virtual void ADDIC_(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const s64 _ra = CPU.GPR[ra];
		CPU.GPR[rt] = CPU.GPR[ra] + simm16;
		CPU.UpdateXER_CA(CPU.IsADD_CA(CPU.GPR[rt], _ra, simm16));
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
	virtual void BC(OP_REG bo, OP_REG bi, OP_sIMM bd, OP_REG aa, OP_REG lk)
	{
		if(!CheckCondition(bo, bi)) return;
		CPU.SetBranch(condBranchTarget(aa ? 0 : CPU.PC, bd));
		if(lk) CPU.LR = CPU.PC + 4;
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
			if(CheckCondition(bo, bi))
			{
				CPU.SetBranch(condBranchTarget(0, CPU.LR));
			}

			if(lk) CPU.LR = CPU.PC + 4;
		}
		virtual void BLR()
		{
			CPU.SetBranch(condBranchTarget(0, CPU.LR));
		}
		virtual void CRNOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = ~(CPU.IsCR(ba) | CPU.IsCR(bb));
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CRANDC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) & ~CPU.IsCR(bb);
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CRXOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) ^ CPU.IsCR(bb);
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CRNAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = ~(CPU.IsCR(ba) & CPU.IsCR(bb));
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CRAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) & CPU.IsCR(bb);
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CREQV(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = ~(CPU.IsCR(ba) ^ CPU.IsCR(bb));
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CROR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) | CPU.IsCR(bb);
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void CRORC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) | ~CPU.IsCR(bb);
			CPU.UpdateCR(bt, v & 0x1);
		}
		virtual void BCCTR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			if(!CheckCondition(bo, bi)) return;
			CPU.SetBranch(condBranchTarget(0, CPU.CTR));
			if(lk) CPU.LR = CPU.PC + 4;
		}
		virtual void BCTR()
		{
			CPU.SetBranch(condBranchTarget(0, CPU.CTR));
		}
		virtual void BCTRL()
		{
			CPU.SetBranch(condBranchTarget(0, CPU.CTR));
			CPU.LR = CPU.PC + 4;
		}
	END_OPCODES_GROUP(G_13);

	virtual void RLWINM(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, OP_REG me, bool rc)
	{
		const u32 r = (CPU.GPR[rs] << sh) | (CPU.GPR[rs] >> (32 - sh));
		CPU.GPR[ra] = r & rotate_mask[mb][me];
		if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
	}

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
			RLDICL(ra, rs, 0, uimm16, false);
		}
		virtual void RLDICL(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			const u64 r = (CPU.GPR[rs] << sh) | (CPU.GPR[rs] >> (64 - sh));
			CPU.GPR[ra] = r & rotate_mask[mb][63];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
	END_OPCODES_GROUP(G_1e);
	
	START_OPCODES_GROUP(G_1f)
		virtual void CMP(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn(bf/4, CPU.GPR[ra], CPU.GPR[rb]);
		}
		virtual void ADDC(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			CPU.UpdateXER_CA(CPU.IsADD_CA(CPU.GPR[rt], RA, RB));
			if(oe) CPU.UpdateXER_OV(CPU.IsADD_OV(CPU.GPR[rt], RA, RB));
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
			CPU.UpdateCRn(bf/4, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], l ? CPU.GPR[rb] : (u32)CPU.GPR[rb]);
		}
		virtual void CMPLD(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb, bool rc)
		{
			if(rc) UNK("cmpld.");
			CPU.UpdateCRn(bf/4, CPU.GPR[ra], CPU.GPR[rb]);
		}
		virtual void SUBF(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA - RB;
			if(oe) CPU.UpdateXER_OV(CPU.IsSUB_OV(CPU.GPR[rt], RA, RB));
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
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
			CPU.GPR[ra] = CPU.GPR[rs] & CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void DCBF(OP_REG ra, OP_REG rb)
		{
			UNK("dcbf");
		}
		virtual void NEG(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = ~CPU.GPR[ra];
			if(oe) ConLog.Warning("nego");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void ADDE(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s8 CR = (CPU.XER >> 29) & 0x1;
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];
			const s64 tmp = RB + CR;

			CPU.GPR[rt] = RA + tmp;

			CPU.UpdateXER_CA(CPU.IsADD_CA(tmp, RB, CR) || CPU.IsADD_CA(CPU.GPR[rt], RA, tmp));

			if(oe) CPU.UpdateXER_OV(CPU.IsADD_OV(CPU.GPR[rt], RA, RB));
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void ADDZE(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			const s8 CR = (CPU.XER >> 29) & 0x1;
			const s64 RA = CPU.GPR[ra];
			CPU.GPR[rs] = CR + RA;

			CPU.UpdateXER_CA(CPU.IsADD_CA(CPU.GPR[rs], RA, CR));
			if(oe) CPU.UpdateXER_OV(CPU.IsADD_OV(CPU.GPR[rs], RA, CR));
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
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			if(oe) CPU.UpdateXER_OV(CPU.IsADD_OV(CPU.GPR[rt], RA, RB));
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void XOR(OP_REG rt, OP_REG ra, OP_REG rb, bool rc)
		{
			CPU.GPR[rt] = ~(CPU.GPR[ra] | CPU.GPR[rb]);
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
			CPU.GPR[rt] = abs((s64)CPU.GPR[ra]);
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void EXTSH(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s16)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
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
		const u32 addr = ra != 0 ? CPU.GPR[ra] + ds : ds;
		
		if(!Memory.MemFlags.IsFStubRange(addr))
		{
			CPU.GPR[rt] = (s64)(s32)Memory.Read32(addr);
			return;
		}

		const u32 val = Memory.Read32(Memory.MemFlags.FindAddr(addr));

		if(!DoFunc(val, rt))
			ConLog.Error("Unknown FStub value: 0x%x #pc: 0x%x", val, CPU.PC);

		CPU.GPR[31] = CPU.GPR[30]; //?
	}
	virtual void LBZ(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		CPU.GPR[rt] = (s64)(s8)Memory.Read8(ra != 0 ? CPU.GPR[ra] + ds : ds);
	}
	virtual void STH(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		Memory.Write16(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
	}
	virtual void STW(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		Memory.Write32(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
	}
	virtual void STB(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		Memory.Write8(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
	}
	virtual void LHZ(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		CPU.GPR[rs] = (s64)(s16)Memory.Read16(ra != 0 ? CPU.GPR[ra] + ds : ds);
	}
	virtual void LHZU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		const u32 addr = ra != 0 ? CPU.GPR[ra] + ds : ds;
		CPU.GPR[rs] = (s64)(s16)Memory.Read16(addr);
		CPU.GPR[ra] = addr;
	}
	
	START_OPCODES_GROUP(G_3a)
		virtual void LD(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			CPU.GPR[rt] = Memory.Read64(ra != 0 ? CPU.GPR[ra] + ds : ds);
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
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FADDS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] + CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSQRTS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = sqrt(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FRES(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb] == 0) return;
			CPU.FPR[frt] = 1.0f/CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMULS(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] * CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
	END_OPCODES_GROUP(G4_S);
	
	START_OPCODES_GROUP(G_3a)
		virtual void STD(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			Memory.Write64(ra != 0 ? CPU.GPR[ra] + ds : ds, CPU.GPR[rs]);
		}
		virtual void STDU(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rs == ra) return;

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
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSUB(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] - CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FADD(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] + CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSQRT(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = sqrt(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSEL(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] < 0.0f ? CPU.FPR[frb] : CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMUL(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra] * CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FRSQRTE(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("frsqrte");
		}
		virtual void FMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] - CPU.FPR[frc]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = -(CPU.FPR[frt] * CPU.FPR[frb] + CPU.FPR[frc]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FCMPO(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			UNK("fcmpo");
		}
		virtual void FNEG(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = -CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMR(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frb];
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = -abs(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = abs(CPU.FPR[frb]);
			if(rc) CPU.UpdateCR1(CPU.FPR[frt]);
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

	bool DoFunc(const u32 code, int r0 = -1)
	{
		static int num = 0;
		struct lwmutex
		{
			u64 lock_var;
			u32 attribute;
			u32 recursive_count;
			u32 sleep_queue;
			u32 unk;
		};

		struct lwmutex_attr
		{
			u32 attr_protocol;
			u32 attr_recursive;
			char name[8];
		};
		
#define FUNC_LOG_ERROR(x) ConLog.Error(x); CPU.GPR[3] = 0; goto _RETURN_
		switch(code)
		{
		//lv2
		case 0x2cb51f0d: CPU.GPR[3] = lv2FsClose(); goto _RETURN_;
		case 0x718bf5f8: CPU.GPR[3] = lv2FsOpen(); goto _RETURN_;
		case 0xecdcf2ab: CPU.GPR[3] = lv2FsWrite(); goto _RETURN_;
		case 0xe6f2c1e7:
			ConLog.Warning("process exit: r3 = 0x%llx, r4 =0x%llx");
			CPU.Close();
			goto _RETURN_;
		return true;
		case 0x67f9fedb: FUNC_LOG_ERROR("TODO: process exit spawn 2"); return true;

		case 0xaff080a4: FUNC_LOG_ERROR("TODO: ppu thread exit"); return true;
		case 0x24a1ea07: FUNC_LOG_ERROR("TODO: ppu thread create ex"); return true;
		case 0x350d454e: //ppu thread get id
			//FUNC_LOG_ERROR("TODO: ppu thread get id");
			ConLog.Warning("ppu thread get id");
			CPU.GPR[3] = 0;
			CPU.GPR[4] = CPU.GetId();
			goto _RETURN_;
		return true;
		case 0x3dd4a957: FUNC_LOG_ERROR("TODO: ppu thread register atexit"); return true;
		case 0x4a071d98: FUNC_LOG_ERROR("TODO: interrupt thread disestablish"); return true;
		case 0xa3e3be68: FUNC_LOG_ERROR("TODO: ppu thread once"); return true;
		case 0xac6fc404: FUNC_LOG_ERROR("TODO: ppu thread unregister atexit"); return true;
		
		case 0x2f85c0ef:
		{
			lwmutex& _lwmutex = *(lwmutex*)Memory.GetMemFromAddr(CPU.GPR[3]);

			ConLog.Write("lwmutex attribute: %x", _lwmutex.attribute);
			ConLog.Write("lwmutex lock_var: %x", _lwmutex.lock_var);
			ConLog.Write("lwmutex recursive_count: %x", _lwmutex.recursive_count);
			ConLog.Write("lwmutex sleep_queue: %x", _lwmutex.sleep_queue);
			CPU.GPR[3] = 0;
			FUNC_LOG_ERROR("TODO: lwmutex create");
		}
		return true;
		case 0xc3476d0c: FUNC_LOG_ERROR("TODO: lwmutex destroy"); return true;
		case 0x1573dc3f: FUNC_LOG_ERROR("TODO: lwmutex lock"); return true;
		case 0xaeb78725: FUNC_LOG_ERROR("TODO: lwmutex trylock"); return true;
		case 0x1bc200f4: FUNC_LOG_ERROR("TODO: lwmutex unlock"); return true;
		
		case 0x744680a2: FUNC_LOG_ERROR("TODO: initialize tls"); return true;
		case 0x8461e528: FUNC_LOG_ERROR("TODO: get system time"); return true;
		case 0xa2c7ba64: FUNC_LOG_ERROR("TODO: prx exitspawn with level"); return true;

		//netCtl
		case 0xbd5a59fc: FUNC_LOG_ERROR("TODO: netCtlInit"); return true;
		case 0x1e585b5d: FUNC_LOG_ERROR("TODO: netCtlGetInfo"); return true;

		//Io
		case 0x068fcbc6: FUNC_LOG_ERROR("TODO: sys_config_start"); return true;
		case 0x6d367953: FUNC_LOG_ERROR("TODO: sys_config_stop"); return true;
		case 0x6ae10596: FUNC_LOG_ERROR("TODO: sys_config_add_service_listener"); return true;
		case 0xf5d9d571: FUNC_LOG_ERROR("TODO: sys_config_remove_service_listener"); return true;
		case 0x78f058a2: FUNC_LOG_ERROR("TODO: sys_config_register_service"); return true;
		case 0x5f81900c: FUNC_LOG_ERROR("TODO: sys_config_unregister_service"); return true;
			//Pad
		case 0x1cf98800: FUNC_LOG_ERROR("TODO: ioPadInit"); return true;
		case 0x4d9b75d5: FUNC_LOG_ERROR("TODO: ioPadEnd"); return true;
		case 0x0d5f2c14: FUNC_LOG_ERROR("TODO: ioPadClearBuf"); return true;
		case 0x0e2dfaad: FUNC_LOG_ERROR("TODO: ioPadInfoPressMode"); return true;
		case 0x20a97ba2: FUNC_LOG_ERROR("TODO: ioPadLddRegisterController"); return true;
		case 0x3aaad464: FUNC_LOG_ERROR("TODO: ioPadGetInfo"); return true;
		case 0x3f797dff: FUNC_LOG_ERROR("TODO: ioPadGetRawData"); return true;
		case 0x6bc09c61: FUNC_LOG_ERROR("TODO: ioPadGetDataExtra"); return true;
		case 0x78200559: FUNC_LOG_ERROR("TODO: ioPadInfoSensorMode"); return true;
		case 0x8b72cda1: FUNC_LOG_ERROR("TODO: ioPadGetData"); return true;
		case 0x8b8231e5: FUNC_LOG_ERROR("TODO: ioPadLddGetPortNo"); return true;
		case 0xbafd6409: FUNC_LOG_ERROR("TODO: ioPadLddDataInsert"); return true;
		case 0xbe5be3ba: FUNC_LOG_ERROR("TODO: ioPadSetSensorMode"); return true;
		case 0xdbf4c59c: FUNC_LOG_ERROR("TODO: ioPadGetCapabilityInfo"); return true;
		case 0xe442faa8: FUNC_LOG_ERROR("TODO: ioPadLddUnregisterController"); return true;
		case 0xf65544ee: FUNC_LOG_ERROR("TODO: ioPadSetActDirect"); return true;
		case 0xf83f8182: FUNC_LOG_ERROR("TODO: ioPadSetPressMode"); return true;
		case 0x4cc9b68d: FUNC_LOG_ERROR("TODO: ioPadPeriphGetInfo"); return true;
		case 0x578e3c98: FUNC_LOG_ERROR("TODO: ioPadSetPortSetting"); return true;
		case 0x8a00f264: FUNC_LOG_ERROR("TODO: ioPadPeriphGetData"); return true;
		case 0xa703a51d: FUNC_LOG_ERROR("TODO: ioPadGetInfo2"); return true;
			//Mouse
		case 0xc9030138: FUNC_LOG_ERROR("TODO: ioMouseInit"); return true;
		case 0xe10183ce: FUNC_LOG_ERROR("TODO: ioMouseEnd"); return true;
		case 0x3ef66b95: FUNC_LOG_ERROR("TODO: ioMouseClearBuf"); return true;
		case 0x5baf30fb: FUNC_LOG_ERROR("TODO: ioMouseGetInfo"); return true;
		case 0x21a62e9b: FUNC_LOG_ERROR("TODO: ioMouseGetTabletDataList"); return true;
		case 0x2d16da4f: FUNC_LOG_ERROR("TODO: ioMouseSetTabletMode"); return true;
		case 0x3138e632: FUNC_LOG_ERROR("TODO: ioMouseGetData"); return true;
		case 0x4d0b3b1f: FUNC_LOG_ERROR("TODO: ioMouseInfoTabletMode"); return true;
		case 0xa328cc35: FUNC_LOG_ERROR("TODO: ioMouseGetRawData"); return true;
		case 0x6bd131f0: FUNC_LOG_ERROR("TODO: ioMouseGetDataList"); return true;
			//Keyboard
		case 0x433f6ec0: FUNC_LOG_ERROR("TODO: ioKbInit"); return true;
		case 0xbfce3285: FUNC_LOG_ERROR("TODO: ioKbEnd"); return true;
		case 0xff0a21b7: FUNC_LOG_ERROR("TODO: ioKbRead"); return true;
		case 0xdeefdfa7: FUNC_LOG_ERROR("TODO: ioKbSetReadMode"); return true;
		case 0xa5f85e4d: FUNC_LOG_ERROR("TODO: ioKbSetCodeType"); return true;
		case 0x4ab1fa77: FUNC_LOG_ERROR("TODO: ioKbCnvRawCode"); return true;
		case 0x3f72c56e: FUNC_LOG_ERROR("TODO: ioKbSetLEDStatus"); return true;
		case 0x2f1774d5: FUNC_LOG_ERROR("TODO: ioKbGetInfo"); return true;
		case 0x1f71ecbe: FUNC_LOG_ERROR("TODO: ioKbGetConfiguration"); return true;
		case 0x2073b7f6: FUNC_LOG_ERROR("TODO: ioKbClearBuf"); return true;

		//Sysutil
		case 0xe558748d: FUNC_LOG_ERROR("TODO: videoGetResolution"); return true;
		case 0x0bae8772: FUNC_LOG_ERROR("TODO: videoConfigure"); return true;
		case 0x887572d5: FUNC_LOG_ERROR("TODO: videoGetState"); return true;

		case 0x1e930eef: FUNC_LOG_ERROR("TODO: videoGetDeviceInfo"); return true;
		case 0x15b0b0cd: FUNC_LOG_ERROR("TODO: videoGetConfiguration"); return true;
		case 0xa322db75: FUNC_LOG_ERROR("TODO: videoGetResolutionAvailability"); return true;
		case 0xcfdf24bb: FUNC_LOG_ERROR("TODO: videoDebugSetMonitorType"); return true;
		case 0x8e8bc444: FUNC_LOG_ERROR("TODO: videoRegisterCallback"); return true;
		case 0x7871bed4: FUNC_LOG_ERROR("TODO: videoUnregisterCallback"); return true;
		case 0x75bbb672: FUNC_LOG_ERROR("TODO: videoGetNumberOfDevice"); return true;
		case 0x55e425c3: FUNC_LOG_ERROR("TODO: videoGetConvertCursorColorInfo"); return true;
		}

		CPU.GPR[3] = 0;
		if(r0 >= 0)
		{
			Memory.Write32(CPU.GPR[r0], CPU.LR);
			Memory.Write32(CPU.GPR[r0] + 4, 0);
		}
		return false;

_RETURN_:
		if(r0 >= 0)
		{
			Memory.Write32(CPU.GPR[r0], CPU.LR);
			Memory.Write32(CPU.GPR[r0] + 4, 0);
		}
		return true;
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		u64 patch;
		if(Memory.MemFlags.IsFlag(code, CPU.PC, patch))
		{
			CPU.GPR[2] = patch;
			CPU.GPR[3] = 2; //?
			CPU.GPR[9] = code;
			CPU.SetBranch(code);
			return;
		}

		if(!DoFunc(code))
			UNK(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));

		Emu.Pause();

		for(uint i=0; i<32; ++i) ConLog.Write("r%d = 0x%x", i, CPU.GPR[i]);
		//for(uint i=0; i<32; ++i) ConLog.Write("f%d = %f", i, CPU.FPR[i]);
		ConLog.Write("CR = 0x%08x", CPU.CR);
		ConLog.Write("LR = 0x%x", CPU.LR);
		ConLog.Write("CTR = 0x%x", CPU.CTR);
		ConLog.Write("XER = 0x%x", CPU.XER);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP