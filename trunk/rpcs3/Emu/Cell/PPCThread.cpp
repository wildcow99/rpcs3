#include "stdafx.h"
#include "PPCThread.h"
#include "Utilites/IdManager.h"
#include "Gui/InterpreterDisAsm.h"
#include "PPUThread.h"
#include "SPUThread.h"

IdManager CPU_IDs;

PPCThread::PPCThread(bool _isSPU)
	: isSPU(_isSPU)
	, DisAsmFrame(NULL)
	, m_arg(0)
	, m_dec(NULL)
	, stack_num(0)
	, stack_size(0)
	, stack_addr(0)
{
}

PPCThread::~PPCThread()
{
	Close();
}

void PPCThread::Close()
{
	Stop();
	if(DisAsmFrame)
	{
		(*(InterpreterDisAsmFrame*)DisAsmFrame).Close();
		DisAsmFrame = NULL;
	}
}

void PPCThread::Reset()
{
	CloseStack();

	SetPc(0);
	cycle = 0;

	isBranch = false;

	m_status = Stoped;
	m_error = 0;
	
	DoReset();
}

void PPCThread::InitStack()
{
	if(stack_num != 0) return;

	stack_num = Memory.MemoryBlocks.GetCount();
	stack_addr = Memory.MemoryBlocks.Get(stack_num - 1).GetEndAddr();
	if(stack_size == 0) stack_size = 0x10000;
	Stack.SetRange(stack_addr, stack_size);
	Memory.MemoryBlocks.Add(Stack);

	if(wxFileExists("stack.dat"))
	{
		ConLog.Warning("loading stack.dat...");
		wxFile stack("stack.dat");		
		stack.Read(Stack.GetMem(), 0x10000);
		stack.Close();
	}

	_InitStack();
}

void PPCThread::CloseStack()
{
	if(stack_num == 0) return;

	Stack.Delete();
	Memory.MemoryBlocks.RemoveFAt(stack_num);

	IdManager& cpus = Emu.GetCPU().GetIDs();
	ID thread;
	u32 pos = 0;
	while(cpus.GetNext(pos, thread))
	{
		u32& num = (*(PPCThread*)thread.m_data).stack_num;
		if(num > stack_num) num--;
	}

	stack_addr = 0;
	stack_size = 0;
	stack_num = 0;
}

void PPCThread::SetId(const u32 id)
{
	m_id = id;
	ID& thread = Emu.GetCPU().GetIDs().GetIDData(m_id);
	thread.m_name = wxString::Format("%s[%d] Thread", (isSPU ? "SPU" : "PPU"), m_id);
	thread.m_attr = isSPU ? 2 : 1;

	if(Ini.m_DecoderMode.GetValue() != 1) return;
	DisAsmFrame = new InterpreterDisAsmFrame(thread.m_name, this);
	(*(InterpreterDisAsmFrame*)DisAsmFrame).Show();
}

void PPCThread::NextBranchPc()
{
	SetPc(nPC);
}

void PPCThread::NextPc()
{
	if(isBranch)
	{
		NextBranchPc();
		isBranch = false;
		return;
	}

	SetPc(PC + 4);
}

void PPCThread::PrevPc()
{
	SetPc(PC - 4);
}

void PPCThread::SetPc(const u32 pc)
{
	PC = pc;
	nPC = PC + 4;
}

void PPCThread::SetBranch(const u32 pc)
{
	nPC = pc;
	isBranch = true;
}

void PPCThread::SetError(const u32 error)
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

wxArrayString PPCThread::ErrorToString(const u32 error)
{
	wxArrayString earr;
	earr.Clear();
	if(error == 0) return earr;

	earr.Add("Unknown error");

	return earr;
}

void PPCThread::Run()
{
	if(IsRunned()) Stop();
	if(IsPaused())
	{
		Resume();
		return;
	}
	
	m_status = Runned;

	InitStack();
	DoRun();
	Emu.CheckStatus();
	if(DisAsmFrame) (*(InterpreterDisAsmFrame*)DisAsmFrame).DoUpdate();
}

void PPCThread::Resume()
{
	if(!IsPaused()) return;
	m_status = Runned;
	DoResume();
	Emu.CheckStatus();
}

void PPCThread::Pause()
{
	if(!IsRunned()) return;
	m_status = Paused;
	DoPause();
	Emu.CheckStatus();
}

void PPCThread::Stop()
{
	if(IsStoped()) return;
	m_status = Stoped;
	Reset();
	DoStop();
	Emu.CheckStatus();
}

void PPCThread::Exec()
{
	if(!IsRunned()) return;
	DoCode(Memory.Read32(PC));
	NextPc();
}

/*
void PPCThread::Step()
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
			ConLog.Error("PPCThread[%d] stoped with error! Error code: %x", m_id, GetError());
			SetError(0);
		}
	}
}
*/