#include "stdafx.h"
#include "CPU.h"
#include "Utilites/IdManager.h"
#include "Gui/InterpreterDisAsm.h"

IdManager CPU_IDs;

CPUThread::CPUThread(bool _isSPU, const u8 _core)
	: StepThread()
	, isSPU(_isSPU)
	, m_id(CPU_IDs.GetNewID())
	, core(_core)
	, DisAsmFrame(NULL)
{
	ID& id = CPU_IDs.GetIDData(m_id);
	id.name = wxString::Format("%s[%d] Thread", (isSPU ? "SPU" : "PPU"), m_id);
	id.attr = isSPU ? 2 : 1;
	StepThread::SetName(id.name);
	m_dec = NULL;
	StepThread::Start();

	if(Ini.m_DecoderMode.GetValue() == 1)
	{
		DisAsmFrame = new InterpreterDisAsmFrame(StepThread::GetName(), this);
		(*(InterpreterDisAsmFrame*)DisAsmFrame).Show();
	}

	stack_num = 0;
	stack_size = 0;
	stack_addr = 0;
}

CPUThread::~CPUThread()
{
}

void CPUThread::Close()
{
	CPU_IDs.RemoveID(GetId());
	Emu.RemoveThread(core);
	if(DisAsmFrame) (*(InterpreterDisAsmFrame*)DisAsmFrame).Close();
	Stop();
	
	if(wxIsMainThread())
	{
		WaitForStop();
		StepThread::Exit();
		this->~CPUThread();
	}
	else
	{
		ThreadAdv::Exit();
	}
}

void CPUThread::Reset()
{
	CloseStack();

	SetPc(0);
	cycle = 0;

	isBranch = false;

	m_status = Stoped;
	m_error = 0;
	
	DoReset();
}

void CPUThread::InitStack()
{
	if(stack_num != 0) return;

	stack_num = Memory.MemoryBlocks.GetCount();
	stack_addr = Memory.MemoryBlocks.Get(stack_num - 1).GetEndAddr();
	if(stack_size == 0) stack_size = 0x100;
	Stack.SetRange(stack_addr, stack_size);
	Memory.MemoryBlocks.Add(Stack);

	_InitStack();
}

void CPUThread::CloseStack()
{
	if(stack_num == 0) return;

	Stack.Delete();
	Memory.MemoryBlocks.RemoveFAt(stack_num);

	for(uint i=0; i<Emu.GetCPU().GetCount(); ++i)
	{
		u32& num = Emu.GetCPU().Get(i).stack_num;
		if(num > stack_num) num--;
	}

	stack_addr = 0;
	stack_size = 0;
	stack_num = 0;
}

void CPUThread::NextBranchPc()
{
	SetPc(nPC);
}

void CPUThread::NextPc()
{
	if(isBranch)
	{
		NextBranchPc();
		isBranch = false;
		return;
	}

	SetPc(PC + 4);
}

void CPUThread::PrevPc()
{
	SetPc(PC - 4);
}

void CPUThread::SetPc(const u32 pc)
{
	PC = pc;
	nPC = PC + 4;
}

void CPUThread::SetBranch(const u32 pc)
{
	nPC = pc;
	isBranch = true;
}

void CPUThread::SetError(const u32 error)
{
	if(error == 0)
	{
		m_error = 0;
	}
	else
	{
		m_error |= error;
	}
}

void CPUThread::Run()
{
	if(IsRunned()) Stop();
	if(IsPaused())
	{
		Resume();
		return;
	}
	
	InitStack();
	DoRun();

	if(Ini.m_DecoderMode.GetValue() != 1) StepThread::DoStep();
	else
	{
		m_status = Runned;
		Pause();
		if(DisAsmFrame) (*(InterpreterDisAsmFrame*)DisAsmFrame).DoUpdate();
	}
}

void CPUThread::Resume()
{
	if(!IsPaused()) return;
	DoResume();
	StepThread::DoStep();
}

void CPUThread::Pause()
{
	if(!IsRunned()) return;
	m_status = Paused;
	DoPause();
}

void CPUThread::Stop()
{
	if(IsStoped()) return;
	m_status = Stoped;
	Reset();
	DoStop();
}

void CPUThread::SysResume()
{
	if(!IsRunned()) return;
	DoSysResume();
	StepThread::DoStep();
}

void CPUThread::WaitForStop()
{
	while(!StepThread::IsWait() && StepThread::IsStarted()) Sleep(1);
}

void CPUThread::Step()
{
	if(Ini.m_DecoderMode.GetValue() == 1)
	{
		DoCode(Memory.Read32(PC));
		NextPc();
		if(DisAsmFrame) (*(InterpreterDisAsmFrame*)DisAsmFrame).DoUpdate();
	}
	else
	{
		m_status = Runned;

		while(IsRunned() && Emu.IsRunned())
		{
			DoCode(Memory.Read32(PC));
			NextPc();
		}

		if(!IsOk())
		{
			ConLog.Error("CpuThread[%d] stoped with error! Error code: %x", m_id, GetError());
			SetError(0);
		}
	}
}