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
	const u64 preadlen_addr = CPU.GPR[6];
	ConLog.Warning("lv2TtyRead: ch: %d, buf addr: %llx, len: %d", ch, buf_addr, len);
	Memory.Write32(preadlen_addr, len);

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
	const u64 pwritelen_addr = CPU.GPR[6];
	//for(uint i=0; i<32; ++i) ConLog.Write("r%d = 0x%llx", i, CPU.GPR[i]);
	//ConLog.Warning("lv2TtyWrite: ch: %d, buf addr: %llx, len: %d", ch, buf_addr, len);
	if(!Memory.IsGoodAddr(buf_addr)) return CELL_UNKNOWN_ERROR;
	Memory.Write32(pwritelen_addr, len);
	const wxString& text = len > 0 ? Memory.ReadString(buf_addr, len) : Memory.ReadString(buf_addr);
	Emu.GetDbgCon().Write(ch, text);
	return CELL_OK;
}