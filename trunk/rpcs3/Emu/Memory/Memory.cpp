#include "stdafx.h"
#include "Memory.h"

MemoryBase Memory;

//TODO: Detect mem

u8* MemoryBase::GetMem(uint& addr)
{
	u8* Mem = NULL;

	if(addr < 0x0FFFFFFF)
	{
		Mem = MainRam;
		//ConLog.Warning("MainRam! (%08X)", addr);
	}
	else if(addr >= 0x0FFFFFFF && addr < 0x0FFFFFFF + 0x0FFFFFFF)
	{
		addr -= 0x0FFFFFFF;
		Mem = VideoMem;

		ConLog.Warning("VideoMem! (%08X)", addr);
	}
	else
	{
		ConLog.Warning("Unknown mem! (%08X)", addr);
	}

	return Mem;
}

void MemoryBase::Write32(uint addr, const u32 data)
{
	// (0xAABBCCDD << 24) = (ret == AA,			BB == 0, CC == 0, DD == 0)
	// (0xAABBCCDD << 16) = (ret == AABB,		BB != 0, CC == 0, DD == 0)
	// (0xAABBCCDD <<  8) = (ret == AABBCC,		BB != 0, CC != 0, DD == 0)
	// (0xAABBCCDD <<  0) = (ret == AABBCCDD,	BB != 0, CC != 0, DD != 0)

	u8* Mem = GetMem(addr);

	if(Mem == NULL) return;

	Mem[addr+0] = (u8)((data >> 24) & 0xffffffff);
	Mem[addr+1] = (u8)((data >> 16) & 0x00ffffff);
	Mem[addr+2] = (u8)((data >>  8) & 0x0000ffff);
	Mem[addr+3] = (u8)((data >>  0) & 0x000000ff);
}

u8 MemoryBase::Read8(uint addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return Mem[addr];
}

u16 MemoryBase::Read16(uint addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return
		((((u32)Mem[addr+0]) & 0xffff) <<  8) |
		((((u32)Mem[addr+1]) & 0x00ff) <<  0);
}


u32 MemoryBase::Read32(uint addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return
		((((u32)Mem[addr+0]) & 0xffffffff) << 24) |
		((((u32)Mem[addr+1]) & 0x00ffffff) << 16) |
		((((u32)Mem[addr+2]) & 0x0000ffff) <<  8) |
		((((u32)Mem[addr+3]) & 0x000000ff) <<  0);
}

u64 MemoryBase::Read64(uint addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return
		((((u64)Mem[addr+0]) & 0xffffffffffffffff) << 56) |
		((((u64)Mem[addr+1]) & 0x00ffffffffffffff) << 48) |
		((((u64)Mem[addr+2]) & 0x0000ffffffffffff) << 40) |
		((((u64)Mem[addr+3]) & 0x000000ffffffffff) << 32) |
		((((u64)Mem[addr+4]) & 0x00000000ffffffff) << 24) |
		((((u64)Mem[addr+5]) & 0x0000000000ffffff) << 16) |
		((((u64)Mem[addr+6]) & 0x000000000000ffff) <<  8) |
		((((u64)Mem[addr+7]) & 0x00000000000000ff) <<  0);
}

u128 MemoryBase::Read128(uint addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return u128::From((u32)0);

	u128 ret;

	ret.lo = Read64(addr+0);
	ret.hi = Read64(addr+8);

	return ret;
}