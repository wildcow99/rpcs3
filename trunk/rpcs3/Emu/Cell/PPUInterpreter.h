#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/SysCalls/SysCalls.h"
#include "rpcs3.h"

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

static u64 rotate_mask[64][64];
void InitRotateMask()
{
	static bool inited = false;
	if(inited) return;

	for(u32 mb=0; mb<64; mb++)for(u32 me=0; me<64; me++)
	{
		const u64 mask = (0xFFFFFFFFFFFFFFFFULL >> mb) ^ (0xFFFFFFFFFFFFFFFFULL >> (me + 1));		
		rotate_mask[mb][me] = mb > me ? ~mask : mask;
	}

	inited = true;
}

/*
u32 rotl32(const u32 x, const u8 n) const { return (x << n) | (x >> (32 - n)); }
u32 rotr32(const u32 x, const u8 n) const { return (x >> n) | (x << (32 - n)); }
u64 rotl64(const u64 x, const u8 n) const { return (x << n) | (x >> (64 - n)); }
u64 rotr64(const u64 x, const u8 n) const { return (x >> n) | (x << (64 - n)); }
*/

#define rotl32 _rotl
#define rotr32 _rotr
#define rotl64 _rotl64
#define rotr64 _rotr64

class PPU_Interpreter : public PPU_Opcodes
{
private:
	PPUThread& CPU;

public:
	PPU_Interpreter(PPUThread& cpu) : CPU(cpu)
	{
		InitRotateMask();
		TestOpcodes();
	}

private:
	virtual void Exit() {}

