#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

enum PPU_THREAD_STATUS
{
	PPU_THREAD_STATUS_IDLE,
	PPU_THREAD_STATUS_RUNNABLE,
	PPU_THREAD_STATUS_ONPROC,
	PPU_THREAD_STATUS_SLEEP,
	PPU_THREAD_STATUS_STOP,
	PPU_THREAD_STATUS_ZOMBIE,
	PPU_THREAD_STATUS_DELETED,
	PPU_THREAD_STATUS_UNKNOWN,
};

PPU_THREAD_STATUS GetPPUThreadStatus(PPUThread& CPU)
{
	if(CPU.IsRunned()) return PPU_THREAD_STATUS_RUNNABLE;
	if(CPU.IsPaused()) return PPU_THREAD_STATUS_SLEEP;
	if(CPU.IsStoped()) return PPU_THREAD_STATUS_STOP;
	return PPU_THREAD_STATUS_UNKNOWN;
}

int SysCalls::lv2ProcessGetPid(PPUThread& CPU)
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessWaitForChild(PPUThread& CPU)
{
	return 0;
}
int SysCalls::lv2ProcessGetStatus(PPUThread& CPU)
{
	if(!CPU.IsSPU()) CPU.GPR[4] = GetPPUThreadStatus(CPU);

	return 0;
}
int SysCalls::lv2ProcessDetachChild(PPUThread& CPU)
{
	return 0;
}
int SysCalls::lv2ProcessGetNumberOfObject(PPUThread& CPU)
{
	CPU.GPR[4] = 1;//???
	return 0;
}
int SysCalls::lv2ProcessGetId(PPUThread& CPU)
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessGetPpid(PPUThread& CPU)
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessKill(PPUThread& CPU)
{
	CPU.Close();
	return 0;
}
int SysCalls::lv2ProcessExit(PPUThread& CPU)
{
	/*
	switch(CPU.GPR[3])
	{
	case PPU_THREAD_STATUS_IDLE:
	case PPU_THREAD_STATUS_DELETED:
	case PPU_THREAD_STATUS_UNKNOWN:
		CPU.Stop();
	break;

	case PPU_THREAD_STATUS_SLEEP:
	case PPU_THREAD_STATUS_ZOMBIE:
		CPU.Pause();
	break;
	}
	*/
	CPU.Close();
	return 0;
}
int SysCalls::lv2ProcessWaitForChild2(PPUThread& CPU)
{
	return 0;
}
int SysCalls::lv2ProcessGetSdkVersion(PPUThread& CPU)
{
	CPU.GPR[4] = 0x360001; //TODO
	return 0;
}