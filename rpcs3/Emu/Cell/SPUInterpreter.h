#pragma once

#include "Emu/Cell/SPUOpcodes.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/SPUThread.h"
#include "Emu/SysCalls/SysCalls.h"

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(x)

class SPU_Interpreter : public SPU_Opcodes
{
private:
	SPUThread& CPU;

public:
	SPU_Interpreter(SPUThread& cpu) : CPU(cpu)
	{
	}

private:
	virtual void Exit(){}

	virtual void SysCall()
	{
	}

	//0 - 10
	virtual void STOP(OP_uIMM code)
	{
		Emu.Stop();
	}
	virtual void LNOP()
	{
	}
	virtual void RDCH(OP_REG rt, OP_REG ra)
	{
		UNK("RDCH"); return;
		if(!CPU.CH[ra].used)
		{
			ConLog.Warning("RDCH: Channel %d is not used! #pc: 0x%x", ra, CPU.PC);
		}

		CPU.GPR[rt] = CPU.CH[ra].data;
	}
	virtual void RCHCNT(OP_REG rt, OP_REG ra)
	{
		UNK("RCHCNT"); return;
		int count = 16;
		for(; count; count--)
		{
			if(CPU.CH[ra].data._i16[count]) continue;
			break;
		}

		CPU.GPR[rt] = count;
	}
	virtual void SF(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.GPR[rt] = _mm_sub_epi32(CPU.GPR[ra], CPU.GPR[rb]);
	}
	virtual void SHLI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		CPU.GPR[rt] = _mm_slli_epi32(CPU.GPR[ra], i7);
	}
	virtual void A(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.GPR[rt] = _mm_add_epi32(CPU.GPR[ra], CPU.GPR[rb]);
	}
	virtual void SPU_AND(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.GPR[rt] = CPU.GPR[ra] & CPU.GPR[rb];
	}
	virtual void LQX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.LSA = CPU.GPR[ra]._u32[0] + CPU.GPR[rb]._u32[0];
		CPU.GPR[rt] = Memory.Read128(CPU.LSA);
	}
	virtual void WRCH(OP_REG ra, OP_REG rt)
	{
		UNK("WRCH"); return;
		CPU.CH[ra].used = true;
		CPU.CH[ra].data = CPU.GPR[rt];
	}
	virtual void STQX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.LSA = CPU.GPR[ra]._u32[0] + CPU.GPR[rb]._u32[0];
		Memory.Write128(CPU.LSA, CPU.GPR[rt]._u128);
	}
	virtual void BI(OP_REG ra)
	{
		CPU.SetBranch(CPU.GPR[ra]._u32[0] & 0xfffffffc);
	}
	virtual void BISL(OP_REG rt, OP_REG ra)
	{
		CPU.SetBranch(CPU.GPR[ra]._u32[0] & 0xfffffffc);
		CPU.GPR[rt] = CPU.PC + 4;
	}
	virtual void HBR(OP_REG p, OP_REG ro, OP_REG ra)
	{
		CPU.SetBranch(CPU.GPR[ra]._u32[0]);
	}
	virtual void CWX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		const u32 pos = ((CPU.GPR[ra]._u32[0] + CPU.GPR[rb]._u32[0]) & 0xc) / 4;
		for(u32 i=0; i<16; ++i) CPU.GPR[rt]._i8[i] = 0x1f - i;
		CPU.GPR[rt]._u32[pos] = 0x10203;
	}
	virtual void ROTQBY(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//CHECK ME
		const u32 s = CPU.GPR[rb]._u8[3];
		CPU.GPR[rt] = 0;
		for(u32 b=0; b<16; ++b)
		{
			if(b + s < 16)
			{
				CPU.GPR[rt] |= (SPU_GPR_hdr)CPU.GPR[ra]._u8[b + s] << b;
			}
			else
			{
				CPU.GPR[rt] |= (SPU_GPR_hdr)CPU.GPR[ra]._u8[b + s - 16] << b;
			}
		}
	}
	virtual void ROTQBYI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		//CHECK ME
		const u16 s = (i7 >> 14) & 0xf;

		CPU.GPR[rt] = 0;
		for(u32 b=0; b < 16; ++b)
		{
			if(b + s < 16)
			{
				CPU.GPR[rt] |= (SPU_GPR_hdr)CPU.GPR[ra]._u8[b + s] << b;
			}
			else
			{
				CPU.GPR[rt] |= (SPU_GPR_hdr)CPU.GPR[ra]._u8[b + s - 16] << b;
			}
		}
	}
	virtual void SHLQBYI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		//CHECK ME
		const u16 s = i7 & 0x1f;

		u16 r = 0;
		for(u32 b=0; b + s < 16; ++b)
		{
			r |= ((CPU.GPR[ra]._u16[0] >> (b + s)) & 0x1) << b;
		}

		CPU.GPR[rt] = r;
	}
	virtual void SPU_NOP(OP_REG rt)
	{
	}
	virtual void CLGT(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		CPU.GPR[rt] = _mm_cmpgt_epi32(CPU.GPR[ra], CPU.GPR[rb]);
	}

	//0 - 8
	virtual void BRZ(OP_REG rt, OP_sIMM i16)
	{
		//CHECK ME
		if(CPU.GPR[rt]._u32[0]) CPU.SetBranch(branchTarget(CPU.PC, i16));
	}
	virtual void BRHZ(OP_REG rt, OP_sIMM i16)
	{
		//CHECK ME
		if(CPU.GPR[rt]._u16[0]) CPU.SetBranch(branchTarget(CPU.PC, i16));
	}
	virtual void BRHNZ(OP_REG rt, OP_sIMM i16)
	{
		//CHECK ME
		if(!CPU.GPR[rt]._u16[0]) CPU.SetBranch(branchTarget(CPU.PC, i16));
	}
	virtual void STQR(OP_REG rt, OP_sIMM i16)
	{
		//CHECK ME
		CPU.LSA = branchTarget(CPU.PC, i16)/* & 0xfffffff0*/;
		if(!Memory.IsGoodAddr(CPU.LSA))
		{
			ConLog.Warning("LQD: Bad addr: 0x%x", CPU.LSA);
			return;
		}

		Memory.Write128(CPU.LSA, CPU.GPR[rt]._u128);
	}
	virtual void BR(OP_sIMM i16)
	{
		CPU.SetBranch(branchTarget(CPU.PC, i16));
	}
	virtual void FSMBI(OP_REG rt, OP_sIMM i16)
	{
		u32 s = i16;
		u32 r = 0;

		for(u32 j=0; j<16; ++j)
		{
			if(s >> j) r |= 0xFF << j;
			else r |= ~(0xFF << j);
		}

		CPU.GPR[rt] = r;
	}
	virtual void BRSL(OP_REG rt, OP_sIMM i16)
	{
		CPU.GPR[rt] = CPU.PC + 4;
		CPU.SetBranch(branchTarget(CPU.PC, i16));
	}
	virtual void IL(OP_REG rt, OP_sIMM i16)
	{
		CPU.GPR[rt] = i16;
	}
	virtual void LQR(OP_REG rt, OP_sIMM i16)
	{
		//CHECK ME
		CPU.LSA = branchTarget(CPU.PC, i16)/* & 0xfffffff0*/;
		if(!Memory.IsGoodAddr(CPU.LSA))
		{
			ConLog.Warning("LQR: Bad addr: 0x%x", CPU.LSA);
			return;
		}

		CPU.GPR[rt] = Memory.Read128(CPU.LSA);
	}

	//0 - 7
	virtual void SPU_ORI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = CPU.GPR[ra]._u32[0] | i10;
	}
	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = CPU.GPR[ra]._i32[0] + i10;
	}
	virtual void AHI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = CPU.GPR[ra]._i16[0] + i10;
	}
	virtual void STQD(OP_REG rt, OP_sIMM i10, OP_REG ra)
	{
		CPU.LSA = (i10 + CPU.GPR[ra]._u32[0])/* & 0xfffffff0*/;
		if(!Memory.IsGoodAddr(CPU.LSA))
		{
			ConLog.Warning("STQD: Bad addr: 0x%x", CPU.LSA);
			return;
		}

		Memory.Write128(CPU.LSA, CPU.GPR[rt]._u128);
	}
	virtual void LQD(OP_REG rt, OP_sIMM i10, OP_REG ra)
	{
		//CHECK ME
		CPU.LSA = (i10 + CPU.GPR[ra]._u32[0])/* & 0xfffffff0*/;
		if(!Memory.IsGoodAddr(CPU.LSA))
		{
			ConLog.Warning("LQD: Bad addr: 0x%x", CPU.LSA);
			return;
		}

		CPU.GPR[rt] = Memory.Read128(CPU.LSA);
	}
	virtual void CLGTI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = _mm_cmpgt_epi32(CPU.GPR[ra], (SPU_GPR_hdr)i10);
	}
	virtual void CLGTHI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = _mm_cmpgt_epi16(CPU.GPR[ra], (SPU_GPR_hdr)i10);
	}
	virtual void CEQI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		CPU.GPR[rt] = _mm_cmpeq_epi32(CPU.GPR[ra], (SPU_GPR_hdr)i10);
	}

	//0 - 6
	virtual void HBRR(OP_sIMM ro, OP_sIMM i16)
	{
		//CHECK ME
		//CPU.GPR[0]._u64[0] = branchTarget(CPU.PC, i16);
		CPU.SetBranch(branchTarget(CPU.PC, ro));
	}
	virtual void ILA(OP_REG rt, OP_sIMM i18)
	{
		CPU.GPR[rt] = i18;
	}

	//0 - 3
	virtual void SELB(OP_REG rc, OP_REG ra, OP_REG rb, OP_REG rt)
	{
		CPU.GPR[rc] = _mm_or_si128(
				_mm_and_si128(CPU.GPR[rt], CPU.GPR[rb]),
				_mm_andnot_si128(CPU.GPR[rt], CPU.GPR[ra])
			);
	}
	virtual void SHUFB(OP_REG rc, OP_REG ra, OP_REG rb, OP_REG rt)
	{
		ConLog.Warning("SHUFB");
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		UNK(wxString::Format("Unknown/Illegal opcode! (0x%08x)", code));
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));
		Emu.Pause();
		for(uint i=0; i<128; ++i) ConLog.Write("r%d = 0x%s", i, CPU.GPR[i].ToString());
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP