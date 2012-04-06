#include "stdafx.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUInterpreter.h"
#include "Emu/Cell/PPUDisAsm.h"

#include "Emu/SysCalls/SysCalls.h"

PPUThread::PPUThread() : PPCThread(false)
{
	Reset();
}

PPUThread::~PPUThread()
{
	//~PPCThread();
}

void PPUThread::DoReset()
{
	//reset regs
	memset(VPR,	 0, sizeof(VPR));
	memset(FPR,  0, sizeof(FPR));
	memset(GPR,  0, sizeof(GPR));
	memset(SPRG, 0, sizeof(SPRG));
	
	CR.CR	= 0;
	LR		= 0;
	CTR		= 0;
	USPRG	= 0;
	TB		= 0;
	XER.XER	= 0;
	FPSCR.FPSCR	= 0;

	cycle = 0;

	reserve = false;
	reserve_addr = 0;
}

void PPUThread::SetBranch(const u32 pc)
{
	u64 fid, waddr;
	if(Memory.MemFlags.IsFlag(pc, waddr, fid))
	{
		GPR[3] = SysCallsManager.DoFunc(fid, *this);

		if((s32)GPR[3] < 0) ConLog.Write("Func[0x%llx] done with code [0x%x]! #pc: 0x%x", fid, (u32)GPR[3], PC);
#ifdef HLE_CALL_LOG
		else ConLog.Write("Func[0xll%x] done with code [0x%llx]! #pc: 0x%x", fid, GPR[3], PC);
#endif
		//ConLog.Warning("Func waddr: 0x%llx", waddr);
		Memory.Write32(waddr, Emu.GetTLSAddr());
		Memory.Write32(Emu.GetTLSAddr(), PC + 4);
		Memory.Write32(Emu.GetTLSAddr()+4, Emu.GetTLSMemsz());
	}

	PPCThread::SetBranch(pc);
}

void PPUThread::_InitStack()
{
	const u32 entry = Memory.Read32(PC);
	const u32 rtoc = Memory.Read32(PC + 4);

	SetPc(entry);
	
	GPR[1] = stack_point;
	GPR[2] = rtoc;
	GPR[3] = m_arg;
	GPR[4] = stack_addr + stack_size - 0x40;
	GPR[5] = stack_addr + stack_size - 0x50;
	GPR[7] = 0x80d90;
	GPR[8] = entry;
	GPR[10] = 0x131700;
	GPR[11] = 0x80;
	GPR[12] = Emu.GetMallocPageSize();
	GPR[13] = 0x10090000;
	GPR[28] = GPR[4];
	GPR[29] = GPR[3];
	GPR[31] = GPR[5];

	//Memory.WriteString(GPR[4], "PATH1");
	//Memory.WriteString(GPR[5], "PATH2");

	Memory.Write32(GPR[4], Emu.GetTLSFilesz());
	Memory.Write32(GPR[4]+4, Emu.GetTLSMemsz());
}

u64 PPUThread::GetFreeStackSize() const
{
	return (GetStackAddr() + GetStackSize()) - GPR[1];
}

void PPUThread::DoRun()
{
	switch(Ini.CPUDecoderMode.GetValue())
	{
	case 0:
		m_dec = new PPU_Decoder(*new PPU_DisAsm(*this));
	break;

	case 1:
	case 2:
		m_dec = new PPU_Decoder(*new PPU_Interpreter(*this));
	break;
	}
}

void PPUThread::DoResume()
{
}

void PPUThread::DoPause()
{
}

void PPUThread::DoStop()
{
	if(m_dec)
	{
		(*(PPU_Decoder*)m_dec).~PPU_Decoder();
		safe_delete(m_dec);
	}
}

bool dump_enable = false;

void PPUThread::DoCode(const s32 code)
{
	if(dump_enable)
	{
		static wxFile f("dump.txt", wxFile::write);
		static PPU_DisAsm disasm(*this, DumpMode);
		static PPU_Decoder decoder(disasm);
		disasm.dump_pc = PC;
		decoder.Decode(code);
		f.Write(disasm.last_opcode);
	}

	if(++cycle > 220)
	{
		cycle = 0;
		TB++;
	}

	(*(PPU_Decoder*)m_dec).Decode(code);
}

bool FPRdouble::IsINF(double d)
{
	return wxFinite(d) ? 1 : 0;
}

bool FPRdouble::IsNaN(double d)
{
	return wxIsNaN(d) ? 1 : 0;
}

bool FPRdouble::IsQNaN(double d)
{
	FPRdouble x(d);

	return
		((x.i & DOUBLE_EXP) == DOUBLE_EXP) &&
		((x.i & 0x0007fffffffffffULL) == DOUBLE_ZERO) &&
		((x.i & 0x000800000000000ULL) == 0x000800000000000ULL);
}

bool FPRdouble::IsSNaN(double d)
{
	FPRdouble x(d);
	
	return
		((x.i & DOUBLE_EXP) == DOUBLE_EXP) &&
		((x.i & DOUBLE_FRAC) != DOUBLE_ZERO) &&
		((x.i & 0x0008000000000000ULL) == DOUBLE_ZERO);
}

int FPRdouble::Cmp(FPRdouble& f)
{
	if(d < f.d) return CR_LT;
	if(d > f.d) return CR_GT;
	if(d == f.d) return CR_EQ;
	return CR_SO;
}