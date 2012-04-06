#include "Emu/Cell/PPUOpcodes.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/SysCalls/SysCalls.h"
#include "rpcs3.h"

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

//#define HLE_CALL_LOG

static u64 rotate_mask[64][64];
void InitRotateMask()
{
	static bool inited = false;
	if(inited) return;

	for(u32 mb=0; mb<64; mb++) for(u32 me=0; me<64; me++)
	{
		const u64 mask = ((u64)-1 >> mb) ^ ((me >= 63) ? 0 : (u64)-1 >> (me + 1));	
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

		/*
		CPU.GPR[3] = 0x9000300090003000;
		CPU.GPR[4] = 0x9999900f9999900f;
		MULHD(3, 3, 4, 0, 0);
		ConLog.Error("Test MULHD is failed! [%lld]", CPU.GPR[3]);
		*/

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
		if(CPU.GPR[3] != 0x7f7f00007f7f) ConLog.Error("Test RLDIMI[1] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0x7f7f;
		CPU.GPR[4] = 0x7f7f;
		RLDIMI(3, 4, 16, 32, false);
		if(CPU.GPR[3] != 0x7f7f7f7f) ConLog.Error("Test RLDIMI[2] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xdfb90000b4dfLL;
		CNTLZD(3, 4, false);
		if(CPU.GPR[3] != 0x10) ConLog.Error("Test CNTLZD is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xABCDEF1234567890;
		SRADI1(3, 4, 0x5, false);
		if(CPU.GPR[3] != 0xfd5e6f7891a2b3c4) ConLog.Error("Test SRADI[1] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xABCDEF1234567890;
		SRADI1(3, 4, 0x20, false);
		if(CPU.GPR[3] != 0xffffffffabcdef12) ConLog.Error("Test SRADI[2] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xABCDEF1234567890;
		SRADI1(3, 4, 0x3a, false);
		if(CPU.GPR[3] != 0xffffffffffffffea) ConLog.Error("Test SRADI[3] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xABCDEF1234567890;
		CPU.GPR[6] = 0x20;
		SRW(3, 4, 6, false);
		if(CPU.GPR[3] != 0x0) ConLog.Error("Test SRW[0] is failed! [0x%llx]", CPU.GPR[3]);

		CPU.GPR[3] = 0;
		CPU.GPR[4] = 0xABCDEF1234567890;
		CPU.GPR[6] = 0xABCDEF1234567899;
		SRW(3, 4, 6, false);
		if(CPU.GPR[3] != 0x1a) ConLog.Error("Test SRW[1] is failed! [0x%llx]", CPU.GPR[3]);

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

		if((s32)CPU.GPR[3] < 0) ConLog.Error("SysCall[%lld] done with code [0x%x]! #pc: 0x%x", CPU.GPR[11], (u32)CPU.GPR[3], CPU.PC);

#ifdef HLE_CALL_LOG
		else ConLog.Write("SysCall[%lld] done with code [0x%llx]! #pc: 0x%x", CPU.GPR[11], CPU.GPR[3], CPU.PC);
#endif
	}

	void DoFunc(const u32 code, u32 r0)
	{
		CPU.GPR[3] = SysCallsManager.DoFunc(code, CPU);

		if((s32)CPU.GPR[3] < 0) ConLog.Write("Func[0x%x] done with code [0x%x]! #pc: 0x%x", code, (u32)CPU.GPR[3], CPU.PC);

#ifdef HLE_CALL_LOG
		else ConLog.Write("Func[0x%x] done with code [0x%llx]! #pc: 0x%x", code, CPU.GPR[3], CPU.PC);
#endif

		CPU.GPR[r0] = CPU.stack_addr;
		Memory.Write32(CPU.GPR[r0],		CPU.LR);
		Memory.Write32(CPU.GPR[r0] + 4, CPU.GPR[3]);
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
		const u8 bo4 = (bo & 0x01) ? 1 : 0;

		if(!bo2) --CPU.CTR;

		const u8 ctr_ok = bo2 | ((CPU.CTR != 0) ^ bo3);
		const u8 cond_ok = bo0 | (CPU.IsCR(bi) ^ (~bo1 & 0x1));

		//if(bo1) CPU.SetCR(bi, bo4 ? 1 : 0);
		//if(bo1) return !bo4;

		//ConLog.Write("bo0: 0x%x, bo1: 0x%x, bo2: 0x%x, bo3: 0x%x", bo0, bo1, bo2, bo3);

		return ctr_ok && cond_ok;
	}

	u64& GetRegBySPR(OP_REG spr)
	{
		const u32 n = (spr >> 5) | ((spr & 0x1f) << 5);

		switch(n)
		{
		case 0x001: return CPU.XER.XER;
		case 0x008: return CPU.LR;
		case 0x009: return CPU.CTR;
		}

		UNK(wxString::Format("GetRegBySPR error: Unknown spr %d! #pc: 0x%x", n, CPU.PC));
		return CPU.XER.XER;
	}

	virtual void TWI(OP_REG to, OP_REG ra, OP_sIMM simm16)
	{
		s32 a = CPU.GPR[ra];

		if( (a < simm16  && (to & 0x10)) ||
			(a > simm16  && (to & 0x8))  ||
			(a == simm16 && (to & 0x4))  ||
			((u32)a < (u32)simm16 && (to & 0x2)) ||
			((u32)a > (u32)simm16 && (to & 0x1)) )
		{
			ConLog.Warning("Trap! (twi %x, r%d, %x) #pc: 0x%x", to, ra, simm16, CPU.PC);
		}
	}

	START_OPCODES_GROUP(G_04)
		virtual void VXOR(OP_REG vrd, OP_REG vra, OP_REG vrb)
		{
			CPU.VPR[vrd] = CPU.VPR[vra] ^ CPU.VPR[vrb];
		}
	END_OPCODES_GROUP(G_04);

	virtual void MULLI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = (s64)CPU.GPR[ra] * simm16;
	}
	virtual void SUBFIC(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		const u64 RA = CPU.GPR[ra];
		CPU.GPR[rt] = ~RA + (s64)simm16 + 1;
		CPU.XER.CA = (RA == 0 || CPU.IsCarry(0-RA, simm16));
	}
	virtual void CMPLI(OP_REG crfd, OP_REG l, OP_REG ra, OP_uIMM uimm16)
	{
		CPU.UpdateCRn<u64>(crfd, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], uimm16);
	}
	virtual void CMPI(OP_REG crfd, OP_REG l, OP_REG ra, OP_sIMM simm16)
	{
		CPU.UpdateCRn<s64>(crfd, l ? CPU.GPR[ra] : (s32)CPU.GPR[ra], simm16);
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
		CPU.UpdateCR0<s64>(CPU.GPR[rt]);
	}
	virtual void ADDI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra ? (CPU.GPR[ra] + simm16) : simm16;
	}
	virtual void ADDIS(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra ? (CPU.GPR[ra] + (simm16 << 16)) : (simm16 << 16);
	}
	virtual void BC(OP_REG bo, OP_REG bi, OP_sIMM bd, OP_REG aa, OP_REG lk)
	{
		if(!CheckCondition(bo, bi)) return;
		CPU.SetBranch(branchTarget((aa ? 0 : CPU.PC), bd));
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
		CPU.SetBranch(branchTarget(aa ? 0 : CPU.PC, ll));
		if(lk) CPU.LR = CPU.PC + 4;
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
			const u8 v = 1 ^ (CPU.IsCR(ba) | CPU.IsCR(bb));
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CRANDC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) & (1 ^ CPU.IsCR(bb));
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CRXOR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) ^ CPU.IsCR(bb);
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CRNAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = 1 ^ (CPU.IsCR(ba) & CPU.IsCR(bb));
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CRAND(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) & CPU.IsCR(bb);
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CREQV(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = 1 ^ (CPU.IsCR(ba) ^ CPU.IsCR(bb));
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CROR(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) | CPU.IsCR(bb);
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void CRORC(OP_REG bt, OP_REG ba, OP_REG bb)
		{
			const u8 v = CPU.IsCR(ba) | (1 ^ CPU.IsCR(bb));
			CPU.SetCRBit2(bt, v & 0x1);
		}
		virtual void BCCTR(OP_REG bo, OP_REG bi, OP_REG bh, OP_REG lk)
		{
			if(bo & 0x10 || CPU.IsCR(bi) == (bo & 0x8))
			{
				CPU.SetBranch(branchTarget(0, CPU.CTR & ~0x3));
				if(lk) CPU.LR = CPU.PC + 4;
			}
		}
		virtual void BCTR()
		{
			CPU.SetBranch(branchTarget(0, CPU.CTR & ~0x3));
		}
		virtual void BCTRL()
		{
			CPU.SetBranch(branchTarget(0, CPU.CTR & ~0x3));
			CPU.LR = CPU.PC + 4;
		}
	END_OPCODES_GROUP(G_13);

	virtual void RLWINM(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, OP_REG me, bool rc)
	{
		CPU.GPR[ra] = rotl32(CPU.GPR[rs], sh) & rotate_mask[32 + mb][32 + me];
		if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
	}
	virtual void RLWNM(OP_REG ra, OP_REG rs, OP_REG rb, OP_REG mb, OP_REG me, bool rc)
	{
		CPU.GPR[ra] = rotl32(CPU.GPR[rs], CPU.GPR[rb] & 0x1f) & rotate_mask[32 + mb][32 + me];
		if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
	}
	virtual void ORI(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		/*
		if(ra == 0 && rs == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		*/
		CPU.GPR[ra] = CPU.GPR[rs] | uimm16;
	}
	virtual void ORIS(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		/*
		if(ra == 0 && rs == 0 && uimm16 == 0)
		{
			NOP();
			return;
		}
		*/
		CPU.GPR[ra] = CPU.GPR[rs] | (uimm16 << 16);
	}
	virtual void XORI(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] ^ uimm16;
	}
	virtual void XORIS(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] ^ (uimm16 << 16);
	}
	virtual void ANDI_(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] & uimm16;
		CPU.UpdateCR0<s64>(CPU.GPR[ra]);
	}
	virtual void ANDIS_(OP_REG ra, OP_REG rs, OP_uIMM uimm16)
	{
		CPU.GPR[ra] = CPU.GPR[rs] & (uimm16 << 16);
		CPU.UpdateCR0<s64>(CPU.GPR[ra]);
	}

	START_OPCODES_GROUP(G_1e)
		virtual void RLDICL(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[mb][63];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void RLDICR(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG me, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[0][me];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void RLDIC(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			CPU.GPR[ra] = rotl64(CPU.GPR[rs], sh) & rotate_mask[mb][63-sh];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void RLDIMI(OP_REG ra, OP_REG rs, OP_REG sh, OP_REG mb, bool rc)
		{
			const u64 mask = rotate_mask[mb][63-sh];
			CPU.GPR[ra] = (CPU.GPR[ra] & ~mask) | (rotl64(CPU.GPR[rs], sh) & mask);
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
	END_OPCODES_GROUP(G_1e);
	
	START_OPCODES_GROUP(G_1f)
		virtual void CMP(OP_REG crfd, OP_REG l, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn<s64>(crfd, l ? CPU.GPR[ra] : (s32)CPU.GPR[ra],  l ? CPU.GPR[rb] : (s32)CPU.GPR[rb]);
		}
		virtual void TW(OP_REG to, OP_REG ra, OP_REG rb)
		{
			s32 a = CPU.GPR[ra];
			s32 b = CPU.GPR[rb];

			if( (a < b  && (to & 0x10)) ||
				(a > b  && (to & 0x8))  ||
				(a == b && (to & 0x4))  ||
				((u32)a < (u32)b && (to & 0x2)) ||
				((u32)a > (u32)b && (to & 0x1)) )
			{
				ConLog.Warning("Trap! (tw %x, r%d, r%d) #pc: 0x%x", to, ra, rb, CPU.PC);
			}
		}
		virtual void ADDC(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			CPU.XER.CA = CPU.IsCarry(RA, RB);
			if(oe) ConLog.Warning("addco");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		virtual void MFOCRF(OP_REG a, OP_REG fxm, OP_REG rt)
		{
			/*
			if(a)
			{
				u32 n = 0, count = 0;
				for(u32 i=0; i<8; ++i)
				{
					if(fxm & (1 << i))
					{
						n = i;
						count++;
					}
				}

				if(count == 1)
				{
					//RT[32+4*n : 32+4*n+3] = CR[4*n : 4*n+3];
					CPU.GPR[rt] = (u64)CPU.GetCR(n) << (n * 4);
				}
				else CPU.GPR[rt] = 0;
			}
			else
			{
			*/
				CPU.GPR[rt] = CPU.CR.CR;
			//}
		}
		virtual void LWARX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			const u64 addr = ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb];
			CPU.reserve_addr = addr;
			CPU.reserve = true;
			CPU.GPR[rd] = Memory.Read32(addr);
		}
		virtual void LDX(OP_REG ra, OP_REG rs, OP_REG rb)
		{
			CPU.GPR[ra] = Memory.Read64(rs ? CPU.GPR[rs] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void LWZX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			CPU.GPR[rd] = Memory.Read32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void SLW(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			const u32 n = CPU.GPR[rb] & 0x1f;
			const u32 r = rotl32((u32)CPU.GPR[rs], n);
			const u32 m = (CPU.GPR[rb] & 0x20) ? 0 : rotate_mask[32][63 - n];

			CPU.GPR[ra] = r & m;

			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
		}
		virtual void CNTLZW(OP_REG ra, OP_REG rs, bool rc)
		{
			u32 i;
			for(i=0; i < 32; i++)
			{
				if(CPU.GPR[rs] & (0x80000000 >> i)) break;
			}

			CPU.GPR[ra] = i;

			if(rc) CPU.UpdateCR0<u32>(CPU.GPR[ra]);
		}
		virtual void SLD(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rb] & 0x40 ? 0 : CPU.GPR[rs] << (CPU.GPR[rb] & 0x3f);
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void AND(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] & CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void CMPL(OP_REG crfd, OP_REG l, OP_REG ra, OP_REG rb)
		{
			CPU.UpdateCRn<u64>(crfd, l ? CPU.GPR[ra] : (u32)CPU.GPR[ra], l ? CPU.GPR[rb] : (u32)CPU.GPR[rb]);
		}
		virtual void SUBF(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = CPU.GPR[rb] - CPU.GPR[ra];
			if(oe) ConLog.Warning("subfo");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		virtual void DCBST(OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbst");
		}
		virtual void CNTLZD(OP_REG ra, OP_REG rs, bool rc)
		{
			u32 i = 0;
			
			for(u64 mask = 1ULL << 63; i < 64; i++, mask >>= 1)
			{
				if(CPU.GPR[rs] & mask) break;
			}

			CPU.GPR[ra] = i;
			if(rc) CPU.UpdateCR0<u64>(CPU.GPR[ra]);
		}
		virtual void ANDC(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] & ~CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void LDARX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			const u64 addr = ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb];
			CPU.reserve_addr = addr;
			CPU.reserve = true;
			CPU.GPR[rd] = Memory.Read64(addr);
		}
		virtual void DCBF(OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbf");
		}
		virtual void LBZX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			CPU.GPR[rd] = Memory.Read8(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void LVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			CPU.VPR[vrd] = Memory.Read128(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void NEG(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = 0-CPU.GPR[ra];
			if(oe) ConLog.Warning("nego");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		virtual void LBZUX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			//if(ra == 0 || ra == rd) throw "Bad instruction [LBZUX]";

			const u64 addr = CPU.GPR[ra] + CPU.GPR[rb];
			CPU.GPR[rd] = Memory.Read8(addr);
			CPU.GPR[ra] = addr;
		}
		virtual void NOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = ~(CPU.GPR[rs] | CPU.GPR[rb]);
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void ADDE(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rd] = CPU.GPR[ra] + CPU.GPR[rb] + CPU.XER.CA;
		}
		virtual void MTOCRF(OP_REG fxm, OP_REG rs)
		{
			u32 n = 0, count = 0;
			for(u32 i=0; i<8; ++i)
			{
				if(fxm & (1 << i))
				{
					n = i;
					count++;
				}
			}

			if(count == 1)
			{
				//CR[4*n : 4*n+3] = RS[32+4*n : 32+4*n+3];
				CPU.SetCR(n, (CPU.GPR[rs] >> (4*n)) & 0xf);
			}
			else CPU.CR.CR = 0;
		}
		virtual void STDX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			Memory.Write64((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), CPU.GPR[rs]);
		}
		virtual void STWCX_(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			CPU.SetCR(0, CPU.XER.SO ? CR_SO : 0);
			if(!CPU.reserve) return;

			const u64 addr = ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb];

			if(addr == CPU.reserve_addr)
			{
				Memory.Write32(addr, CPU.GPR[rs]);
				CPU.SetCR_EQ(0, true);
			}
			else
			{
				static const bool u = 0;
				if(u) Memory.Write32(addr, CPU.GPR[rs]);
				CPU.SetCR_EQ(0, u);
				CPU.reserve = false;
			}
		}
		virtual void STWX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			Memory.Write32((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), CPU.GPR[rs]);
		}
		virtual void STDUX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			const u64 addr = CPU.GPR[ra] + CPU.GPR[rb];
			Memory.Write64(addr, CPU.GPR[rs]);
			CPU.GPR[ra] = addr;
		}
		virtual void ADDZE(OP_REG rd, OP_REG ra, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			CPU.GPR[rd] = CPU.XER.CA + RA;

			CPU.XER.CA = CPU.IsCarry(RA, CPU.XER.CA);
			if(oe) ConLog.Warning("addzeo");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rd]);
		}
		virtual void STBX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			Memory.Write8((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), CPU.GPR[rs]);
		}
		virtual void STVX(OP_REG vrd, OP_REG ra, OP_REG rb)
		{
			Memory.Write128((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), CPU.VPR[vrd]);
		}
		virtual void MULLD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = CPU.GPR[ra] * CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
			if(oe) ConLog.Warning("mulldo");
		}
		virtual void ADDME(OP_REG rd, OP_REG ra, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			CPU.GPR[rd] = RA + CPU.XER.CA - 1;
			CPU.XER.CA = CPU.IsCarry(RA, CPU.XER.CA - 1);

			if(oe) ConLog.Warning("addmeo");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rd]);
		}
		virtual void MULLW(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = (s64)(s32)((s32)CPU.GPR[ra] * (s32)CPU.GPR[rb]);
			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[rt]);
			if(oe) ConLog.Warning("mullwo");
		}
		virtual void MULHD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];

			//TODO...

			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
			if(oe) ConLog.Warning("mulhdo");
		}
		virtual void DCBTST(OP_REG th, OP_REG ra, OP_REG rb)
		{
			ConLog.Warning("dcbtst");
		}
		/*0x108*///DOZ
		virtual void ADD(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];
			CPU.GPR[rt] = RA + RB;
			CPU.XER.CA = CPU.IsCarry(RA, RB);
			if(oe) ConLog.Warning("addo");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		virtual void DCBT(OP_REG ra, OP_REG rb, OP_REG th)
		{
			//ConLog.Warning("dcbt");
		}
		virtual void LHZX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			CPU.GPR[rd] = Memory.Read16(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void EQV(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = ~(CPU.GPR[rs] ^ CPU.GPR[rb]);
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void ECIWX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			//HACK!
			CPU.GPR[rd] = Memory.Read32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void DIV(OP_REG rt, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			if(CPU.GPR[rb] == 0) return;
			if(oe) ConLog.Warning("divo");
			CPU.GPR[rt] = CPU.GPR[ra] / CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		virtual void LHZUX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			const u64 addr = ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb];
			CPU.GPR[rd] = Memory.Read16(addr);
			CPU.GPR[ra] = addr;
		}
		virtual void XOR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] ^ CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void MFSPR(OP_REG rd, OP_REG spr)
		{
			CPU.GPR[rd] = GetRegBySPR(spr);
		}
		virtual void LHAX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			CPU.GPR[rd] = (s64)(s16)Memory.Read16(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]);
		}
		virtual void ABS(OP_REG rt, OP_REG ra, OP_REG oe, bool rc)
		{
			CPU.GPR[rt] = abs((s64)CPU.GPR[ra]);
			if(oe) ConLog.Warning("abso");
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rt]);
		}
		/*0x16b*///DIVS
		virtual void MFTB(OP_REG rt, OP_REG spr)
		{
			const u32 n = (spr >> 5) | ((spr & 0x1f) << 5);

			switch(n)
			{
			case 268: CPU.GPR[rt] = CPU.TB /*& 0xffffffff*/; break;
			case 269: CPU.GPR[rt] = CPU.TB >> 32; break;
			default: UNK(wxString::Format("mftb r%d, %d", rt, spr)); break;
			}
		}
		virtual void LHAUX(OP_REG rd, OP_REG ra, OP_REG rb)
		{
			const u64 addr = ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb];
			CPU.GPR[rd] = (s64)(s16)Memory.Read16(addr);
			CPU.GPR[ra] = addr;
		}
		virtual void ECOWX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			//HACK!
			Memory.Write32((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), CPU.GPR[rs]);
		}
		virtual void OR(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rs] | CPU.GPR[rb];
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void DIVDU(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u64 RA = CPU.GPR[ra];
			const u64 RB = CPU.GPR[rb];

			if(RB == 0)
			{
				if(oe) ConLog.Warning("divduo");
				CPU.GPR[rd] = 0;
			}
			else
			{
				CPU.GPR[rd] = RA / RB;
			}

			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rd]);
		}
		virtual void DIVWU(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const u32 RA = CPU.GPR[ra];
			const u32 RB = CPU.GPR[rb];

			if(RB == 0)
			{
				if(oe) ConLog.Warning("divwuo");
				CPU.GPR[rd] = 0;
			}
			else
			{
				CPU.GPR[rd] = RA / RB;
			}

			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[rd]);
		}
		virtual void MTSPR(OP_REG spr, OP_REG rs)
		{
			GetRegBySPR(spr) = CPU.GPR[rs];
		}
		/*0x1d6*///DCBI
		virtual void DIVD(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s64 RA = CPU.GPR[ra];
			const s64 RB = CPU.GPR[rb];

			if (RB == 0 || ((u64)RA == 0x8000000000000000 && RB == -1))
			{
				if(oe) ConLog.Warning("divdo");
				CPU.GPR[rd] = (((u64)RA & 0x8000000000000000) && RB == 0) ? -1 : 0;
			}
			else
			{
				CPU.GPR[rd] = (u64)(RA / RB);
			}

			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[rd]);
		}
		virtual void DIVW(OP_REG rd, OP_REG ra, OP_REG rb, OP_REG oe, bool rc)
		{
			const s32 RA = CPU.GPR[ra];
			const s32 RB = CPU.GPR[rb];

			if (RB == 0 || ((u32)RA == 0x80000000 && RB == -1))
			{
				if(oe) ConLog.Warning("divwo");
				CPU.GPR[rd] = (((u32)RA & 0x80000000) && RB == 0) ? -1 : 0;
			}
			else
			{
				CPU.GPR[rd] = (s64)(RA / RB);
			}

			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[rd]);
		}
		/*0x217*///LFSX
		virtual void SRW(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rb] & 0x20 ? 0 : (u32)CPU.GPR[rs] >> (CPU.GPR[rb] & 0x1f);
			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
		}
		virtual void SRD(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			CPU.GPR[ra] = CPU.GPR[rb] & 0x40 ? 0 : CPU.GPR[rs] >> (CPU.GPR[rb] & 0x3f);
			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		/*0x237*///LFSUX
		/*0x257*///LFDX
		/*0x28a*///LDUX
		/*0x277*///LFDUX
		virtual void STFSX(OP_REG rs, OP_REG ra, OP_REG rb)
		{
			float v = (float)CPU.FPR[rs].d;
			Memory.Write32((ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb]), *(u32*)&v);
		}
		/*0x316*///LHBRX
		virtual void SRAW(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			if(CPU.GPR[rb] & 0x20)
			{
				if(CPU.GPR[rs] & 0x80000000)
				{
					CPU.GPR[ra] = ~0LL;
					CPU.XER.CA = 1;
				}
				else
				{
					CPU.GPR[ra] = 0LL;
					CPU.XER.CA = 0;
				}
			}
			else
			{
				const u8 n = CPU.GPR[rb] & 0x1f;
				if(n == 0)
				{
					CPU.GPR[ra] = (s64)(s32)CPU.GPR[rs];
					CPU.XER.CA = 0;
				}
				else
				{
					CPU.GPR[ra] = (s32)CPU.GPR[rs] >> n;
					CPU.XER.CA = (CPU.GPR[rs] & 0x80000000) > 0;
				}
			}

			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
		}
		virtual void SRAD(OP_REG ra, OP_REG rs, OP_REG rb, bool rc)
		{
			if(CPU.GPR[rb] & 0x40)
			{
				if(CPU.GPR[rs] & 0x8000000000000000LL)
				{
					CPU.GPR[ra] = ~0LL;
					CPU.XER.CA = 1;
				}
				else
				{
					CPU.GPR[ra] = 0LL;
					CPU.XER.CA = 0;
				}
			}
			else
			{
				const u8 n = CPU.GPR[rb] & 0x3f;
				if(n == 0)
				{
					CPU.GPR[ra] = CPU.GPR[rs];
					CPU.XER.CA = 0;
				}
				else
				{
					CPU.GPR[ra] = (s64)CPU.GPR[rs] >> n;
					CPU.XER.CA = (CPU.GPR[rs] & 0x8000000000000000LL) > 0;
				}
			}

			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void SRAWI(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			const u8 n = sh & 0x1f;
			const u64 r = rotl32(CPU.GPR[rs], 64 - n);
			const u64 m = rotate_mask[n + 32][63];
			const s64 s = CPU.GPR[rs] & 0x80000000 ? -1 : 0;

			CPU.GPR[ra] = (r & m) | (s & ~m);
			CPU.XER.CA = s & ((u32)(r & ~m) != 0);

			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
		}
		virtual void SRADI1(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			const u64 n = sh;
			const u64 r = rotl64(CPU.GPR[rs], 64 - n);
			const u64 m = rotate_mask[n][63];
			const u64 s = CPU.GPR[rs] & 0x8000000000000000ULL ? 0xFFFFFFFFFFFFFFFFULL : 0;

			CPU.GPR[ra] = (r & m) | (s & ~m);
			CPU.XER.CA = s & ((r & ~m) != 0);

			if(rc) CPU.UpdateCR0<s64>(CPU.GPR[ra]);
		}
		virtual void SRADI2(OP_REG ra, OP_REG rs, OP_REG sh, bool rc)
		{
			SRADI1(ra, rs, sh, rc);
		}
		virtual void EIEIO()
		{
		}
		virtual void EXTSH(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s16)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0<s16>(CPU.GPR[ra]);
		}
		virtual void EXTSB(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s8)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0<s8>(CPU.GPR[ra]);
		}
		virtual void STFIWX(OP_REG frs, OP_REG ra, OP_REG rb)
		{
			Memory.Write32(ra ? CPU.GPR[ra] + CPU.GPR[rb] : CPU.GPR[rb], *(u32*)&CPU.FPR[frs].d);
		}
		virtual void EXTSW(OP_REG ra, OP_REG rs, bool rc)
		{
			CPU.GPR[ra] = (s64)(s32)CPU.GPR[rs];
			if(rc) CPU.UpdateCR0<s32>(CPU.GPR[ra]);
		}
		/*0x3d6*///ICBI
		/*0x3f6*///DCBZ
	END_OPCODES_GROUP(G_1f);

	virtual void LWZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		CPU.GPR[rt] = Memory.Read32(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LWZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		CPU.GPR[rt] = Memory.Read32(addr);
		CPU.GPR[ra] = addr;
	}
	virtual void LBZ(OP_REG rt, OP_REG ra, OP_sIMM d)
	{
		CPU.GPR[rt] = Memory.Read8(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LBZU(OP_REG rt, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		CPU.GPR[rt] = Memory.Read8(addr);
		CPU.GPR[ra] = addr;
	}
	virtual void STW(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write32(ra ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void STWU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		Memory.Write32(addr, CPU.GPR[rs]);
		CPU.GPR[ra] = addr;
	}
	virtual void STB(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write8(ra ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void STBU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		Memory.Write8(addr, CPU.GPR[rs]);
		CPU.GPR[ra] = addr;
	}
	virtual void LHZ(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		CPU.GPR[rs] = Memory.Read16(ra ? CPU.GPR[ra] + d : d);
	}
	virtual void LHZU(OP_REG rs, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		CPU.GPR[rs] = Memory.Read16(addr);
		CPU.GPR[ra] = addr;
	}
	virtual void STH(OP_REG rs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write16(ra ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
	}
	virtual void LFS(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		const u32 v = Memory.Read32(ra ? CPU.GPR[ra] + d : d);
		CPU.FPR[frt] = *(float*)&v;
	}
	virtual void LFSU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		const u32 v = Memory.Read32(addr);
		CPU.FPR[frt] = *(float*)&v;
		CPU.GPR[ra] = addr;
	}
	virtual void LFD(OP_REG frt, OP_REG ra, OP_sIMM d)
	{
		const u64 v = Memory.Read64(ra ? CPU.GPR[ra] + d : d);
		CPU.FPR[frt] = *(double*)&v;
	}
	virtual void LFDU(OP_REG frt, OP_REG ra, OP_sIMM ds)
	{
		const u64 addr = CPU.GPR[ra] + ds;
		const u64 v = Memory.Read64(addr);
		CPU.FPR[frt] = *(double*)&v;
		CPU.GPR[ra] = addr;
	}
	virtual void STFS(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write32(ra ? CPU.GPR[ra] + d : d, *(u32*)&CPU.FPR[frs].d);
	}
	virtual void STFD(OP_REG frs, OP_REG ra, OP_sIMM d)
	{
		Memory.Write64(ra ? CPU.GPR[ra] + d : d, *(u64*)&CPU.FPR[frs].d);
	}
	
	START_OPCODES_GROUP(G_3a)
		virtual void LD(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			CPU.GPR[rt] = Memory.Read64(ra ? CPU.GPR[ra] + ds : ds);
		}
		virtual void LDU(OP_REG rt, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rt == ra) return;
			const u64 addr = CPU.GPR[ra] + ds;
			CPU.GPR[rt] = Memory.Read64(addr);
			CPU.GPR[ra] = addr;
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
			if(CPU.FPR[frb].d == 0.0) return;
			CPU.FPR[frt].d = 1.0f/CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fres.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMULS(OP_REG frt, OP_REG fra, OP_REG frc, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d;
			if(rc) ConLog.Warning("fmuls.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADDS(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d + CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fmadds.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMSUBS(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d - CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fmsubs.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUBS(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[fra].d * CPU.FPR[frc].d - CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fnmsubs.");////CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADDS(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[fra].d * CPU.FPR[frc].d + CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fnmadds.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
	END_OPCODES_GROUP(G_3b);
	
	START_OPCODES_GROUP(G_3e)
		virtual void STD(OP_REG rs, OP_REG ra, OP_sIMM d)
		{
			Memory.Write64(ra ? CPU.GPR[ra] + d : d, CPU.GPR[rs]);
		}
		virtual void STDU(OP_REG rs, OP_REG ra, OP_sIMM ds)
		{
			//if(ra == 0 || rs == ra) return;
			const u64 addr = CPU.GPR[ra] + ds;
			Memory.Write64(addr, CPU.GPR[rs]);
			CPU.GPR[ra] = addr;
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
		virtual void MTFSFI(OP_REG crfd, OP_REG i, bool rc)
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
		virtual void FCMPU(OP_REG crfd, OP_REG fra, OP_REG frb)
		{
			if((CPU.FPSCR.FPRF = CPU.FPR[fra].Cmp(CPU.FPR[frb])) == 1)
			{
				if(CPU.FPR[fra].IsSNaN() || CPU.FPR[frb].IsSNaN())
				{
					CPU.SetFPSCRException(FPSCR_VXSNAN);
				}
			}

			CPU.SetCR(crfd, CPU.FPSCR.FPRF);
		}
		virtual void FRSP(OP_REG frt, OP_REG frb, bool rc)
		{
			const double b = CPU.FPR[frb].d;
			const double r = (double)(float)b;
			//CPU.FPRCR.FR = fabs(r) > fabs(b);
			//CPU.FPRCR.FI = b != r;
			//FPSCR.FPRF
			CPU.FPR[frt] = r;

		}
		virtual void FCTIW(OP_REG frt, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].d > (double)0x7fffffff)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				*(u64*)&CPU.FPR[frt].d = 0x7fffffff;
			}
			else if(CPU.FPR[frb].d < -(double)0x80000000)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				*(u64*)&CPU.FPR[frt].d = 0x80000000;
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

				*(u64*)&CPU.FPR[frt].d = (u32)i;
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

			*(u64*)&CPU.FPR[frt].d |= 0xfff8000000000000ULL;

			if(*(u64*)&CPU.FPR[frt].d == 0 && ((*(u64*)&CPU.FPR[frt].d) & DOUBLE_SIGN))
			{
				*(u64*)&CPU.FPR[frt].d |= 0x100000000ULL;
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

				*(u64*)&CPU.FPR[frt].d = 0x7fffffff;
			}
			else if(CPU.FPR[frb].d < -(double)0x80000000)
			{
				CPU.SetFPSCRException(FPSCR_VXCVI);
				CPU.FPSCR.FR = 0;
				CPU.SetFPSCR_FI(0);

				*(u64*)&CPU.FPR[frt].d = 0x80000000;
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

				*(u64*)&CPU.FPR[frt].d = (u32)iFRB;
			}

			*(u64*)&CPU.FPR[frt].d |= 0xfff8000000000000ULL;

			if(*(u64*)&CPU.FPR[frt].d == 0 && ((*(u64*)&CPU.FPR[frt].d) & DOUBLE_SIGN))
			{
				*(u64*)&CPU.FPR[frt].d |= 0x100000000ULL;
			}

			if(rc) ConLog.Warning("fctiwz.");
		}
		virtual void FDIV(OP_REG frt, OP_REG fra, OP_REG frb, bool rc)
		{
			if(CPU.FPR[frb].d == 0.0) return;
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
		virtual void FSEL(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d < 0.0 ? CPU.FPR[frc].d : CPU.FPR[frb].d;
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
		virtual void FMSUB(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[fra].d * CPU.FPR[frc].d - CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fmsub.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMADD(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = CPU.FPR[fra].d * CPU.FPR[frc].d + CPU.FPR[frb].d;
			if(rc) ConLog.Warning("fmadd.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMSUB(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[fra].d * CPU.FPR[frc].d - CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fnmsub.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNMADD(OP_REG frt, OP_REG fra, OP_REG frc, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = -(CPU.FPR[fra].d * CPU.FPR[frc].d + CPU.FPR[frb].d);
			if(rc) ConLog.Warning("fnmadd.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FCMPO(OP_REG crfd, OP_REG fra, OP_REG frb)
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

			CPU.SetCR(crfd, CPU.FPSCR.FPRF);
		}
		virtual void FNEG(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt].d = *(u64*)&CPU.FPR[frb].d ^ 0x8000000000000000ULL;
			if(rc) ConLog.Warning("fneg.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FMR(OP_REG frt, OP_REG frb, bool rc)
		{
			CPU.FPR[frt] = CPU.FPR[frb];
			if(rc) ConLog.Warning("fmr.");//CPU.UpdateCR1(CPU.FPR[frt]);
		}
		virtual void FNABS(OP_REG frt, OP_REG frb, bool rc)
		{
			*(u64*)&CPU.FPR[frt].d = *(u64*)&CPU.FPR[frb].d | 0x8000000000000000ULL;
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

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
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