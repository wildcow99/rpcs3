#include "Emu/Cell/SPUOpcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/SPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(x)

class SPU_Interpreter
	: public SPU_Opcodes
	//, public SysCalls
{
private:
	//Display* display;
	SPUThread& CPU;

public:
	SPU_Interpreter(SPUThread& cpu)
		: CPU(cpu)
		//, SysCalls(cpu)
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
		//CPU.GPR[3] = DoSyscall(CPU.GPR[11]);
		//ConLog.Write("SysCall[%d] done with code [%d]!", CPU.GPR[11], CPU.GPR[3]);
	}

	virtual void NOP()
	{
		//__asm nop
	}


	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		CPU.GPR[rt] = ra != 0 ? CPU.GPR[ra] + simm16 : simm16;
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
	/*
		if(Memory.MemFlags.IsFlag(code))
		{
			ConLog.Warning("Flag: %x", code);
			CPU.SetBranch(code);
			return;
		}
	*/

		switch(code)
		{
		case 0xe6f2c1e7: ConLog.Warning("process exit #pc: 0x%x", CPU.PC); break;
		case 0x67f9fedb: ConLog.Warning("process exit spawn 2 #pc: 0x%x", CPU.PC); break;

		case 0xaff080a4: ConLog.Warning("ppu thread exit #pc: 0x%x", CPU.PC); break;
		case 0x24a1ea07: ConLog.Warning("ppu thread create ex #pc: 0x%x", CPU.PC); break;
		case 0x350d454e: ConLog.Warning("ppu thread get id #pc: 0x%x", CPU.PC); break;
		case 0x3dd4a957: ConLog.Warning("ppu thread register atexit #pc: 0x%x", CPU.PC); break;
		case 0x4a071d98: ConLog.Warning("interrupt thread disestablish #pc: 0x%x", CPU.PC); break;
		case 0xa3e3be68: ConLog.Warning("ppu thread once #pc: 0x%x", CPU.PC); break;
		case 0xac6fc404: ConLog.Warning("ppu thread unregister atexit #pc: 0x%x", CPU.PC); break;
		
		case 0x2f85c0ef: ConLog.Warning("lwmutex create #pc: 0x%x", CPU.PC); break;
		case 0xc3476d0c: ConLog.Warning("lwmutex destroy #pc: 0x%x", CPU.PC); break;
		case 0x1573dc3f: ConLog.Warning("lwmutex lock #pc: 0x%x", CPU.PC); break;
		case 0xaeb78725: ConLog.Warning("lwmutex trylock #pc: 0x%x", CPU.PC); break;
		case 0x1bc200f4: ConLog.Warning("lwmutex unlock #pc: 0x%x", CPU.PC); break;
		
		case 0x744680a2: ConLog.Warning("initialize tls #pc: 0x%x", CPU.PC); break;
		case 0x8461e528: ConLog.Warning("get system time #pc: 0x%x", CPU.PC); break;
		case 0xa2c7ba64: ConLog.Warning("prx exitspawn with level #pc: 0x%x", CPU.PC); break;

		default:
			UNK(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
		break;
		}
	}

	void UNK(const wxString& err)
	{
		ConLog.Error(err + wxString::Format(" #pc: 0x%x", CPU.PC));
		Emu.Pause();
		for(uint i=0; i<128; ++i) ConLog.Write("r%d = 0x%x", i, CPU.GPR[i]);
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP