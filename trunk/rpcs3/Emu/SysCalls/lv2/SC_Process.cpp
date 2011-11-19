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

int SysCalls::lv2ProcessGetPid()
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessWaitForChild()
{
	return 0;
}
int SysCalls::lv2ProcessGetStatus()
{
	if(!CPU.IsSPU()) CPU.GPR[4] = GetPPUThreadStatus(CPU);

	return 0;
}
int SysCalls::lv2ProcessDetachChild()
{
	return 0;
}
int SysCalls::lv2ProcessGetNumberOfObject()
{
	CPU.GPR[4] = 1;//???
	return 0;
}
int SysCalls::lv2ProcessGetId()
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessGetPpid()
{
	CPU.GPR[4] = CPU.GetId();
	return 0;
}
int SysCalls::lv2ProcessKill()
{
	CPU.Close();
	return 0;
}
int SysCalls::lv2ProcessExit()
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
int SysCalls::lv2ProcessWaitForChild2()
{
	return 0;
}
int SysCalls::lv2ProcessGetSdkVersion()
{
	CPU.GPR[4] = 0x360001; //TODO
	return 0;
}