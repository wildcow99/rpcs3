#include "Emu/Opcodes/Opcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

class InterpreterOpcodes : public Opcodes
{
private:
	Display* display;

public:
	InterpreterOpcodes()
	{
		display = new Display();
	}

private:
	virtual void Exit()
	{
		delete display;
	}

	virtual void Step()
	{
		/*
		static const u64 max_cycle = (PS3_CLK * 1000000ULL) / (1000 * 60);

		if(++CPU.cycle >= max_cycle)
		{
			//display->Flip();
			CPU.cycle = 0;
		}
		*/
	}

	virtual bool DoSysCall(const int code)
	{
		switch(code)
		{
		case 20://lv1_gpu_context_attribute
			ConLog.Write("SYSCALL: lv1_gpu_context_attribute");
			switch(CPU.GPR[4])
			{
			case 0x0100: //Display mode set
				display->SetMode();
			break;

			case 0x0101: //Display sync
				ConLog.Error("SYSCALL: Display sync");
			break;

			case 0x0102: //Display flip
				display->Flip();
			break;
			};

			CPU.GPR[3] = 0;//OK
		return true;
			
		default: ConLog.Error("Unknown SYSCALL! (%d #%x)", code, code); return false;
		};

		return false;
	}

	virtual void NOP()
	{
		//__asm nop
	}

	// Special
	virtual void UNK_SPECIAL(const int code, const int rs, const int rt, const int rd,
		const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown special opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, func, rs, rt, rd, sa, sa, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
	//

	virtual void SPECIAL2()
	{
		ConLog.Error("SPECIAL 2");
	}

	/*
	virtual void XOR(const int rd, const int rs, const int rt)
	{
		 if (rd != 0) CPU.GPR[rd] = CPU.GPR[rs] ^ CPU.GPR[rt];
	}
	*/

	virtual void MULLI(const int rt, const int rs, const int imm_s16)
	{
		if (rt != 0) CPU.GPR[rt] = ((s64)CPU.GPR[rs] * (s64)imm_s16) & 0xffffffffLL;
	}

	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("SUBFIC");
	}

	virtual void CMPLWI(const int rs, const int rt, const int imm_u16)
	{
		CPU.UpdateCR0(CPU.GPR[rt], imm_u16);
		if(rs != 0) for(uint i=0; i<4; ++i) CPU.CR[rs/4][i] = CPU.CR[0][i];
	}

	virtual void CMPWI(const int rs, const int rt, const int rd)
	{
		CPU.UpdateCR0(CPU.GPR[rt], CPU.GPR[rd]);
		if(rs != 0) for(uint i=0; i<4; ++i) CPU.CR[rs/4][i] = CPU.CR[0][i];
	}

	virtual void ADDIC(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;
		CPU.GPR[rs] = CPU.GPR[rt] + imm_s16;
		CPU.XER[XER_CA] = 1;
	}

	virtual void ADDIC_(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int value = CPU.GPR[rt] + imm_s16;
		CPU.GPR[rs] = value;
		CPU.XER[XER_CA] = 1;
		CPU.UpdateCR0(value);
	}

	//g1 - 0e
	virtual void G1()
	{
		ConLog.Error("G1");
	}
	//
	//virtual void ADDI(const int rt, const int rs, const int imm_u16)=0;
	//
	virtual void ADDIS(const int rt, const int rs, const int imm_s16)
	{
		if(rt == 0) return;

		if(rs == 0)
		{
			CPU.GPR[rt] = imm_s16 << 16;
		}
		else
		{
			CPU.GPR[rt] = CPU.GPR[rs] + (imm_s16 << 16);
		}
	}
	//g2 - 10
	virtual void G2()
	{
		ConLog.Error("G2");
	}

	//
	virtual void SC()
	{
		//Hmm... Is it really syscall?

		ConLog.Write("SYSCALL: 2!");
		DoSysCall((Memory.Read32(CPU.PC) >> 6) & 0x000fffff);

		/*
		CPU.NextPc();

		const u32 code = (Memory.Read32(CPU.PC) >> 6) & 0x000fffff;

		if(DoSysCall(code)) return;

		ConLog.Warning( "SYSCODE: %08X", code );
		*/
	}

	//g3 - 12
	virtual void G3()
	{
		ConLog.Error("G3");
	}
	//
	virtual void BLR()
	{
		ConLog.Error("BLR");
	}

	virtual void RLWINM()
	{
		ConLog.Error("RLWINM");
	}

	virtual void ROTLW(const int rt, const int rs, const int rd)
	{
		ConLog.Error("ROTLW");
	}

	virtual void ORI(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | imm_u16;
	}

	virtual void ORIS(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | (imm_u16 << 16);
	}

	virtual void XORI(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ imm_u16;
	}

	virtual void XORIS(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ (imm_u16 << 16);
	}

	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)
	{
		ConLog.Error("CLRLDI");
	}

	//g4 - 1f
	virtual void CMPW(const int rs, const int rt, const int rd)
	{
		ConLog.Error("CMPW r%d,r%d,r%d", rs, rt, rd);
	}
	//g4_g1
	virtual void CMPLD(const int rs, const int rt, const int rd)
	{
		ConLog.Error("CMPLD r%d,r%d,r%d", rs, rt, rd);
	}
	virtual void SUBF(const int rs, const int rt, const int rd)
	{
		ConLog.Error("SUBF r%d,r%d,r%d", rs, rt, rd);
	}
	virtual void UNK_G4_G1(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown G4_G1 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
	//
	virtual void ADD(const int rs, const int rt, const int rd)
	{
		 if (rs != 0) CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd];
	}
	virtual void XOR(const int rt, const int rs, const int rd)
	{
		 if (rd != 0) CPU.GPR[rd] = CPU.GPR[rs] ^ CPU.GPR[rt];
	}
	virtual void MR(const int rt, const int rs)
	{
		ConLog.Error("MR r%d,r%d", rt, rs);
	}
	//g4_g2
	virtual void MTLR(const int rs)
	{
		ConLog.Error("MTLR r%d", rs);
	}
	virtual void MTCTR(const int rs)
	{
		ConLog.Error("MTCTR r%d", rs);
	}
	virtual void UNK_G4_G2(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown G4_G2 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
	//
	virtual void SRAWI(const int rt, const int rs, const int rd)
	{
		ConLog.Error("SRAWI r%d,r%d,%d", rt, rs, rd);
	}
	virtual void EXTSW(const int rt, const int rs)
	{
		ConLog.Error("EXTSW r%d,r%d", rt, rs);
	}
	virtual void LWZX(const int rs, const int rt, const int rd)
	{
		ConLog.Error("LWZX r%d,r%d,r%d", rs, rt, rd);
	}
	virtual void UNK_G4(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown G4 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
	//

	//virtual void MFLR(const int rs)=0;
	//
	virtual void LWZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read32(CPU.GPR[rt] + imm_s16);
	}

	virtual void LWZU(const int rt, const int rs, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read32(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void LBZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read8(CPU.GPR[rt] + imm_s16);
	}

	virtual void LBZU(const int rt, const int rs, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read8(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void STW(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write32(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STWU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write32(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void STB(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write8(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STBU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write8(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void LHZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read16(CPU.GPR[rt] + imm_s16);
	}

	virtual void LHZU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read16(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void STH(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write16(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STHU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write16(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void LFS(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("LFS");
	}

	virtual void LFSU(const int rs, const int rt, const int imm_u16)
	{
		ConLog.Error("LFSU");
	}

	virtual void LFD(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("LFD");
	}

	virtual void STFS(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("STFS");
	}

	virtual void STFSU(const int rs, const int rt, const int imm_u16)
	{
		ConLog.Error("STFSU");
	}

	virtual void STFD(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("STFD");
	}

	virtual void LD(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;
		CPU.GPR[rs] = Memory.Read64(CPU.GPR[rt] + imm_s16);
	}

	virtual void FDIVS(const int rs, const int rt, const int rd)
	{
		ConLog.Error("FDIVS");
	}

	virtual void STD(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write32(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	//g5 - 3f
	virtual void G5()
	{
		ConLog.Error("G5");
	}
	//virtual void FCFID(const int rs, const int rd)=0;
	//
	
	virtual void UNK(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
};