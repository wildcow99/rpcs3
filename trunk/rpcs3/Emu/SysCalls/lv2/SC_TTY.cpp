#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

int SysCalls::lv2TtyRead(PPUThread& CPU)
{
	//u32 ch;
	//void* buf;
	//u32 len;
	//u32* preadlen;
	const u32 ch = CPU.GPR[3];
	const u64 buf_addr = CPU.GPR[4];
	const u32 len = CPU.GPR[5];
	u64& preadlen = CPU.GPR[6];
	ConLog.Warning("lv2TtyRead: ch: %d, buf addr: %llx, len: %d", ch, buf_addr, len);
	preadlen = len;

	return CELL_OK;
}

int SysCalls::lv2TtyWrite(PPUThread& CPU)
{
	//u32 ch;
	//const void* buf;
	//u32 len;
	//u32* pwritelen;
	const u32 ch = CPU.GPR[3];
	const u64 buf_addr = CPU.GPR[4];
	const u32 len = CPU.GPR[5];
	u64& pwritelen = CPU.GPR[6];
	ConLog.Warning("lv2TtyWrite: ch: %d, buf addr: %llx, len: %d", ch, buf_addr, len);
	pwritelen = len;
	if(!Memory.IsGoodAddr(buf_addr)) return CELL_UNKNOWN_ERROR;
	if(len > 0) ConLog.Warning("lv2TtyWrite: %s", Memory.ReadString(buf_addr, len));
	else ConLog.Warning("lv2TtyWrite: %s", Memory.ReadString(buf_addr));

	return CELL_OK;
}