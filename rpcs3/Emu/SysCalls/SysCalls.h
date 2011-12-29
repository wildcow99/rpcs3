#pragma once
#include <Emu/Cell/PPUThread.h>
//#include <Emu/Cell/SPUThread.h>
#include "Utilites/IdManager.h"
#include "ErrorCodes.h"

class SysCallBase
{
private:
	wxString m_module_name;

public:
	SysCallBase(const wxString& name) : m_module_name(name)
	{
	}

	wxString GetName() { return m_module_name; }

	void Log(const u32 id, wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Warning(GetName() + wxString::Format("[%d]: ", id) + wxString::FormatV(fmt, list));
		va_end(list);
	}

	void Log(wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Warning(GetName() + ": " + wxString::FormatV(fmt, list));
		va_end(list);
	}

	void Warning(const u32 id, wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Warning(GetName() + wxString::Format("[%d] warning: ", id) + wxString::FormatV(fmt, list));
		va_end(list);
	}

	void Warning(wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Warning(GetName() + wxString::Format(" warning: ") + wxString::FormatV(fmt, list));
		va_end(list);
	}

	void Error(const u32 id, wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Error(GetName() + wxString::Format("[%d] error: ", id) + wxString::FormatV(fmt, list));
		va_end(list);
	}

	void Error(wxString fmt, ...)
	{
		va_list list;
		va_start(list, fmt);
		ConLog.Error(GetName() + wxString::Format(" error: ") + wxString::FormatV(fmt, list));
		va_end(list);
	}
};

static wxString FixPatch(const wxString& patch)
{
	if(!patch(0, 10).CmpNoCase("/dev_hdd0/")) return CurGameInfo.root + patch(10, patch.Length());

	ConLog.Warning("Unknown patch: %s", patch);
	return patch;
}

class SysCalls
{
	IdManager SysCalls_IDs;

public:
	//process
	int lv2ProcessGetPid(PPUThread& CPU);
	int lv2ProcessWaitForChild(PPUThread& CPU);
	int lv2ProcessGetStatus(PPUThread& CPU);
	int lv2ProcessDetachChild(PPUThread& CPU);
	int lv2ProcessGetNumberOfObject(PPUThread& CPU);
	int lv2ProcessGetId(PPUThread& CPU);
	int lv2ProcessGetPpid(PPUThread& CPU);
	int lv2ProcessKill(PPUThread& CPU);
	int lv2ProcessExit(PPUThread& CPU);
	int lv2ProcessWaitForChild2(PPUThread& CPU);
	int lv2ProcessGetSdkVersion(PPUThread& CPU);

	//ppu thread
	int lv2PPUThreadCreate(PPUThread& CPU);
	int lv2PPUThreadExit(PPUThread& CPU);
	int lv2PPUThreadYield(PPUThread& CPU);
	int lv2PPUThreadJoin(PPUThread& CPU);
	int lv2PPUThreadDetach(PPUThread& CPU);
	int lv2PPUThreadGetJoinState(PPUThread& CPU);
	int lv2PPUThreadSetPriority(PPUThread& CPU);
	int lv2PPUThreadGetPriority(PPUThread& CPU);
	int lv2PPUThreadGetStackInformation(PPUThread& CPU);
	int lv2PPUThreadRename(PPUThread& CPU);
	int lv2PPUThreadRecoverPageFault(PPUThread& CPU);
	int lv2PPUThreadGetPageFaultContext(PPUThread& CPU);

	//lwmutex
	int Lv2LwmutexCreate(PPUThread& CPU);
	int Lv2LwmutexDestroy(PPUThread& CPU);
	int Lv2LwmutexLock(PPUThread& CPU);
	int Lv2LwmutexTrylock(PPUThread& CPU);
	int Lv2LwmutexUnlock(PPUThread& CPU);

	//memory
	int lv2MemContinerCreate(PPUThread& CPU);
	int lv2MemContinerDestroy(PPUThread& CPU);
	int lv2MemAllocate(PPUThread& CPU);
	int lv2MemGetUserMemorySize(PPUThread& CPU);

	//tty
	int lv2TtyRead(PPUThread& CPU);
	int lv2TtyWrite(PPUThread& CPU);

	//filesystem
	int lv2FsOpen(PPUThread& CPU);
	int lv2FsRead(PPUThread& CPU);
	int lv2FsWrite(PPUThread& CPU);
	int lv2FsClose(PPUThread& CPU);
	int lv2FsOpenDir(PPUThread& CPU);
	int lv2FsReadDir(PPUThread& CPU);
	int lv2FsCloseDir(PPUThread& CPU);
	int lv2FsMkdir(PPUThread& CPU);
	int lv2FsRename(PPUThread& CPU);
	int lv2FsLSeek64(PPUThread& CPU);
	int lv2FsRmdir(PPUThread& CPU);
	int lv2FsUtime(PPUThread& CPU);

public:
	SysCalls()// : CPU(cpu)
	{
	}

	~SysCalls()
	{
		Close();
	}

	void Close()
	{
		SysCalls_IDs.Clear();
	}

	u64 DoSyscall(int code, PPUThread& CPU)
	{
		switch(code)
		{
			//=== lv2 ===
			//process
			case 1: return lv2ProcessGetPid(CPU);
			case 2: return lv2ProcessWaitForChild(CPU);
			case 4: return lv2ProcessGetStatus(CPU);
			case 5: return lv2ProcessDetachChild(CPU);
			case 12: return lv2ProcessGetNumberOfObject(CPU);
			case 13: return lv2ProcessGetId(CPU);
			case 18: return lv2ProcessGetPpid(CPU);
			case 19: return lv2ProcessKill(CPU);
			case 22: return lv2ProcessExit(CPU);
			case 23: return lv2ProcessWaitForChild2(CPU);
			case 25: return lv2ProcessGetSdkVersion(CPU);
			//ppu thread
			//case ?: return lv2PPUThreadCreate(CPU);
			//case ?: return lv2PPUThreadExit(CPU);
			case 43: return lv2PPUThreadYield(CPU);
			case 44: return lv2PPUThreadJoin(CPU);
			case 45: return lv2PPUThreadDetach(CPU);
			case 46: return lv2PPUThreadGetJoinState(CPU);
			case 47: return lv2PPUThreadSetPriority(CPU);
			case 48: return lv2PPUThreadGetPriority(CPU);
			case 49: return lv2PPUThreadGetStackInformation(CPU);
			case 56: return lv2PPUThreadRename(CPU);
			case 57: return lv2PPUThreadRecoverPageFault(CPU);
			case 58: return lv2PPUThreadGetPageFaultContext(CPU);
			//lwmutex
			case 95: return Lv2LwmutexCreate(CPU);
			case 96: return Lv2LwmutexDestroy(CPU);
			case 97: return Lv2LwmutexLock(CPU);
			case 98: return Lv2LwmutexTrylock(CPU);
			case 99: return Lv2LwmutexUnlock(CPU);
			//time
			case 146: return wxDateTime::GetTimeNow(); 	//sys_time_get_system_time
			case 147: return 79800000; //get timebase frequency
			//memory
			case 324: return lv2MemContinerCreate(CPU);
			case 325: return lv2MemContinerDestroy(CPU);
			case 348: return lv2MemAllocate(CPU);
			case 352: return lv2MemGetUserMemorySize(CPU);
			//tty
			case 402: return lv2TtyRead(CPU);
			case 403: return lv2TtyWrite(CPU);
			//file system
			case 801: return lv2FsOpen(CPU);
			case 802: return lv2FsRead(CPU);
			case 803: return lv2FsWrite(CPU);
			case 804: return lv2FsClose(CPU);
			case 805: return lv2FsOpenDir(CPU);
			case 806: return lv2FsReadDir(CPU);
			case 807: return lv2FsCloseDir(CPU);
			case 811: return lv2FsMkdir(CPU);
			case 812: return lv2FsRename(CPU);
			case 813: return lv2FsRmdir(CPU);
			case 818: return lv2FsLSeek64(CPU);
			case 815: return lv2FsUtime(CPU);
			case 988:
				ConLog.Warning("SysCall 988! r3: 0x%llx, r4: 0x%llx, r5: 0x%llx, pc: 0x%llx",
					CPU.GPR[3], CPU.GPR[4], CPU.GPR[5], CPU.PC);
				return 0;
		}

		ConLog.Error("Unknown syscall: %d - %08x", code, code);
		return 0;
	}
	
	u64 DoFunc(const u32 id, PPUThread& CPU);
};

extern SysCalls SysCallsManager;