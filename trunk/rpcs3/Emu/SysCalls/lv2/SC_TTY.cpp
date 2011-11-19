#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

int SysCalls::lv2TtyRead()
{
	//u32 ch;
	//void* buf;
	//u32 len;
	//u32* preadlen;
	const u32 ch = CPU.GPR[3];
	const u64 buf_addr = CPU.GPR[4];
	const u32 len = CPU.GPR[5];
	u64& preadlen = CPU.GPR[6];
	ConLog.Warning("lv2TtyRead: ch: %d, buf addr: %x, len: %d", ch, (u32)buf_addr, len);
	preadlen = len;

	return 0;
}

int SysCalls::lv2TtyWrite()
{
	//u32 ch;
	//const void* buf;
	//u32 len;
	//u32* pwritelen;
	const u32 ch = CPU.GPR[3];
	const u64 buf_addr = CPU.GPR[4];
	const u32 len = CPU.GPR[5];
	u64& pwritelen = CPU.GPR[6];
	ConLog.Warning("lv2TtyWrite: ch: %d, buf addr: %x, len: %d", ch, (u32)buf_addr, len);
	pwritelen = len;
	if(len != 0) ConLog.Warning("lv2TtyWrite: %s", Memory.ReadString(buf_addr, len));

	return 0;
}