	virtual void TestOpcodes()
	{
		static bool tested = false;
		if(tested) return;

		u64 gpr3 = CPU.GPR[3];
		u64 gpr4 = CPU.GPR[4];
		u64 gpr6 = CPU.GPR[6];

		CPU.GPR[6] = 0x90003000;
		RLWINM(3, 6, 2, 0, 0x1D, false);
		if(CPU.GPR[3] != 0x4000C000) ConLog.Error("Test RLWINM is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x00040000;
		CPU.GPR[4] = 0x00004000;
		ADD(3,6,4,false,false);
		if(CPU.GPR[3] != 0x00044000) ConLog.Error("Test ADD is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0xFFF25730;
		CPU.GPR[6] = 0x7B4192C0;
		AND(3,4,6, false);
		if(CPU.GPR[3] != 0x7B401200) ConLog.Error("Test AND is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0x90003000;
		CPU.GPR[6] = 0x789A789B;
		XOR(3,4,6, false);
		if(CPU.GPR[3] != 0xE89A489B) ConLog.Error("Test XOR is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0x90003000;
		SUBFIC(3, 4, 0x00007000);
		if(CPU.GPR[3] != 0xffffffff70004000) ConLog.Error("Test SUBFIC is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0xFFFFFFFFFFFFFFFF;
		ABS(3, 4, false, false);
		if(CPU.GPR[3] != 0x00000001) ConLog.Error("Test ABS is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0xA5A5A5A5;
		EXTSB(3, 4, false);
		if(CPU.GPR[3] != 0xFFFFFFFFFFFFFFA5) ConLog.Error("Test EXTSB is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0x90003000;
		SRAWI(3, 4, 0x4, false);
		if(CPU.GPR[3] != 0xfffffffff9000300) ConLog.Error("Test SRAWI is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0x7B4192C0;
		XORI(3, 4, 0x5730);
		if(CPU.GPR[3] != 0x7B41C5F0) ConLog.Error("Test XORI is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0x00003000;
		MULLI(3, 4, 0xa);
		if(CPU.GPR[3] != 0x0001E000) ConLog.Error("Test MULLI is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[4] = 0xB0043000;
		CPU.GPR[6] = 0x789A789B;
		NOR(3, 4, 6, false);
		if(CPU.GPR[3] != 0xffffffff07618764) ConLog.Error("Test NOR is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x789A789B789A789BLL;
		RLDICL(3, 6, 0x36, 0x24, false);
		if(CPU.GPR[3] != 0x6de269e) ConLog.Error("Test RLDICL is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x789A789B;
		NEG(3, 6, false, false);
		if(CPU.GPR[3] != 0xffffffff87658765) ConLog.Error("Test NEG[1] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x90003000;
		NEG(3, 6, false, false);
		if(CPU.GPR[3] != 0xffffffff6fffd000) ConLog.Error("Test NEG[2] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x9000300090003000LL;
		NEG(3, 6, false, false);
		if(CPU.GPR[3] != 0x6fffcfff6fffd000) ConLog.Error("Test NEG[3] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x789A789B789A789BLL;
		RLDICR(3, 6, 0x36, 0x24, false);
		if(CPU.GPR[3] != 0x26de269e20000000) ConLog.Error("Test RLDICR is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[6] = 0x789A789B789A789BLL;
		RLDIC(3, 6, 0x36, 0x24, false);
		if(CPU.GPR[3] != 0x26c0000006de269e) ConLog.Error("Test RLDIC is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0x7f7f;
		CPU.GPR[4] = 0x7f7f;
		RLDIMI(3, 4, 32, 0, false);
		if(CPU.GPR[3] != 0x7f7f00007f7f) ConLog.Error("Test RLDIMI is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xdfb90000b4dfLL;
		CNTLZD(3, 4, false);
		if(CPU.GPR[3] != 0x10) ConLog.Error("Test CNTLZD is failed! [0x%llx]", CPU.GPR[3]);

		CPU.VPR[4] = 0x7B4192C0;
		CPU.VPR[3] = 0x5730;
		VXOR(3, 4, 3);
		if(CPU.VPR[3] != 0x7B41C5F0) ConLog.Error("Test VXOR is failed! [0x%s]", CPU.VPR[3].ToString());

		CPU.VPR[3].Clear();
		CPU.VPR[4].Clear();

		for(uint n=0; n<63; ++n)
		{
			//To shift the contents of a register right by n bits, set SH = 64 - n and MB = n.
			CPU.GPR[6] = 0x789A789B789A789BLL;
			RLDICL(3, 6, 64 - n, n, false);
			if(CPU.GPR[3] != 0x789A789B789A789BLL>>n) ConLog.Error("Test RLDICL (shift right %d) is failed! [0x%llx]", n, CPU.GPR[3]);
		}

		for(uint n=0; n<63; ++n)
		{
			//To shift the contents of a register left by n bits, by setting SH = n and ME = 63 - n.
			CPU.GPR[6] = 0x789A789B789A789BLL;
			RLDICR(3, 6, n, 63 - n, false);
			if(CPU.GPR[3] != 0x789A789B789A789BLL<<n) ConLog.Error("Test RLDICR (shift left %d) is failed! [0x%llx]", n, CPU.GPR[3]);

			//To clear the low-order n bits of a register, by setting SH = 0 and ME = 63 - n.
			CPU.GPR[6] = 0x789A789B789A789BLL;
			RLDICR(3, 6, 0, 63 - n, false);
			if(CPU.GPR[3] != (0x789A789B789A789BLL>>n)<<n) ConLog.Error("Test RLDICR (clear %d) is failed! [0x%llx]", n, CPU.GPR[3]);
		}


		CPU.GPR[3] = gpr3;
		CPU.GPR[4] = gpr4;
		CPU.GPR[6] = gpr6;
		
		ConLog.Write("Opcodes test done.");
		tested = true;
	}

	virtual void SysCall()
	{
		CPU.GPR[3] = SysCallsManager.DoSyscall(CPU.GPR[11], CPU);
		ConLog.Write("SysCall[%lld] done with code [%lld]!", CPU.GPR[11], CPU.GPR[3]);
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
		case 0x001: return CPU.XER.XER;
		case 0x008: return CPU.LR;
		case 0x009: return CPU.CTR;
		default: break;
		}

		UNK(wxString::Format("GetRegBySPR error: Unknown spr %d! #pc: 0x%x", n, CPU.PC));
		return CPU.XER.XER;
	}

	virtual void MFSPR(OP_REG rd, OP_REG spr)
	{
		CPU.GPR[rd] = GetRegBySPR(spr);
	}

	START_OPCODES_GROUP(G_04)
		virtual void VXOR(OP_REG vrd, OP_REG vra, OP_REG vrb)
		{
			CPU.VPR[vrd] = CPU.VPR[vra] ^ CPU.VPR[vrb];
		}
	END_OPCODES_GROUP(G_04);

	virtual void MULLI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = CPU.GPR[ra] * simm16;
	}
	virtual void SUBFIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ((s64)simm16) - CPU.GPR[ra];
		CPU.XER.CA = (CPU.GPR[ra] == 0 || CPU.IsCarry(0-CPU.GPR[ra], simm16));
	}
	virtual void CMPLI(OP_REG bf, OP_REG l, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.UpdateCRn(bf/4, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], l ? (u64)uimm16 : uimm16);
	}
	virtual void CMPI(OP_REG bf, OP_REG l, OP_REG ra, OP_sIMM simm16)
	{
		CPU.UpdateCRn(bf/4, l ? (s64)CPU.GPR[ra] : (s32)CPU.GPR[ra], l ? (s64)simm16 : simm16);
	}
	virtual void ADDIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const u64 RA = CPU.GPR[ra];
		CPU.GPR[rt] = RA + simm16;
		CPU.XER.CA = CPU.IsCarry(RA, simm16);
	}
	virtual void ADDIC_(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const u64 RA = CPU.GPR[ra];
		CPU.GPR[rt] = RA + simm16;
		CPU.XER.CA = CPU.IsCarry(RA, simm16);
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
		CPU.SetBranch(branchTarget(aa ? 0 : CPU.PC, bd));
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
			if(!CheckCondition(bo, bi)) return;
			CPU.SetBranch(branchTarget(0, CPU.LR));
			if(lk) CPU.LR = CPU.PC + 4;
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
			CPU.SetBranch(branchTarget(0, CPU.CTR));
			if(lk) CPU.LR = CPU.PC + 4;
		}
		virtual void BCTR()
		{
			CPU.SetBranch(branchTarget(0, CPU.CTR));
		}
		virtual void BCTRL()
		{
			CPU.SetBranch(branchTarget(0, CPU.CTR));
			CPU.LR = CPU.PC + 4;
		}
	END_OPCODES_GROUP(G_13);

	virtual void RLWINM(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, OP_REG me, bool rc)
	{
		CPU.GPR[ra] = rotl32(CPU.GPR[rs], sh) & rotate_mask[32 + mb][32 + me];
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
	virtual void XORI(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.GPR[rs] = CPU.GPR[ra] ^ uimm16;
	}
	virtual void XORIS(OP_REG rs, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.GPR[rs] = CPU.GPR[ra] ^ (uimm16 << 16);
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
		virtual void RLDICL(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[mb][63];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void RLDICR(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG me, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[0][me];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void RLDIC(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[mb][63-sh];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void RLDIMI(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			const u64 mask = rotate_mask[mb][63-sh];
			CPU.GPR[ra] = (CPU.GPR[ra] & ~mask) | (rotl64(CPU.GPR[rs], sh) & mask);
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
	END_OPCODES_GROUP(G_1e);
	
	START_OPCODES_GROUP(G_1f)
		virtual void CMP(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn(bf/4, l ? (s64)CPU.GPR[ra] : (s32)CPU.GPR[ra],  l ? (s64)CPU.GPR[rb] : (s32)CPU.GPR[rb]);
		}
		virtual void ADDC(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			CPU.XER.CA = CPU.IsCarry(RA, RB);
			if(oe) ConLog.Warning("addco");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}

		START_OPCODES_GROUP(G_1f_1d3)
			virtual void MFCR(OP_REG rt)
			{
				CPU.GPR[rt] = CPU.CR.CR;
			}
		END_OPCODES_GROUP(G_1f_1d3);

		virtual void LWARX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			CPU.GPR[ra] = /*(s64)(s32)*/Memory.Read32(rs ? CPU.GPR[rs] + CPU.GPR[rb] : CPU.GPR[rb]);
			CPU.reserve32 = CPU.GPR[ra];
			CPU.has_reserve32 = true;
		}
		virtual void LDX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			CPU.GPR[ra] = Memory.Read64(rs ? CPU.GPR[rs] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void LWZX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			CPU.GPR[ra] = /*(s64)(s32)*/Memory.Read32(rs ? CPU.GPR[rs] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void SLW(OP_REG ra, OP_REG rs, OP_REG rb, OP_REG a, bool rc)
		{
			CPU.GPR[ra] = a ? CPU.GPR[rs] >> CPU.GPR[rb] : CPU.GPR[rs] << CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void CNTLZW(OP_REG rs, OP_REG ra, bool rc)
		{
			const u32 RS = CPU.GPR[rs];
			u8 i=0;
			for (u32 mask=0x80000000; i < 32; i++, mask >>= 1) if(RS & mask) break;
			CPU.GPR[ra] = i;
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void AND(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] & CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void CMPL(OP_REG bf, OP_REG l, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn(bf/4, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], l ? CPU.GPR[rb] : (u32)CPU.GPR[rb]);
		}
		virtual void CMPLD(OP_REG bf, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn(bf/4, CPU.GPR[ra], CPU.GPR[rb]);
		}
		virtual void SUBF(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = CPU.GPR[rb] - CPU.GPR[ra];
			if(oe) ConLog.Warning("subfo");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void DCBST(OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbst");
		}
		virtual void CNTLZD(OP_REG ra, OP_REG rs, bool rc)
		{
			const u64 RS = CPU.GPR[rs];
			u8 i=0;
			for (u64 mask=0x8000000000000000ULL; i < 64; i++, mask >>= 1) if(RS & mask) break;
			CPU.GPR[ra] = i;
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void ANDC(OP_REG rs, OP_REG ra, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] & ~CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void DCBF(OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbf");
		}
		virtual void LVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			CPU.VPR[vrd] = Memory.Read128(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void NEG(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = 0-CPU.GPR[ra];
			if(oe) ConLog.Warning("nego");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void NOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = ~(CPU.GPR[rs] | CPU.GPR[rb]);
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void ADDE(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];

			CPU.GPR[rt] = RA + RB + CPU.XER.CA;
			CPU.XER.CA = CPU.IsCarry(RA, RB) || (CPU.XER.CA && CPU.IsCarry(RA + RB, CPU.XER.CA));

			if(oe) ConLog.Warning("addeo");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void MTOCRF(OP_REG fxm, OP_REG rs)
		{
			//CHECK ME!
			uint i, count;
			for(i=0, count=0; i<8; ++i)
			{
				if((fxm >> 1) & 0x1) count++;
			}

			if(count == 1)
			{
				//CR[4*i : 4*i+3] = RS[32+4*i : 32+4*i+3];
				CPU.SetCR(i, (CPU.GPR[rs] >> (4*i)) & 0xf);
			}
			//else
			//	CPU.CR.CR = 0;
		}
		virtual void STWCX_(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			CPU.SetCR(0, 0);
			if(CPU.has_reserve32)
			{
				if(Memory.Read32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]) == CPU.reserve32)
				{
					Memory.Write32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb], CPU.GPR[rs]);
					CPU.UpdateCR_EQ(0, true);
				}
			}

			CPU.UpdateCR_SO(0, CPU.XER.SO);
		}
		virtual void STWX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			Memory.Write32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb], CPU.GPR[rs]);
		}
		virtual void ADDZE(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			CPU.GPR[rs] = CPU.XER.CA + RA;

			CPU.XER.CA = CPU.IsCarry(RA, CPU.XER.CA);
			if(oe) ConLog.Warning("addzeo");
			if(rc) CPU.UpdateCR0(CPU.GPR[rs]);
		}
		virtual void STVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			Memory.Write128(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb], CPU.VPR[vrd]);
		}
		virtual void ADDME(OP_REG rs, OP_REG ra, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			CPU.GPR[rs] = RA + CPU.XER.CA - 1;
			CPU.XER.CA = CPU.IsCarry(RA, CPU.XER.CA - 1);

			if(oe) ConLog.Warning("addmeo");
			if(rc) CPU.UpdateCR0(CPU.GPR[rs]);
		}
		virtual void MULLW(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = (u32)CPU.GPR[ra] * (u32)CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
			if(oe) ConLog.Warning("mullwo");
		}
		virtual void DCBTST(OP_REG th, OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbtst");
		}
		virtual void ADD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			CPU.XER.CA = CPU.IsCarry(RA, RB);
			if(oe) ConLog.Warning("addo");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void DCBT(OP_REG ra, OP_REG rb, OP_REG th)
		{
			//ConLog.Warning("dcbt");
		}
		virtual void SRAWI(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			const int v = sh;
			if(v != 0)
			{
				const s32 r = CPU.GPR[rs];
				CPU.GPR[ra] = r >> v;
				CPU.XER.CA = r < 0 && r << (32 - v);
			}
			else
			{
				CPU.GPR[ra] = CPU.GPR[rs];
				CPU.XER.CA = 0;
			}

			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void SRADI(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			const int v = sh;
			if(v != 0)
			{
				const s64 r = CPU.GPR[rs];
				CPU.GPR[ra] = r >> v;
				CPU.XER.CA = r < 0 && r << (64 - v);
			}
			else
			{
				CPU.GPR[ra] = CPU.GPR[rs];
				CPU.XER.CA = 0;
			}

			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void XOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] ^ CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void DIV(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(CPU.GPR[rb] == 0) return;
			if(oe) ConLog.Warning("divo");
			CPU.GPR[rt] = CPU.GPR[ra] / CPU.GPR[rb];
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void MFLR(OP_REG rt)
		{
			CPU.GPR[rt] = CPU.LR & (~3);
		}
		virtual void ABS(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = abs((s64)CPU.GPR[ra]);
			if(oe) ConLog.Warning("abso");
			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void MFTB(OP_REG rt)
		{
			CPU.GPR[rt] = CPU.TB;
		}
		virtual void DIVDU(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];

			if(RB == 0)
			{
				if(oe) ConLog.Warning("divduo");
				CPU.GPR[rt] = 0;
			}
			else
			{
				CPU.GPR[rt] = RA / RB;
			}

			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void DIVWU(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u32 RA = CPU.GPR[ra];
			const u32 RB = CPU.GPR[rb];

			if(RB == 0)
			{
				if(oe) ConLog.Warning("divwuo");
				CPU.GPR[rt] = 0;
			}
			else
			{
				CPU.GPR[rt] = RA / RB;
			}

			if(rc) CPU.UpdateCR0(CPU.GPR[rt]);
		}
		virtual void EXTSH(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s16)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void EXTSB(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s8)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void OR(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			if(rs==rb)
			{
				CPU.GPR[ra] = CPU.GPR[rs];
			}
			else
			{
				CPU.GPR[ra] = CPU.GPR[rs] | CPU.GPR[rb];
			}
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
		virtual void STFIWX(OP_REG frs, OP_REG ra, OP_REG rb)
		{
			Memory.Write32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb], (u32)CPU.FPR[frs].d);
		}
		virtual void EXTSW(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s32)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0(CPU.GPR[ra]);
		}
		virtual void SRW(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] >> CPU.GPR[rb];
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

	virtual void LWZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		const u64 addr = ra ? CPU.GPR[ra] + d : d;

		if(!Memory.MemFlags.IsFStubRange(addr))
		{
			CPU.GPR[rt] = /*(s64)(s32)*/Memory.Read32(addr);
			return;
		}

		DoFunc(Memory.Read32(Memory.MemFlags.FindAddr(Memory.Read32(addr))), rt);
	}
	virtual void LWZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		CPU.GPR[rt] = /*(s64)(s32)*/Memory.Read32(CPU.GPR[ra] + ds);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void LBZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		CPU.GPR[rt] = /*(s64)(s8)*/Memory.Read8(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LBZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		CPU.GPR[rt] = /*(s64)(s8)*/Memory.Read8(CPU.GPR[ra] + ds);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void STW(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write32(ra != 0 ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void STWU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		Memory.Write32(CPU.GPR[ra] + ds, CPU.GPR[rs]);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void STB(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write8(ra != 0 ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void LHZ(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		CPU.GPR[rs] = /*(s64)(s16)*/Memory.Read16(ra != 0 ? CPU.GPR[ra] + d : d);
	}
	virtual void LHZU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		CPU.GPR[rs] = /*(s64)(s16)*/Memory.Read16(CPU.GPR[ra] + ds);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void STH(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write16(ra != 0 ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void LFS(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		CPU.FPR[frt] = (float)Memory.Read32(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LFSU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		CPU.FPR[frt] = (float)Memory.Read32(CPU.GPR[ra] + ds);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void LFD(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		CPU.FPR[frt] = (double)Memory.Read64(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LFDU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		CPU.FPR[frt] = (double)Memory.Read64(CPU.GPR[ra] + ds);
		CPU.GPR[ra] = CPU.GPR[ra] + ds;
	}
	virtual void STFS(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write32(ra ? CPU.GPR[ra] + d : d, (u32)CPU.FPR[frs].i);
	}
	virtual void STFD(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write64(ra ? CPU.GPR[ra] + d : d, CPU.FPR[frs].i);
	}
	
	START_OPCODES_GROUP(G_3a)
		virtual void LD(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			CPU.GPR[rt] = Memory.Read64(ra != 0 ? CPU.GPR[ra] + ds : ds);
		}
		virtual void LDU(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rt == ra) return;
			CPU.GPR[rt] = Memory.Read64(CPU.GPR[ra] + ds);
			CPU.GPR[ra] = CPU.GPR[ra] + ds;
		}
	END_OPCODES_GROUP(G_3a);

	START_OPCODES_GROUP(G_3b)
		virtual void FDIVS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			if(rc) ConLog.Warning("fdivs.");//CPU.UpdateCR1(CPU.FPR[frt]);

			if(CPU.FPR[frb].d == 0.0)
			{
				CPU.SetFPSCRException(FPSCR_ZX);

				if(CPU.FPR[fra].d == 0.0)
				{
					CPU.FPR[frt].d = (double)0x7FF8000000000000ULL;
					CPU.SetFPSCRException(FPSCR_VXZDZ);
					return;
				}
			}
			else if(CPU.FPR[fra].IsINF() || CPU.FPR[frb].IsINF())
			{
				CPU.FPR[frt].d = (double)0x7FF8000000000000ULL;
				CPU.SetFPSCRException(FPSCR_VXIDI);
				return;
			}

			CPU.FPR[frt].d = CPU.FPR[fra].d / CPU.FPR[frb].d;
		}
		virtual void FSUBS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d - CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fsubs.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FADDS(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d + CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fadds.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSQRTS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = sqrt(CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fsqrts.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FRES(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].i == 0) return;
			CPU.FPR[frt].d = 1.0f/CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fres.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMULS(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmuls.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[frt].d * CPU.FPR[frb].d + CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmadds.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[frt].d * CPU.FPR[frb].d - CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmsubs.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUBS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[frt].d * CPU.FPR[frb].d - CPU.FPR[frc].d);
			if(rc) ConLog.Warning("fnmsubs.");////CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADDS(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[frt].d * CPU.FPR[frb].d + CPU.FPR[frc].d);
			if(rc) ConLog.Warning("fnmadds.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
	END_OPCODES_GROUP(G_3b);
	
	START_OPCODES_GROUP(G_3e)
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
	END_OPCODES_GROUP(G_3e);

	START_OPCODES_GROUP(G_3f)
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
			if((CPU.FPSCR.FPRF = CPU.FPR[fra].Cmp(CPU.FPR[frb])) == 1)
			{
				if(CPU.FPR[fra].IsSNaN() || CPU.FPR[frb].IsSNaN())
				{
					CPU.SetFPSCRException(FPSCR_VXSNAN);
				}
			}

			CPU.SetCR(bf/4, CPU.FPSCR.FPRF);
		}
		virtual void FRSP(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("frsp");
		}
		virtual void FCTIW(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].d > (double)0x7fffffff)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				CPU.FPR[frt].i = 0x7fffffff;
			}
			else if(CPU.FPR[frb].d < -(double)0x80000000)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				CPU.FPR[frt].i = 0x80000000;
			}
			else
			{
				s32 i = 0;
				switch(CPU.FPSCR.RN)
				{
					case FPSCR_RN_NEAR:
					{
						const double FRB_T = CPU.FPR[frb].d + 0.5;
						i = (s32)FRB_T;
						if(FRB_T - i < 0 || (FRB_T - i == 0 && CPU.FPR[frb].d > 0)) i--;
					}
					break;

					case FPSCR_RN_ZERO:
					{
						i = (s32)CPU.FPR[frb].d;
					}
					break;

					case FPSCR_RN_PINF:
					{
						i = (s32)CPU.FPR[frb].d;
						if(CPU.FPR[frb].d - i > 0) i++;
					}
					break;

					case FPSCR_RN_MINF:
					{
						i = (s32)CPU.FPR[frb].d;
						if(CPU.FPR[frb].d - i < 0) i--;
					}
					break;
                }

				CPU.FPR[frt].i = (u32)i;
				const double di = i;

				if(di == CPU.FPR[frb].d)
				{
					CPU.SetFPSCR_FI(0);
					CPU.FPSCR.FR = 0;
				}
				else
				{
					CPU.SetFPSCR_FI(1);
					CPU.FPSCR.FR = fabs(di) > fabs(CPU.FPR[frb].d);
				}
			}

			CPU.FPR[frt].i |= 0xfff8000000000000ULL;

			if(CPU.FPR[frt].i == 0 && ((*(u64*)&CPU.FPR[frt].d) & DOUBLE_SIGN))
			{
				CPU.FPR[frt].i |= 0x100000000ULL;
			}

			if(rc) ConLog.Warning("fctiw.");
		}
		virtual void FCTIWZ(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].d > (double)0x7fffffff)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				CPU.FPR[frt].i = 0x7fffffff;
			}
			else if(CPU.FPR[frb].d < -(double)0x80000000)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				CPU.FPR[frt].i = 0x80000000;
			}
			else
			{
				const s32 iFRB = (s32)CPU.FPR[frb].d;
				const double dFRB = iFRB;

				if(iFRB == dFRB)
				{
					CPU.SetFPSCR_FI(0);
					CPU.FPSCR.FR = 0;
				}
				else
				{
					CPU.SetFPSCR_FI(1);
					CPU.FPSCR.FR = fabs(dFRB) > fabs(CPU.FPR[frb].d);
				}

				CPU.FPR[frt].i = (u32)iFRB;
			}

			CPU.FPR[frt].i |= 0xfff8000000000000ULL;

			if(CPU.FPR[frt].i == 0 && ((*(u64*)&CPU.FPR[frt].d) & DOUBLE_SIGN))
			{
				CPU.FPR[frt].i |= 0x100000000ULL;
			}

			if(rc) ConLog.Warning("fctiwz.");
		}
		virtual void FDIV(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].i == 0) return;
			CPU.FPR[frt].d = CPU.FPR[fra].d / CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fdiv.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSUB(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d - CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fsub.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FADD(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d + CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fadd.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSQRT(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = sqrt(CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fsqrt.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FSEL(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d < 0.0 ? CPU.FPR[frb].d : CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fsel.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMUL(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d;
			CPU.FPSCR.FI = 0;
			CPU.FPSCR.FR = 0;
			if(rc) ConLog.Warning("fmul.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FRSQRTE(OP_REG frt, OP_REG frb, bool rc)
		{
			UNK("frsqrte");
		}
		virtual void FMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frt].d * CPU.FPR[frb].d - CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmsub.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[frt].d * CPU.FPR[frb].d + CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmadd.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUB(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[frt].d * CPU.FPR[frb].d - CPU.FPR[frc].d);
			if(rc) ConLog.Warning("fnmsub.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADD(OP_REG frt, OP_REG fra, OP_REG frb, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[frt].d * CPU.FPR[frb].d + CPU.FPR[frc].d);
			if(rc) ConLog.Warning("fnmadd.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FCMPO(OP_REG bf, OP_REG fra, OP_REG frb)
		{
			if((CPU.FPSCR.FPRF = CPU.FPR[fra].Cmp(CPU.FPR[frb])) == 1)
			{
				const bool isSNaN = CPU.FPR[fra].IsSNaN() || CPU.FPR[frb].IsSNaN();
				if(!isSNaN || (isSNaN && !CPU.FPSCR.VE))
				{
					CPU.SetFPSCRException(FPSCR_VXVC);
				}

				CPU.FPSCR.FX = 1;
			}

			CPU.SetCR(bf/4, CPU.FPSCR.FPRF);
		}
		virtual void FNEG(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].i = CPU.FPR[frb].i ^ 0x8000000000000000ULL;
			if(rc) ConLog.Warning("fneg.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMR(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frb];
			if(rc) ConLog.Warning("fmr.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].i = CPU.FPR[frb].i | 0x8000000000000000ULL;
			if(rc) ConLog.Warning("fnabs.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FABS(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = fabs(CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fabs.");//CPU.UpdateCR1(CPU.FPR[frt]);
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
			CPU.FPR[frt].d = (float)CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fcfid.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
	END_OPCODES_GROUP(G_3f);

	void DoFunc(const u32 code, u32 r0)
	{
		CPU.GPR[3] = SysCallsManager.DoFunc(code, CPU);

		CPU.GPR[r0] = CPU.stack_addr; //(Memory.MainRam.GetEndAddr() & ~0x3) - 4*4;
		Memory.Write32(CPU.GPR[r0],		CPU.LR);
		Memory.Write32(CPU.GPR[r0] + 4, CPU.GPR[3]);
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		u64 toc;
		if(Memory.MemFlags.IsFlag(code, CPU.PC, toc))
		{
			CPU.GPR[2] = toc;
			CPU.SetBranch(code);
			return;
		}

		UNK(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));

		Emu.Pause();

		for(uint i=0; i<32; ++i) ConLog.Write("r%d = 0x%llx", i, CPU.GPR[i]);
		for(uint i=0; i<32; ++i) ConLog.Write("f%d = %llf", i, CPU.FPR[i].d);
		for(uint i=0; i<32; ++i) ConLog.Write("v%d = 0x%s", i, CPU.VPR[i].ToString());
		ConLog.Write("CR = 0x%08x", CPU.CR);
		ConLog.Write("LR = 0x%llx", CPU.LR);
		ConLog.Write("CTR = 0x%llx", CPU.CTR);
		ConLog.Write("XER = 0x%llx", CPU.XER);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP