#include "Emu/Opcodes/Opcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(name, name_string)\
	virtual void UNK_##name##(\
		const int code, const int opcode, OP_REG rs, OP_REG rt, OP_REG rd, OP_REG sa, const u32 func,\
		OP_REG crfs, OP_REG crft, OP_REG crm, OP_REG bd, OP_REG lk, OP_REG ms, OP_REG mt, OP_REG mfm,\
		OP_REG ls, OP_REG lt, const s32 imm_m, const s32 imm_s16, const u32 imm_u16, const u32 imm_u26) \
	{\
		ConLog.Error\
		(\
			"Unknown "##name_string##" opcode! - (%08x - %02x) - " \
			"rs: r%d, rt: r%d, rd: r%d, " \
			"sa: 0x%x : %d, func: 0x%x : %d, " \
			"crfs: 0x%x : %d, crft: 0x%x : %d, " \
			"crm: 0x%x : %d, bd: 0x%x : %d, " \
			"lk: 0x%x : %d, ms: 0x%x : %d, " \
			"mt: 0x%x : %d, mfm: 0x%x : %d, " \
			"ls: 0x%x : %d, lt: 0x%x : %d," \
			"imm m: 0x%x : %d, imm s16: 0x%x : %d, " \
			"imm u16: 0x%x : %d, imm u26: 0x%x : %d, " \
			"Branch: 0x%x, Jump: 0x%x", \
			code, opcode, \
			rs, rt, rd, \
			sa, sa, func, func, \
			crfs, crfs, crft, crft, \
			crm, crm, bd, bd, \
			lk, lk, ms, ms, \
			mt, mt, mfm, mfm, \
			ls, ls, lt, lt, \
			imm_m, imm_m, imm_s16, imm_s16, \
			imm_u16, imm_u16, imm_u26, imm_u26, \
			branchTarget(imm_u26), jumpTarget(imm_u26) \
		); \
	}

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
			display->Flip();
			CPU.cycle = 0;
		}
		*/
	}

	virtual void SysCall(const int code)
	{
		CPU.GPR[3] = DoSyscall(code);
	}

	virtual void NOP()
	{
		//__asm nop
	}

	START_OPCODES_GROUP(SPECIAL)
	END_OPCODES_GROUP(SPECIAL, "special");

	START_OPCODES_GROUP(SPECIAL2)
	END_OPCODES_GROUP(SPECIAL2, "special 2");
	/*
	virtual void XOR(OP_REG rd, OP_REG rs, OP_REG rt)
	{
		 if (rd != 0) CPU.GPR[rd] = CPU.GPR[rs] ^ CPU.GPR[rt];
	}
	*/
	virtual void MULLI(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
	{
		if (rs != 0) CPU.GPR[rs] = ((s64)CPU.GPR[rt] * (s64)imm_s16) & 0xffffffffLL;
	}
	virtual void SUBFIC(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		const s64 _rt = CPU.GPR[rt];
		if(rs != 0) CPU.GPR[rs] = imm_s16 - _rt;
		CPU.UpdateXER_CA(CPU.IsCarryGen(-_rt, imm_s16));
	}
	virtual void CMPLDI(OP_REG rs, OP_REG rt, OP_REG imm_u16)
	{	
		ConLog.Error("CMPLDI");
	}
	virtual void CMPDI(OP_REG rs, OP_REG rt, OP_REG rd)
	{
		ConLog.Error("CMPDI r%d,r%d,r%d", rs, rt, rd);
	}
	virtual void ADDIC(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;
		const s32 lvalue = CPU.GPR[rt];
		const s32 rvalue = ext_s16(imm_s16);
		CPU.GPR[rs] = lvalue + rvalue;
		CPU.UpdateXER_CA(lvalue, rvalue);
	}
	virtual void ADDIC_(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;
		const s32 lvalue = CPU.GPR[rt];
		const s32 rvalue = ext_s16(imm_s16);
		const s32 value = lvalue + rvalue;
		CPU.GPR[rs] = value;
		CPU.UpdateXER_CA(lvalue, rvalue);
		CPU.UpdateCR0(value);
	}

	START_OPCODES_GROUP(G_0x04)
		START_OPCODES_GROUP(G_0x04_0x0)
			virtual void MULHHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				ConLog.Error("MULHHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				ConLog.Error("MULHHWU. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				ConLog.Error("MACHHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				ConLog.Error("MACHHWU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x0, "G_0x04_0x0");

		START_OPCODES_GROUP(G_0x04_0x1)
			virtual void MULHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHHW r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHHW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHHW. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHW r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACHHW r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x1, "G_0x04_0x1");

		START_OPCODES_GROUP(G_0x04_0x2)
			virtual void MACHHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				ConLog.Error("MACHHWSU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWSU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x2, "G_0x04_0x2");

		START_OPCODES_GROUP(G_0x04_0x3)
			virtual void MACHHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWS r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWS_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWS. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACHHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACHHWS r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x3, "G_0x04_0x3");

		START_OPCODES_GROUP(G_0x04_0x4)
			virtual void MULCHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULCHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULCHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULCHWU. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x4, "G_0x04_0x4");

		START_OPCODES_GROUP(G_0x04_0x5)
			virtual void MULCHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULCHW r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHW r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACCHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACCHW r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x5, "G_0x04_0x5");

		START_OPCODES_GROUP(G_0x04_0x6)
			virtual void MACCHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWSU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWSU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x6, "G_0x04_0x6");

		START_OPCODES_GROUP(G_0x04_0x7)
			virtual void MACCHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWS r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACCHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACCHWS r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACCHWS_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACCHWS. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x7, "G_0x04_0x7");

		START_OPCODES_GROUP(G_0x04_0xc)
			virtual void MULLHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLHWU. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACLHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACLHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0xc, "G_0x04_0xc");

		START_OPCODES_GROUP(G_0x04_0xd)
			virtual void NMACLHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACLHW r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0xd, "G_0x04_0xd");

		START_OPCODES_GROUP(G_0x04_0xe)
			virtual void MACLHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWSU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACLHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWSU. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0xe, "G_0x04_0xe");

		START_OPCODES_GROUP(G_0x04_0xf)
			virtual void MACLHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWS r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0xf, "G_0x04_0xf");

		START_OPCODES_GROUP(G_0x04_0x10)
			virtual void MACHHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x10, "G_0x04_0x10");

		START_OPCODES_GROUP(G_0x04_0x12)
			virtual void MACHHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWSUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWSUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x12, "G_0x04_0x12");

		START_OPCODES_GROUP(G_0x04_0x13)
			virtual void MACHHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWSO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACHHWSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACHHWSO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACHHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACHHWSO r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x13, "G_0x04_0x13");

		START_OPCODES_GROUP(G_0x04_0x14)
			virtual void MACCHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x14, "G_0x04_0x14");

		START_OPCODES_GROUP(G_0x04_0x15)
			virtual void MACCHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACCHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACCHWO r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x15, "G_0x04_0x15");

		START_OPCODES_GROUP(G_0x04_0x16)
			virtual void MACCHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWSUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACCHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACCHWSUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x16, "G_0x04_0x16");

		START_OPCODES_GROUP(G_0x04_0x17)
			virtual void NMACCHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACCHWSO r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x17, "G_0x04_0x17");

		START_OPCODES_GROUP(G_0x04_0x18)
			virtual void MFVSCR(OP_REG rs)
			{
				ConLog.Error("MFVSCR r%d", rs);
			}
		END_OPCODES_GROUP(G_0x04_0x18, "G_0x04_0x18");

		START_OPCODES_GROUP(G_0x04_0x1c)
			virtual void MACLHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACLHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x1c, "G_0x04_0x1c");

		START_OPCODES_GROUP(G_0x04_0x1d)
			virtual void NMACLHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACLHWO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACLHWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACLHWO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x1d, "G_0x04_0x1d");

		START_OPCODES_GROUP(G_0x04_0x1e)
			virtual void MACLHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWSUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MACLHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MACLHWSUO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x1e, "G_0x04_0x1e");

		START_OPCODES_GROUP(G_0x04_0x1f)
			virtual void NMACLHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACLHWSO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NMACLHWSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("NMACLHWSO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G_0x04_0x1f, "G_0x04_0x1f");
	END_OPCODES_GROUP(G_0x04, "G_0x04");

	START_OPCODES_GROUP(G_0x0b)
	END_OPCODES_GROUP(G_0x0b, "G_0x0b");

	START_OPCODES_GROUP(G1)
		virtual void LI(OP_REG rs, OP_sREG imm_s16)
		{
			if(rs != 0) CPU.GPR[rs] = imm_s16;
		}
		virtual void ADDI(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			if(rs == 0) return;

			if(rt == 0)
			{
				CPU.GPR[rs] = imm_s16;
			}
			else
			{
				CPU.GPR[rs] = CPU.GPR[rt] + imm_s16;
			}
		}
	END_OPCODES_GROUP(G1, "G1");

	START_OPCODES_GROUP(G_0x0f)
		
			virtual void ADDIS(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
			{
				if(rs == 0) return;

				if(rt == 0)
				{
					CPU.GPR[rs] = imm_s16 << 16;
				}
				else
				{
					CPU.GPR[rs] = CPU.GPR[rt] + (imm_s16 << 16);
				}
			}
			virtual void LIS(OP_REG rs, OP_sREG imm_s16)
			{
				ConLog.Error("LIS");
			}
		
	END_OPCODES_GROUP(G_0x0f, "G_0x0f");

	START_OPCODES_GROUP(G2)
	END_OPCODES_GROUP(G2, "G2");
	
	virtual void SC(const u32 sys)
	{
		ConLog.Write("SYSCALL %08x: %d!", sys, CPU.GPR[11]);
		if(sys == 2) SysCall(CPU.GPR[11]);
	}

	START_OPCODES_GROUP(BRANCH)
		virtual void B(const u32 imm_u26)
		{
			CPU.SetBranch(branchTarget(imm_u26));
		}
		virtual void BL(const u32 imm_u26)
		{
			CPU.LR = CPU.PC + 4;
			CPU.SetBranch(branchTarget(imm_u26));
		}
	END_OPCODES_GROUP(BRANCH, "BRANCH");

	START_OPCODES_GROUP(G3_S)
		//virtual void BLR()
		START_OPCODES_GROUP(G3_S_0x0)
			virtual void MCRF(OP_REG crfs, OP_REG crft)
			{
				ConLog.Error("MCRF");
			}
			START_OPCODES_GROUP(G3_S_0x0_0x20)
			virtual void BLELR(OP_REG crft)
			{
				ConLog.Error("BLELR");
			}
			virtual void BEQLR()
			{
				ConLog.Error("BEQLR");
			}
			virtual void BLR()
			{
				ConLog.Error("BLR");
			}
			END_OPCODES_GROUP(G3_S_0x0_0x20, "G3_S_0x0_0x20");
		
		END_OPCODES_GROUP(G3_S_0x0, "G3_S_0x0");
		START_OPCODES_GROUP(G3_S_0x10)
			virtual void BCTR()
		{
			ConLog.Error("BCTR");
		}
		virtual void BCTRL()
		{
			ConLog.Error("BCTRL");
		}
		END_OPCODES_GROUP(G3_S_0x10, "G3_S_0x10");
		
	END_OPCODES_GROUP(G3_S, "G3_S");

	START_OPCODES_GROUP(G3_S0)
		virtual void RLWINM_()
		{
			ConLog.Error("RLWINM.");
		}
		virtual void RLWINM()
		{
			ConLog.Error("RLWINM");
		}
		
	END_OPCODES_GROUP(G3_S0, "G3_S0");

	START_OPCODES_GROUP(G3_S0_0x3e)
		virtual void ROTLWI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			ConLog.Error("ROTLWI");
		}
		virtual void CLRLWI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			ConLog.Error("CLRLWI");
		}
	END_OPCODES_GROUP(G3_S0_0x3e, "G3_S0_0x3e");
	START_OPCODES_GROUP(G3_S0_0x3f)
		virtual void ROTLWI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			ConLog.Error("ROTLWI.");
		}
		virtual void CLRLWI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			ConLog.Error("CLRLWI.");
		}
	END_OPCODES_GROUP(G3_S0_0x3f, "G3_S0_0x3f");

	virtual void ROTLW(OP_REG rt, OP_REG rs, OP_REG rd)
	{
		ConLog.Error("ROTLW");
	}
	virtual void ORI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | imm_u16;
	}
	virtual void ORIS(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | (imm_u16 << 16);
	}
	virtual void XORI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ imm_u16;
	}
	virtual void XORIS(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ (imm_u16 << 16);
	}
	virtual void ANDI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt == 0) return;
		const s64 value = CPU.GPR[rs] & imm_u16;
		CPU.GPR[rt] = value;

		CPU.UpdateCR0(value);
	}
	virtual void ANDIS_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if (rt == 0) return;
		const s64 value = CPU.GPR[rs] & (imm_u16 << 16);
		CPU.GPR[rt] = value;

		CPU.UpdateCR0(value);
	}
	//virtual void LRLDIÑ(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	//{
	//	ConLog.Error("LRLDIÑ");
	//}
	START_OPCODES_GROUP(G_0x1e)
			virtual void CLRLDI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
			{
				ConLog.Error("CLRLDI r%d,r%d,%d #%x", rs, rt, imm_u16, imm_u16);
			}
			virtual void CLRLDI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
			{
				ConLog.Error("CLRLDI_ r%d,r%d,%d #%x", rs, rt, imm_u16, imm_u16);
			}
		END_OPCODES_GROUP(G_0x1e, "G_0x1e");

	START_OPCODES_GROUP(G4)
		START_OPCODES_GROUP(G4_G0)
			//virtual void CMPW(OP_REG rs, OP_REG rt, OP_REG rd)
			//{
			//	ConLog.Error("CMPW r%d,r%d,r%d", rs, rt, rd);
			//}
			virtual void LVSL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVSL r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LVEBX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: LVEBX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHDU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHDU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHDU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHDU. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ADDC(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				const s64 _rt = CPU.GPR[rt];
				const s64 _rd = CPU.GPR[rd];

				CPU.GPR[rs] = _rt + _rd;

				CPU.UpdateXER_CA(_rt, _rd);
			}
			virtual void MULHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				CPU.GPR[rs] = ((CPU.GPR[rt] * CPU.GPR[rd]) >> 32);
			}
			virtual void MULHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				const s64 value = (CPU.GPR[rt] * CPU.GPR[rd]) >> 32;
				CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			//virtual void MFCR(OP_REG rs)
			//{
			//	ConLog.Error("MFCR r%d", rs);
			//}
			START_OPCODES_GROUP(G4_G0_0x26)
				virtual void MFCR(OP_REG rs)
				{
					if(rs != 0) CPU.GPR[rs] = CPU.CR[0];
				}
				virtual void MFOCRF(OP_REG rs, OP_sREG imm_s16)
				{
					ConLog.Error("MFOCRF");
				}
			END_OPCODES_GROUP(G4_G0_0x26, "G4_G0_0x26");

			virtual void LWARX(OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				ConLog.Error("LWARX");
			}
			virtual void LDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				CPU.GPR[rs] = rt == 0
					? Memory.Read64(CPU.GPR[rd])
					: Memory.Read64(CPU.GPR[rt] + CPU.GPR[rd]);
			}
			virtual void LWZX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LWZX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void CNTLZW(OP_REG rt, OP_REG rs)
			{
				if(rt == 0) return;
				for(u32 i=0, mask = 0x80000000; i<32; ++i, mask >>= 1)
				{
					if(mask & CPU.GPR[rs])
					{
						CPU.GPR[rt] = i;
						break;
					}
				}
			}
			virtual void AND(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt != 0) CPU.GPR[rt] = CPU.GPR[rs] & CPU.GPR[rd];
			}
			virtual void AND_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt == 0) return;
				const s64 value = CPU.GPR[rs] & CPU.GPR[rd];
				CPU.GPR[rt] = value;
				CPU.UpdateCR0(value);
			}
			virtual void MASKG(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("MASKG r%d,r%d,r%d", rt, rs, rd);
			}
		END_OPCODES_GROUP(G4_G0, "G4_G0");

		START_OPCODES_GROUP(G4_G1)
			virtual void CMPLD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("CMPLD r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LVSR(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVSR r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LVEHX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVEHX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void SUBF(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.GPR[rs] = CPU.GPR[rd] - CPU.GPR[rt];
			}
			virtual void SUBF_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				
				const s64 value = CPU.GPR[rd] - CPU.GPR[rt];
				CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void LDUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				
				CPU.GPR[rs] = rt==0 
					? Memory.Read64(CPU.GPR[rd]) 
					: Memory.Read64(CPU.GPR[rt] + CPU.GPR[rd]);
			}
			virtual void LWZUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0 || rt == rs) return;
				const s64 addr = CPU.GPR[rt] + CPU.GPR[rd];
				CPU.GPR[rs] = Memory.Read32(addr);
				if(rt != 0) CPU.GPR[rt] = addr;
			}
			virtual void CNTLZD(OP_REG rs, OP_REG rt)
			{
				if(rt == 0) return;
				for(u32 i=0, mask = 0x80000000; i<32; ++i, mask >>= 1)
				{
					if(mask & CPU.GPR[rs])
					{
						CPU.GPR[rt] = i;
						break;
					}
				}
			}
			virtual void ANDC(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt != 0) CPU.GPR[rt] = CPU.GPR[rs] & (~(CPU.GPR[rd]));
			}
		END_OPCODES_GROUP(G4_G1, "G4_G1");
		//virtual void MULHW(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	ConLog.Error("MULHW r%d,r%d,r%d", rs, rt, rd);
		//}
		START_OPCODES_GROUP(G4_G_0x2)
			virtual void LVEWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVEWX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHD r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULHD. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.GPR[rs] = (CPU.GPR[rt] * CPU.GPR[rt]) >> 32;
			}
			virtual void MULHW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				const s64 value = (CPU.GPR[rt] * CPU.GPR[rt]) >> 32;
				CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void DLMZB(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("DLMZB r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void DLMZB_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("DLMZB. r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void LDARX(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("LDARX r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void DCBF(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("DCBF r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void LBZX(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rs == 0) return;
				
				CPU.GPR[rs] = rt==0 
					? Memory.Read8(CPU.GPR[rd]) 
					: Memory.Read8(CPU.GPR[rt] + CPU.GPR[rd]);
			}
		END_OPCODES_GROUP(G4_G_0x2, "G4_G_0x2");

		START_OPCODES_GROUP(G4_G2)
			virtual void LVX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void NEG(OP_REG rs, OP_REG rt)
			{
				if(rs != 0) CPU.GPR[rs] = ~CPU.GPR[rt];
			}
			virtual void MUL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.GPR[rs] = CPU.GPR[rt] * CPU.GPR[rd];
			}
			virtual void MUL_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				CPU.GPR[rs] = CPU.GPR[rt] * CPU.GPR[rd];
				CPU.UpdateCR0(CPU.GPR[rs]);
			}
			virtual void MTSRDIN(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("MTSRDIN r%d,r%d", rs, rd);
			}
			//virtual void NOT(OP_REG rt, OP_REG rd)
			//{
			//	ConLog.Error("NOT r%d,r%d", rt, rd);
			//}
		END_OPCODES_GROUP(G4_G2, "G4_G2");

		START_OPCODES_GROUP(G4_G3)
			virtual void WRTEE(OP_REG rs)
			{
				ConLog.Error("WRTEE r%d", rs);
			}
			virtual void STVEBX(OP_REG rs, OP_REG rt, OP_REG rd) //FIXME
			{
				ConLog.Error("STVEBX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void SUBFE(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				const s64 _rt = ~CPU.GPR[rt];
				const s64 _rd = CPU.GPR[rd];
				const s64 _ca = CPU.IsXER_CA() ? 1 : 0;

				if(rs != 0) CPU.GPR[rs] = _rt + _rd + _ca;
				CPU.UpdateXER_CA(_rt, _rd, _ca);
			}
			virtual void SUBFE_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				const s64 _rt = ~CPU.GPR[rt];
				const s64 _rd = CPU.GPR[rd];
				const s64 _ca = CPU.IsXER_CA() ? 1 : 0;

				const s64 value = _rt + _rd + _ca;
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
				CPU.UpdateXER_CA(_rt, _rd, _ca);
			}
			virtual void ADDE(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				const s64 _rt = CPU.GPR[rt];
				const s64 _rd = CPU.GPR[rd];
				const s64 _ca = CPU.IsXER_CA() ? 1 : 0;

				if(rs != 0) CPU.GPR[rs] = _rt + _rd + _ca;
				CPU.UpdateXER_CA(_rt, _rd, _ca);
			}
			virtual void ADDE_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				const s64 _rt = CPU.GPR[rt];
				const s64 _rd = CPU.GPR[rd];
				const s64 _ca = CPU.IsXER_CA() ? 1 : 0;

				const s64 value = _rt + _rd + _ca;
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
				CPU.UpdateXER_CA(_rt, _rd, _ca);
			}
			//virtual void MTOCRF(OP_REG rs) //FIXME
			//{
			//	ConLog.Error("Unimplemented: MTOCRF r%d", rs);
			//}
			virtual void STDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				if(rt == 0)
				{
					Memory.Write64(CPU.GPR[rs], CPU.GPR[rd]);
				}
				else
				{
					Memory.Write64(CPU.GPR[rs], CPU.GPR[rt] + CPU.GPR[rd]);
				}
			}
			virtual void STWCX_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("STWCX. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void STWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				if(rt == 0)
				{
					Memory.Write32(CPU.GPR[rs], CPU.GPR[rd]);
				}
				else
				{
					Memory.Write32(CPU.GPR[rs], CPU.GPR[rt] + CPU.GPR[rd]);
				}
			}
			virtual void SLQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SLQ r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void SLQ_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SLQ. r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void SLE_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SLE. r%d,r%d,r%d", rt, rs, rd);
			}
		END_OPCODES_GROUP(G4_G3, "G4_G3");
		virtual void STDUX(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("STDUX r%d,r%d,r%d", rs, rt, rd);
		}
		//virtual void ADDZE(OP_REG rs, OP_REG rt)
		//{
		//	ConLog.Error("ADDZE r%d,r%d", rs, rt);
		//}
		START_OPCODES_GROUP(G4_G4)
			virtual void STVX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("STVX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLD r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLD. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ADDME(OP_REG rs, OP_REG rt)
			{
				const s8 _ca = CPU.IsXER_CA() - 1;
				const s64 _rt = CPU.GPR[rt];
				if(rs != 0) CPU.GPR[rs] = _rt + _ca;
				CPU.UpdateXER_CA(_rt, _ca);
			}
			virtual void ADDME_(OP_REG rs, OP_REG rt)
			{
				const s8 _ca = CPU.IsXER_CA() - 1;
				const s64 _rt = CPU.GPR[rt];
				const s64 value = _rt + _ca;
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
				CPU.UpdateXER_CA(_rt, _ca);
			}
			virtual void MULLW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.GPR[rs] = (CPU.GPR[rt] * CPU.GPR[rd]) & 0xffffffffll;
			}
			virtual void MULLW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				const s64 value = (CPU.GPR[rt] * CPU.GPR[rd]) & 0xffffffffll;
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void DCBTST(OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DCBTST r%d,r%d", rt, rd);
			}
			virtual void SLLIQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SLLIQ r%d,r%d,r%d", rt, rs, rd);
			}
		END_OPCODES_GROUP(G4_G4, "G4_G4");

		START_OPCODES_GROUP(G4_G_ADD)
			virtual void ICBT(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: ICBT (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DOZ(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DOZ r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DOZ_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DOZ. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ADD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				 if (rs != 0) CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd];
			}
			virtual void ADD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				 if (rs == 0) return;
				 CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd];
				 CPU.UpdateCR0(CPU.GPR[rs]);
			}
			virtual void LSCBX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LSCBX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LSCBX_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LSCBX. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DCBT(OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DCBT r%d,r%d", rt, rd);
			}
			virtual void LHZX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;

				CPU.GPR[rs] = rt==0 
					? Memory.Read16(CPU.GPR[rd]) 
					: Memory.Read16(CPU.GPR[rt] + CPU.GPR[rd]);
			}
			virtual void EQV(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: EQV (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void EQV_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: EQV. (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G4_G_ADD, "G4_G_ADD");

		START_OPCODES_GROUP(G4_G_ADDZE)
			virtual void ADDZE(OP_REG rs, OP_REG rt)
			{
				const s64 _rt = CPU.GPR[rt];
				const s8 _ca = CPU.IsXER_CA();
				if(rs != 0) CPU.GPR[rs] = _rt + _ca;
				CPU.UpdateXER_CA(_rt, _ca);
			}
			virtual void ADDZE_(OP_REG rs, OP_REG rt)
			{
				const s64 _rt = CPU.GPR[rt];
				const s8 _ca = CPU.IsXER_CA();
				const s64 value = _rt + _ca;
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateXER_CA(_rt, _ca);
				CPU.UpdateCR0(value);
			}
		END_OPCODES_GROUP(G4_G_ADDZE, "G4_G_ADDZE");
		//virtual void XOR(OP_REG rt, OP_REG rs, OP_REG rd)
		//{
		//	 if (rd != 0) CPU.GPR[rd] = CPU.GPR[rs] ^ CPU.GPR[rt];
		//}
		virtual void SRW(OP_REG rt, OP_REG rs, OP_REG rd)
		{
			if(rt != 0) CPU.GPR[rt] = CPU.GPR[rs] >> (CPU.GPR[rd] & 0x1f);
		}
		//virtual void MFLR(OP_REG rs)
		//{
		//	ConLog.Error("MFLR r%d", rs);
		//}
		START_OPCODES_GROUP(G4_G_0x9)
			virtual void DOZI(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
			{
				ConLog.Error("DOZI");
			}
			virtual void LHZUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0 || rt == rs) return;
				const s64 value = CPU.GPR[rt] + CPU.GPR[rd];
				CPU.GPR[rs] = Memory.Read16(value);
				CPU.GPR[rt] = value;
			}
			virtual void XOR(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ CPU.GPR[rd];
			}
		END_OPCODES_GROUP(G4_G_0x9, "G4_G_0x9");

		START_OPCODES_GROUP(G4_G_0xa)
			virtual void MFDCR(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: MFDCR (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIV(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				 if (rs != 0 && CPU.GPR[rd] != 0) CPU.GPR[rs] = CPU.GPR[rt] / CPU.GPR[rd];
			}
			virtual void DIV_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				 if (rs == 0 && CPU.GPR[rd] == 0) return;
				 CPU.GPR[rs] = CPU.GPR[rt] / CPU.GPR[rd];
				 CPU.UpdateCR0(CPU.GPR[rs]);
			}
			//virtual void MFCTR(OP_REG rs)
			//{
			//	ConLog.Error("MFCTR r%d", rs);
			//}
			START_OPCODES_GROUP(G4_G_0xa_0x26)
				virtual void MFSPR(OP_REG rs, OP_sREG imm_s16)
				{
					ConLog.Error("MFSPR");
				}
				virtual void MFLR(OP_REG rs)
				{
					ConLog.Error("MFLR r%d", rs);
				}
				virtual void MFCTR(OP_REG rs)
				{
					ConLog.Error("MFCTR r%d", rs);
				}
			END_OPCODES_GROUP(G4_G_0xa_0x26, "G4_G_0xa_0x26");

			virtual void LWAX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LWAX r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G4_G_0xa, "G4_G_0xa");

		START_OPCODES_GROUP(G4_G_0xb)
			virtual void LVXL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVXL v%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MFTB(OP_REG rs)
			{
				ConLog.Error("MFTB r%d", rs);
			}
		END_OPCODES_GROUP(G4_G_0xb, "G4_G_0xb");

		START_OPCODES_GROUP(G4_G5)
			virtual void STHX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				if(rt == 0)
				{
					Memory.Write16(CPU.GPR[rs], CPU.GPR[rd]);
				}
				else
				{
					Memory.Write16(CPU.GPR[rs], CPU.GPR[rt] + CPU.GPR[rd]);
				}
			}
			virtual void ORC(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | (~CPU.GPR[rd]);
			}
			virtual void ORC_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				const s64 value = CPU.GPR[rs] | (~CPU.GPR[rd]);
				if(rt != 0) CPU.GPR[rt] = value;
				CPU.UpdateCR0(value);
			}
		END_OPCODES_GROUP(G4_G5, "G4_G5");
		//virtual void MR(OP_REG rt, OP_REG rs)
		//{
		//	ConLog.Error("MR r%d,r%d", rt, rs);
		//}
		START_OPCODES_GROUP(G4_G6)
			virtual void MTDCR(OP_REG rs) //FIXME
			{
				ConLog.Error("Unimplemented: MTDCR r%d", rs);
			}
			virtual void DCCCI(OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DCCCI r%d,r%d", rt, rd);
			}
			virtual void DIVDU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVDU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVDU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVDU. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0 && (s32)CPU.GPR[rd] != 0)
					CPU.GPR[rs] = (s32)CPU.GPR[rt] / (s32)CPU.GPR[rd];
			}
			virtual void DIVWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if((s32)CPU.GPR[rd] == 0) return;
				const s32 value = (s32)CPU.GPR[rt] / (s32)CPU.GPR[rd];
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void DCBI(OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DCBI r%d,r%d", rt, rd);
			}
			virtual void NAND(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				if(rt != 0) CPU.GPR[rt] = ~(CPU.GPR[rs] & CPU.GPR[rt]);
			}
			virtual void NAND_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				const s64 value = ~(CPU.GPR[rs] & CPU.GPR[rt]);
				if(rt != 0) CPU.GPR[rt] = value;
				CPU.UpdateCR0(value);
			}
			START_OPCODES_GROUP(G4_G6_G1)
				//virtual void MTSPR(OP_REG rs)
				//{
				//	ConLog.Error("MTSPR r%d", rs);
				//}
				virtual void MTLR(OP_REG rs)
				{
					ConLog.Error("MTLR r%d", rs);
				}
				virtual void MTCTR(OP_REG rs)
				{
					ConLog.Error("MTCTR r%d", rs);
				}
			END_OPCODES_GROUP(G4_G6_G1, "G4_G6_G1");
		END_OPCODES_GROUP(G4_G6, "G4_G6");
		//virtual void DIVW(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	ConLog.Error("DIVW r%d,r%d,r%d", rs, rt, rd);
		//}
		START_OPCODES_GROUP(G4_G_0xf)
			virtual void DIVD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVD r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVD. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0 || CPU.GPR[rd] == 0 ||
					(CPU.GPR[rd] == -1 && CPU.GPR[rt] == 0x80000000)) return;

				CPU.GPR[rs] = CPU.GPR[rt] / CPU.GPR[rd];
			}
		END_OPCODES_GROUP(G4_G_0xf, "G4_G_0xf");

		START_OPCODES_GROUP(G4_G_ADDCO)
			virtual void MCRXR(OP_REG rs)
			{
				ConLog.Error("MCRXR r%d", rs);
			}
			virtual void LVLX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVLX r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ADDCO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				CPU.UpdateXER_SO_OV(CPU.GPR[rt], CPU.GPR[rd]);
				CPU.UpdateXER_CA(CPU.CheckOverflow(CPU.GPR[rt], CPU.GPR[rd]));
				if(rs != 0) CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd];
			}
			virtual void ADDCO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				CPU.UpdateXER_SO_OV(CPU.GPR[rt], CPU.GPR[rd]);
				CPU.UpdateXER_CA(CPU.CheckOverflow(CPU.GPR[rt], CPU.GPR[rd]));
				const s64 value = CPU.GPR[rt] + CPU.GPR[rd];
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void LSWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: LSWX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LWBRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: LWBRX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void LFSX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.FPR[rs] = rt == 0 ? 
					Memory.Read32(CPU.GPR[rd]) :
					Memory.Read32(CPU.GPR[rt] + CPU.GPR[rd]);
			}
			virtual void MASKIR(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("MASKIR r%d,r%d,r%d", rt, rs, rd);
			}
		END_OPCODES_GROUP(G4_G_ADDCO, "G4_G_ADDCO");
		START_OPCODES_GROUP(G4_G_0x11)
			virtual void LVRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVRX f%d,r%d,r%d", rs, rt, rd);
			}
			virtual void SUBFO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				CPU.UpdateXER_SO_OV(CPU.GPR[rd], CPU.GPR[rt]);
				CPU.GPR[rs] = CPU.GPR[rd] - CPU.GPR[rt];
			}
		END_OPCODES_GROUP(G4_G_0x11, "G4_G_0x11");
		
		START_OPCODES_GROUP(G4_G_0x12)
			START_OPCODES_GROUP(G4_G_0x12_0x26)
				virtual void MFSR(OP_REG rs, OP_sREG imm_s16)
				{
					ConLog.Error("MFSR");
				}
			END_OPCODES_GROUP(G4_G_0x12_0x26, "G4_G_0x12_0x26");

			virtual void LWSYNC() //FIXME
			{
				ConLog.Error("Unimplemented: LWSYNC");
			}
			virtual void LFDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.FPR[rs] = rt == 0 ? 
					Memory.Read64(CPU.GPR[rd]) :
					Memory.Read64(CPU.GPR[rt] + CPU.GPR[rd]);
			}
		END_OPCODES_GROUP(G4_G_0x12, "G4_G_0x12");

		START_OPCODES_GROUP(G4_G7)
			virtual void NEGO() //FIXME
			{
				ConLog.Error("Unimplemented: NEGO");
			}
			virtual void MULO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULO f%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULO. f%d,r%d,r%d", rs, rt, rd);
			}

			START_OPCODES_GROUP(G4_G7_0x26)
				virtual void MFSRI(OP_REG rs, OP_REG rt, OP_REG rd)
				{
					ConLog.Error("MFSRI f%d,r%d,r%d", rs, rt, rd);
				}
			END_OPCODES_GROUP(G4_G7_0x26, "G4_G7_0x26");
		END_OPCODES_GROUP(G4_G7, "G4_G7");
		//virtual void STFSX(OP_REG rs, OP_REG rt, OP_REG rd) //FIXME
		//{
		//	ConLog.Error("STFSX f%d,r%d,r%d", rs, rt, rd);
		//}
		START_OPCODES_GROUP(G4_G_ADDEO)
			virtual void ADDEO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				CPU.UpdateXER_SO_OV(CPU.GPR[rt], CPU.GPR[rd], CPU.IsXER_CA());
				CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd] + CPU.IsXER_CA();
				CPU.UpdateXER_CA(CPU.GPR[rt], CPU.GPR[rd], CPU.IsXER_CA());
			}
			virtual void ADDEO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs == 0) return;
				const s64 value = CPU.GPR[rt] + CPU.GPR[rd] + CPU.IsXER_CA();
				CPU.UpdateXER_SO_OV(CPU.GPR[rt], CPU.GPR[rd],CPU.IsXER_CA());
				CPU.GPR[rs] = value;
				CPU.UpdateXER_CA(CPU.GPR[rt], CPU.GPR[rd],CPU.IsXER_CA());
				CPU.UpdateCR0(value);
			}

			START_OPCODES_GROUP(G4_G_ADDEO_0x26)
				virtual void MFSRIN(OP_REG rs, OP_REG rd)
				{
					ConLog.Error("MFSRIN f%d,r%d", rs, rd);
				}
			END_OPCODES_GROUP(G4_G_ADDEO_0x26, "G4_G_ADDEO_0x26");
		END_OPCODES_GROUP(G4_G_ADDEO, "G4_G_ADDEO");

		START_OPCODES_GROUP(G4_G8)
			virtual void ADDZEO_(OP_REG rs, OP_REG rt, OP_REG rd) //temp
			{
				ConLog.Error("Unimplemented: ADDZEO. (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void STSWI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("STSWI r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void STFDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("STFDX f%d,r%d,r%d", rs, rt, rd);
			}
			virtual void SRLQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SRLQ r%d,r%d,r%d", rt, rs, rd);
			}
			virtual void SREQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				ConLog.Error("SREQ r%d,r%d,r%d", rt, rs, rd);
			}
		END_OPCODES_GROUP(G4_G8, "G4_G8");

		START_OPCODES_GROUP(G4_G_ADDO)
			virtual void LVLXL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("LVLXL f%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DOZO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DOZO f%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ADDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[rt], CPU.GPR[rd]));
				if(rs != 0) CPU.GPR[rs] = CPU.GPR[rt] + CPU.GPR[rd];
			}
			virtual void ADDO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				CPU.UpdateXER_SO_OV(CPU.CheckOverflow(CPU.GPR[rt], CPU.GPR[rd]));
				const s64 value = CPU.GPR[rt] + CPU.GPR[rd];
				if(rs != 0) CPU.GPR[rs] = value;
				CPU.UpdateCR0(value);
			}
			virtual void LHBRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				if(rs != 0) CPU.GPR[rs] = rt == 0 ?
					Memory.Read16(CPU.GPR[rd]) :
					Memory.Read16(CPU.GPR[rt] + CPU.GPR[rd]);
			}
		END_OPCODES_GROUP(G4_G_ADDO, "G4_G_ADDO");

		START_OPCODES_GROUP(G4_G9)
			virtual void MULLDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLDO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLDO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLDO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLWO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MULLWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("MULLWO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G4_G9, "G4_G9");
		//virtual void SRAW(OP_REG rt, OP_REG rs, OP_REG rd)
		//{
		//	ConLog.Error("SRAW r%d,r%d,r%d", rt, rs, rd);
		//}
		virtual void SRAWI(OP_REG rt, OP_REG rs, OP_REG rd)
		{
			ConLog.Error("SRAWI r%d,r%d,r%d", rt, rs, rd);
		}
		//virtual void EIEIO()
		//{
		//	ConLog.Error("EIEIO");
		//}
		START_OPCODES_GROUP(G4_G_0x1a)
			virtual void DIVO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void EIEIO()
			{
				ConLog.Error("EIEIO");
			}
		END_OPCODES_GROUP(G4_G_0x1a, "G4_G_0x1a");

		START_OPCODES_GROUP(G4_G10)
			virtual void ABSO_() //FIXME
			{
				ConLog.Error("Unimplemented: ABSO.");
			}
			virtual void DIVSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVSO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVSO. r%d,r%d,r%d", rs, rt, rd);
			}
		END_OPCODES_GROUP(G4_G10, "G4_G10");

		START_OPCODES_GROUP(G4_G_0x1e)
			virtual void ICCCI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: ICCCI (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void RLDCL_(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				ConLog.Error("RLDCL.");
			}
			virtual void DIVDUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVDUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVDUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVDUO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVWUO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVWUO. r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void ICBI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: ICBI (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void RLDCL(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				ConLog.Error("RLDCL");
			}
			virtual void EXTSW(OP_REG rt, OP_REG rs)
			{
				ConLog.Error("EXTSW r%d,r%d", rt, rs);
			}
			virtual void EXTSW_(OP_REG rt, OP_REG rs)
			{
				ConLog.Error("EXTSW. r%d,r%d", rt, rs);
			}
		END_OPCODES_GROUP(G4_G_0x1e, "G4_G_0x1e");

		START_OPCODES_GROUP(G4_G_0x1f)
			virtual void ICREAD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("Unimplemented: ICREAD (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DIVDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DIVDO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void DCBZ(OP_REG rt, OP_REG rd)
			{
				ConLog.Error("DCBZ r%d,r%d", rt, rd);
			}
		END_OPCODES_GROUP(G4_G_0x1f, "G4_G_0x1f");

		virtual void EXTSH(OP_REG rt, OP_REG rs)
		{
			if(rt != 0) CPU.GPR[rt] = ext_s16(CPU.GPR[rs] & 0xff);
		}
		virtual void EXTSB(OP_REG rt, OP_REG rs)
		{	
			if(rt != 0) CPU.GPR[rt] = ext_s8(CPU.GPR[rs] & 0xff);
		}
		//virtual void EXTSW(OP_REG rt, OP_REG rs)
		//{
		//	ConLog.Error("EXTSW r%d,r%d", rt, rs);
		//}
		//virtual void DCBZ(OP_REG rt, OP_REG rd)
		//{
		//	ConLog.Error("DCBZ r%d,r%d", rt, rd);
		//}
		//virtual void LWZX(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	ConLog.Error("LWZX r%d,r%d,r%d", rs, rt, rd);
		//}
	END_OPCODES_GROUP(G4, "G4");
	//virtual void MFLR(OP_REG rs)=0;
	//
	virtual void LWZ(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) CPU.GPR[rs] = Memory.Read32(CPU.GPR[rt] + imm_s16);
	}
	virtual void LWZU(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		OP_REG addr = CPU.GPR[rt] + imm_s16;

		CPU.GPR[rs] = Memory.Read32(addr);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void LBZ(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) CPU.GPR[rs] = Memory.Read8(CPU.GPR[rt] + imm_s16);
	}
	virtual void LBZU(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const s64 addr = CPU.GPR[rt] + imm_s16;
		CPU.GPR[rs] = Memory.Read8(addr);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void STW(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) Memory.Write32(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}
	virtual void STWU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 addr = CPU.GPR[rt] + imm_s16;
		Memory.Write32(addr, CPU.GPR[rs]);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void STB(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) Memory.Write8(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}
	virtual void STBU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 addr = CPU.GPR[rt] + imm_s16;
		Memory.Write8(addr, CPU.GPR[rs]);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void LHZ(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) CPU.GPR[rs] = Memory.Read16(CPU.GPR[rt] + imm_s16);
	}
	virtual void LHZU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const s64 addr = CPU.GPR[rt] + imm_s16;
		CPU.GPR[rs] = Memory.Read16(addr);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void LHA(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;
		CPU.GPR[rs] = Memory.Read16(CPU.GPR[rt] + imm_s16);
	}
	virtual void LHAU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const uint addr = CPU.GPR[rt] + imm_s16;

		CPU.GPR[rs] = Memory.Read16(addr);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void STH(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs != 0) Memory.Write16(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}
	virtual void STHU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 addr = CPU.GPR[rt] + imm_s16;
		Memory.Write16(addr, CPU.GPR[rs]);
		if(rt != 0) CPU.GPR[rt] = addr;
	}
	virtual void LMW(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;
		s64 pc = rt != 0 ? CPU.GPR[rt] + imm_s16 : imm_s16;

		for(uint i=rs; i<32; ++i, pc += 4)
		{
			CPU.GPR[i] = Memory.Read32(pc);
		}
	}
	virtual void LFS(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		CPU.FPR[rs] = Memory.Read32(offs);
	}
	virtual void LFSU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		CPU.FPR[rs] = Memory.Read32(offs);
		if(rt != 0) CPU.GPR[rt] = offs;
	}
	virtual void LFD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		CPU.FPR[rs] = Memory.Read64(offs);
	}
	virtual void LFDU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		CPU.FPR[rs] = Memory.Read64(offs);
		if(rt != 0) CPU.GPR[rt] = offs;
	}
	virtual void STFS(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		Memory.Write32(offs, (u32)CPU.FPR[rs]);
	}
	virtual void STFSU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		Memory.Write32(offs, (u32)CPU.FPR[rs]);
		if(rt != 0) CPU.GPR[rt] = offs;
	}
	virtual void STFD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(rs == 0) return;

		const s64 offs = rt == 0 ? imm_s16 : CPU.GPR[rt] + imm_s16;
		Memory.Write64(offs, (u32)CPU.FPR[rs]);
	}

	START_OPCODES_GROUP(G_0x3a)
		virtual void LDU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			if(rs == 0 || rs == rt) return;

			const s64 addr = CPU.GPR[rs] + imm_s16;
			CPU.GPR[rs] = Memory.Read64(addr);
			if(rt != 0) CPU.GPR[rt] = addr;
		}
		virtual void LWA(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			if(rs != 0) CPU.GPR[rs] = Memory.Read32(CPU.GPR[rt] + imm_s16);
		}
		virtual void LD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			if(rs != 0) CPU.GPR[rs] = Memory.Read64(CPU.GPR[rt] + imm_s16);
		}
	END_OPCODES_GROUP(G_0x3a, "G_0x3a");

	START_OPCODES_GROUP(G4_S)
		virtual void FSUBS(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FSUBS r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FSUBS_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FSUBS. r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FADDS(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FADDS r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FSQRTS(OP_REG rs, OP_REG rd)
		{
			ConLog.Error("FSQRTS r%d,r%d", rs, rd);
		}
		virtual void FRES(OP_REG rs, OP_REG rd)
		{
			ConLog.Error("FRES r%d,r%d", rs, rd);
		}
		virtual void FMULS(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			ConLog.Error("FMULS r%d,r%d,r%d", rs, rt, sa);
		}
		virtual void FMULS_(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			ConLog.Error("FMULS. r%d,r%d,r%d", rs, rt, sa);
		}
		virtual void FMADDS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMADDS r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMADDS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMADDS. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMSUBS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMSUBS r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMSUBS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMSUBS. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMSUBS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMSUBS r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMSUBS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMSUBS. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMADDS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMADDS r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMADDS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMADDS. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
	END_OPCODES_GROUP(G4_S, "G4_S");

	virtual void STD(OP_REG rs, OP_REG rt, OP_sREG imm_s16) //CHECK ME!
	{
		//if imm_s16 < 0 then STDU else STD
		Memory.Write32(CPU.GPR[rt] + (imm_s16 < 0 ? imm_s16 - 1 : imm_s16), CPU.GPR[rs]);
	}

	START_OPCODES_GROUP(G5)
		START_OPCODES_GROUP(G5_G0x0)
			virtual void FCMPU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("FCMPU r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void FCMPO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				ConLog.Error("FCMPO r%d,r%d,r%d", rs, rt, rd);
			}
			virtual void MCRFS(OP_REG rs, OP_REG rt)
			{
				ConLog.Error("MCRFS r%d,r%d", rs, rt);
			}
		END_OPCODES_GROUP(G5_G0x0, "G5_G0x0");

		START_OPCODES_GROUP(G5_G0x10)
			virtual void FNEG(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FNEG r%d,r%d", rs, rd);
			}
			virtual void FMR(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FMR r%d,r%d", rs, rd);
			}
			virtual void FNABS(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FNABS r%d,r%d", rs, rd);
			}
			virtual void FABS(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FABS r%d,r%d", rs, rd);
			}
		END_OPCODES_GROUP(G5_G0x10, "G5_G0x10");

		START_OPCODES_GROUP(G5_G0x1c)
			virtual void FCTIW(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FCTIW r%d,r%d", rs, rd);
			}
			virtual void FCTID(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FCTID r%d,r%d", rs, rd);
			}
			virtual void FCFID(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FCFID r%d,r%d", rs, rd);
			}
		END_OPCODES_GROUP(G5_G0x1c, "G5_G0x1c");

		START_OPCODES_GROUP(G5_G0x1e)
			virtual void FCTIWZ(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FCTIWZ r%d,r%d", rs, rd);
			}
			virtual void FCTIDZ(OP_REG rs, OP_REG rd)
			{
				ConLog.Error("FCTIDZ r%d,r%d", rs, rd);
			}
		END_OPCODES_GROUP(G5_G0x1e, "G5_G0x1e");
		//virtual void MFFS(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	ConLog.Error("Unimplemented: MFFS (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd);
		//}
		virtual void FDIV(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FDIV r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FDIV_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FDIV. r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FSUB(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FSUB r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FSUB_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FSUB. r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FADD(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FADD r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FADD_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			ConLog.Error("FADD. r%d,r%d,r%d", rs, rt, rd);
		}
		virtual void FSQRT(OP_REG rs, OP_REG rd)
		{
			ConLog.Error("FSQRT r%d,r%d", rs, rd);
		}
		virtual void FSEL(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FSEL r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FSEL_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FSEL. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMUL(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			ConLog.Error("FMUL r%d,r%d,r%d", rs, rt, sa);
		}
		virtual void FMUL_(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			ConLog.Error("FMUL. r%d,r%d,r%d", rs, rt, sa);
		}
		virtual void FRSQRTE(OP_REG rs, OP_REG rd)
		{
			ConLog.Error("FRSQRTE r%d,r%d", rs, rd);
		}
		virtual void FMSUB(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMSUB r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMSUB_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMSUB. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FMADD(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FMADD r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMSUB(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMSUB r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMSUB_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMSUB. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMADD(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMADD r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
		virtual void FNMADD_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			ConLog.Error("FNMADD. r%d,r%d,r%d,r%d", rs, rt, sa, rd);
		}
	END_OPCODES_GROUP(G5, "G5");
	
	virtual void UNK(OP_REG code, OP_REG opcode, OP_REG rs, OP_REG rt,
		OP_REG rd, OP_REG sa, OP_REG func, OP_sREG imm_s16, OP_REG imm_u16, OP_REG imm_u26)
	{
		ConLog.Error
		(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP