#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

SysCallBase sc_ppu_thread("sys_ppu_thread");

#define PPU_THREAD_ID_INVALID 0xFFFFFFFFU

int sys_ppu_thread_exit(int errorcode)
{
	sc_ppu_thread.Log("sys_ppu_thread_exit(errorcode=%d)", errorcode);
	Emu.GetCPU().RemoveThread(GetCurrentPPUThread().GetId());

	return CELL_OK;
}

int sys_ppu_thread_yield()
{
	sc_ppu_thread.Log("sys_ppu_thread_yield()");
	wxThread::Yield();

	return CELL_OK;
}

int sys_ppu_thread_join(u32 thread_id, u32 vptr_addr)
{
	sc_ppu_thread.Error("sys_ppu_thread_join(thread_id=%d, vptr_addr=0x%x)", thread_id, vptr_addr);

	return CELL_OK;
}

int sys_ppu_thread_detach(u32 thread_id)
{
	sc_ppu_thread.Error("sys_ppu_thread_detach(thread_id=%d)", thread_id);

	return CELL_OK;
}

void sys_ppu_thread_get_join_state(u32 isjoinable_addr)
{
	sc_ppu_thread.Warning("sys_ppu_thread_get_join_state(isjoinable_addr=0x%x)", isjoinable_addr);
	Memory.Write32(isjoinable_addr, GetCurrentPPUThread().IsJoinable());
}

int sys_ppu_thread_set_priority(u32 thread_id, int prio)
{
	sc_ppu_thread.Warning("sys_ppu_thread_set_priority(thread_id=%d, prio=%d)", thread_id, prio);

	PPCThread* thr = Emu.GetCPU().GetThread(thread_id);
	if(!thr) return CELL_ESRCH;

	thr->SetPrio(prio);

	return CELL_OK;
}

int sys_ppu_thread_get_priority(u32 thread_id, u32 prio_addr)
{
	sc_ppu_thread.Log("sys_ppu_thread_get_priority(thread_id=%d, prio_addr=0x%x)", thread_id, prio_addr);

	PPCThread* thr = Emu.GetCPU().GetThread(thread_id);
	if(!thr) return CELL_ESRCH;
	if(!Memory.IsGoodAddr(prio_addr)) return CELL_EFAULT;

	Memory.Write32(prio_addr, thr->GetPrio());

	return CELL_OK;
}

int sys_ppu_thread_get_stack_information(u32 info_addr)
{
	sc_ppu_thread.Log("sys_ppu_thread_get_stack_information(info_addr=0x%x)", info_addr);

	if(!Memory.IsGoodAddr(info_addr)) return CELL_EFAULT;

	declCPU();

	Memory.Write32(info_addr,   CPU.GetStackAddr());
	Memory.Write32(info_addr+4, CPU.GetStackSize());

	return CELL_OK;
}

int sys_ppu_thread_stop(u32 thread_id)
{
	sc_ppu_thread.Warning("sys_ppu_thread_stop(thread_id=%d)", thread_id);

	PPCThread* thr = Emu.GetCPU().GetThread(thread_id);
	if(!thr) return CELL_ESRCH;

	thr->Stop();

	return CELL_OK;
}

int sys_ppu_thread_restart(u32 thread_id)
{
	sc_ppu_thread.Error("sys_ppu_thread_restart(thread_id=%d)", thread_id);

	PPCThread* thr = Emu.GetCPU().GetThread(thread_id);
	if(!thr) return CELL_ESRCH;

	//thr->Restart();

	return CELL_OK;
}

int sys_ppu_thread_create(u32 thread_id, u32 entry, u32 arg, int prio, u32 stacksize, u64 flags, u32 threadname_addr)
{
	sc_ppu_thread.Error("sys_ppu_thread_restart(thread_id=%d)", thread_id);

	PPCThread* thr = Emu.GetCPU().GetThread(thread_id);
	if(!thr) return CELL_ESRCH;

	//thr->Restart();

	return CELL_OK;
}

int SysCalls::lv2PPUThreadCreate(PPUThread& CPU)
{
	ConLog.Write("lv2PPUThreadCreate:");
	//ConLog.Write("**** id: %d", CPU.GPR[3]);
	ConLog.Write("**** entry: 0x%x", CPU.GPR[4]);
	ConLog.Write("**** arg: 0x%x", CPU.GPR[5]);
	ConLog.Write("**** prio: 0x%x", CPU.GPR[6]);
	ConLog.Write("**** stacksize: 0x%x", CPU.GPR[7]);
	ConLog.Write("**** flags: 0x%x", CPU.GPR[8]);
	ConLog.Write("**** threadname: \"%s\"[0x%x]", Memory.ReadString(CPU.GPR[9]), CPU.GPR[9]);

	if(!Memory.IsGoodAddr(CPU.GPR[4])) return CELL_EFAULT;

	PPCThread& new_thread = Emu.GetCPU().AddThread(true);

	Memory.Write32(CPU.GPR[3], new_thread.GetId());
	new_thread.SetPc(CPU.GPR[4]);
	new_thread.SetArg(CPU.GPR[5]);
	new_thread.SetPrio(CPU.GPR[6]);
	new_thread.stack_size = CPU.GPR[7];
	//new_thread.flags = CPU.GPR[8];
	new_thread.SetName(Memory.ReadString(CPU.GPR[9]));
	new_thread.Run();
	new_thread.Exec();
	return CELL_OK;
}


int SysCalls::lv2PPUThreadYield(PPUThread& CPU)
{
	//ConLog.Warning("TODO: PPU[%d] thread yield!", CPU.GetId());
	return CELL_OK;
}

int SysCalls::lv2PPUThreadJoin(PPUThread& CPU)
{
	ConLog.Warning("TODO: PPU[%d] thread join!", CPU.GPR[3]);
	return CELL_OK;
}

int SysCalls::lv2PPUThreadDetach(PPUThread& CPU)
{
	ConLog.Warning("PPU[%d] thread detach", CPU.GPR[3]);
	if(!Emu.GetIdManager().CheckID(CPU.GPR[3])) return CELL_ESRCH;
	const ID& id = Emu.GetIdManager().GetIDData(CPU.GPR[3]);
	if(!id.m_used) return CELL_ESRCH;
	PPCThread& thread = *(PPCThread*)id.m_data;
	if(thread.IsJoinable()) return CELL_EINVAL;
	if(thread.IsJoining()) return CELL_EBUSY;
	thread.SetJoinable(false);
	if(!thread.IsRunned()) Emu.GetCPU().RemoveThread(thread.GetId());
	return CELL_OK;
}

int SysCalls::lv2PPUThreadGetJoinState(PPUThread& CPU)
{
	ConLog.Warning("PPU[%d] get join state", CPU.GetId());
	Memory.Write32(CPU.GPR[3], CPU.IsJoinable() ? 1 : 0);
	return CELL_OK;
}

int SysCalls::lv2PPUThreadSetPriority(PPUThread& CPU)
{
	ConLog.Write("PPU[%d] thread set priority", CPU.GPR[3]);
	if(!Emu.GetIdManager().CheckID(CPU.GPR[3])) return CELL_ESRCH;
	const ID& id = Emu.GetIdManager().GetIDData(CPU.GPR[3]);
	CPU.SetPrio(CPU.GPR[4]);
	return CELL_OK;
}

int SysCalls::lv2PPUThreadGetPriority(PPUThread& CPU)
{
	ConLog.Write("PPU[%d] thread get priority", CPU.GPR[3]);
	if(!Emu.GetIdManager().CheckID(CPU.GPR[3])) return CELL_ESRCH;
	const ID& id = Emu.GetIdManager().GetIDData(CPU.GPR[3]);
	Memory.Write32(CPU.GPR[4], CPU.GetPrio());
	return CELL_OK;
}

int SysCalls::lv2PPUThreadGetStackInformation(PPUThread& CPU)
{
	ConLog.Write("PPU[%d] thread get stack information(0x%llx)", CPU.GetId(), CPU.GPR[3]);
	Memory.Write32(CPU.GPR[3],   CPU.GetStackAddr());
	Memory.Write32(CPU.GPR[3]+4, CPU.GetStackSize());
	return CELL_OK;
}

int SysCalls::lv2PPUThreadRename(PPUThread& CPU)
{
	ConLog.Write("PPU[%d] thread rename(%s)", CPU.GPR[3], Memory.ReadString(CPU.GPR[4]));
	return CELL_OK;
}

int SysCalls::lv2PPUThreadRecoverPageFault(PPUThread& CPU)
{
	ConLog.Warning("TODO: PPU[%d] thread recover page fault!", CPU.GPR[3]);
	return CELL_OK;
}

int SysCalls::lv2PPUThreadGetPageFaultContext(PPUThread& CPU)
{
	ConLog.Warning("TODO: PPU[%d] thread get page fault context!", CPU.GPR[3]);
	return CELL_OK;
}

int SysCalls::lv2PPUThreadGetId(PPUThread& CPU)
{
	//ConLog.Write("PPU[%d] thread get id(0x%llx)", CPU.GetId(), CPU.GPR[3]);
	Memory.Write64(CPU.GPR[3], CPU.GetId());
	return CELL_OK;
}