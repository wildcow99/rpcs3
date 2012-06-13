#include "stdafx.h"
#include "Emu/Cell/PPUThread.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUInterpreter.h"
#include "Emu/Cell/PPUDisAsm.h"

#include "Emu/SysCalls/SysCalls.h"

extern gcmInfo gcm_info;

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

void PPUThread::SetBranch(const u64 pc)
{
	u64 fid, waddr;
	if(Memory.MemFlags.IsFlag(pc, waddr, fid))
	{
		GPR[3] = SysCallsManager.DoFunc(fid, *this);

		if((s32)GPR[3] < 0 && fid != 0x72a577ce) ConLog.Write("Func[0x%llx] done with code [0x%x]! #pc: 0x%llx", fid, (u32)GPR[3], PC);
#ifdef HLE_CALL_LOG
		else ConLog.Warning("Func[0xll%x] done with code [0x%llx]! #pc: 0x%llx", fid, GPR[3], PC);
#endif
		//ConLog.Warning("Func waddr: 0x%llx", waddr);
		const u64 addr = Emu.GetTLSAddr();
		Memory.Write32(waddr, addr);
		Memory.Write32(addr, PC + 4);
		if(fid == 0x744680a2) Memory.Write32(addr+4, GPR[3]);
		//Memory.Write32(addr+4, Emu.GetTLSMemsz());
	}
	else if(pc == Memory.VideoMem.GetStartAddr())
	{
		ConLog.Warning("gcm: callback(context=0x%llx, count=0x%llx) #pc: 0x%llx", GPR[3], GPR[4], PC);

		CellGcmContextData& ctx = *(CellGcmContextData*)Memory.GetMemFromAddr(GPR[3]);
		CellGcmControl& ctrl = *(CellGcmControl*)Memory.GetMemFromAddr(gcm_info.control_addr);

		while(ctrl.put != ctrl.get) Sleep(0);

		const u32 reserve = GPR[4] * 4;
		ctx.current = ctx.begin;

		ctrl.put = 0/*re32(reserve)*/;
		ctrl.get = 0;

		GPR[3] = 0;

		PPCThread::SetBranch(PC + 4);
		return;
	}

	PPCThread::SetBranch(pc);
}

void PPUThread::AddArgv(const wxString& arg)
{
	stack_point -= arg.Len() + 1;
	argv_addr.AddCpy(stack_point + 1);
	Memory.WriteString(stack_point + 1, arg);
}

void PPUThread::InitRegs()
{
	const u32 entry = Memory.Read32(PC);
	const u32 rtoc = Memory.Read32(PC + 4);

	SetPc(entry);
	
	int argc = m_arg;
	int argv = 0;

	if(argv_addr.GetCount())
	{
		argc = argv_addr.GetCount();
		stack_point -= 0xc + 4 * argc;
		argv = stack_point;

		mem64_t argv_list(argv);
		for(int i=0; i<argc; ++i) argv_list += argv_addr[i];
	}

	GPR[1] = stack_point;
	GPR[2] = rtoc;
	if(argc)
	{
		GPR[3] = argc;
		GPR[4] = argv;
		GPR[5] = argv ? argv - 0xc - 4 * argc : 0; //unk
	}
	else
	{
		GPR[3] = m_arg;
	}
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

	//Memory.Write32(GPR[4], Emu.GetTLSFilesz());
	//Memory.Write32(GPR[4]+4, Emu.GetTLSMemsz());

	//Memory.Write32(Memory.Read32(0x30e44), 0x12e464);
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

	TB++;
	/*
	if(++cycle > 220)
	{
		cycle = 0;
		TB++;
	}
	*/

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
	return
		((*(u64*)&d & DOUBLE_EXP) == DOUBLE_EXP) &&
		((*(u64*)&d & 0x0007fffffffffffULL) == DOUBLE_ZERO) &&
		((*(u64*)&d & 0x000800000000000ULL) == 0x000800000000000ULL);
}

bool FPRdouble::IsSNaN(double d)
{
	return
		((*(u64*)&d & DOUBLE_EXP) == DOUBLE_EXP) &&
		((*(u64*)&d & DOUBLE_FRAC) != DOUBLE_ZERO) &&
		((*(u64*)&d & 0x0008000000000000ULL) == DOUBLE_ZERO);
}

int FPRdouble::Cmp(double a, double b)
{
	if(a < b) return CR_LT;
	if(a > b) return CR_GT;
	if(a == b) return CR_EQ;
	return CR_SO;
}