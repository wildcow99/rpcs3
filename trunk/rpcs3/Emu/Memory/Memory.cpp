#include "stdafx.h"
#include "Memory.h"

MemoryBase Memory;

u8* MemoryBase::GetMem(u32& addr)
{
	//addr -= 0x08000000;
	if(addr < 0x0FFFFFFF)
	{
		//ConLog.Warning("MainRam! (%08X)", addr);
		return MainRam;
	}

	if(addr < 0x0FFFFFFF + 0x0FBFFFFF)
	{
		ConLog.Warning("Video: FrameBuffer! (%08X)", addr);
		addr -= 0x0FFFFFFF;
		return Video_FrameBuffer;
	}

	if(addr < 0x0FFFFFFF + 0x0FBFFFFF + 0x003FFFFF)
	{
		ConLog.Warning("Video: GPUdata! (%08X)", addr);
		addr -= 0x0FBFFFFF + 0x0FFFFFFF;
		return Video_GPUdata;
	}

	ConLog.Warning("Unknown mem! (%08X)", addr);
	return NULL;
}

void MemoryBase::Write8(u32 addr, const u8 data)
{
	u8* Mem = GetMem(addr);

	if(Mem == NULL) return;

	Mem[addr] = data;
}

void MemoryBase::Write16(u32 addr, const u16 data)
{
	u8* Mem = GetMem(addr);

	if(Mem == NULL) return;

	Mem[addr+0] = (u8)((data >> 8) & 0xffff);
	Mem[addr+1] = (u8)((data >> 0) & 0x00ff);
}

void MemoryBase::Write32(u32 addr, const u32 data)
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

void MemoryBase::Write64(u32 addr, const u64 data)
{
	u8* Mem = GetMem(addr);

	if(Mem == NULL) return;

	Mem[addr+0] = (u8)((data >> 56) & 0xffffffffffffffff);
	Mem[addr+1] = (u8)((data >> 48) & 0x00ffffffffffffff);
	Mem[addr+2] = (u8)((data >> 40) & 0x0000ffffffffffff);
	Mem[addr+3] = (u8)((data >> 32) & 0x000000ffffffffff);

	Mem[addr+0] = (u8)((data >> 24) & 0x000000ffffffffff);
	Mem[addr+1] = (u8)((data >> 16) & 0x0000000000ffffff);
	Mem[addr+2] = (u8)((data >>  8) & 0x000000000000ffff);
	Mem[addr+3] = (u8)((data >>  0) & 0x00000000000000ff);
}

void MemoryBase::Write128(u32 addr, const u128 data)
{
	Write64(addr + 0, data.lo);
	Write64(addr + 8, data.hi);
}

u8 MemoryBase::Read8(u32 addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return Mem[addr];
}

u16 MemoryBase::Read16(u32 addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return
		((((u16)Mem[addr+0]) & 0xffff) << 8) |
		((((u16)Mem[addr+1]) & 0x00ff) << 0);
}

u32 MemoryBase::Read32(u32 addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return NULL;

	return
		((((u32)Mem[addr+0]) & 0xffffffff) << 24) |
		((((u32)Mem[addr+1]) & 0x00ffffff) << 16) |
		((((u32)Mem[addr+2]) & 0x0000ffff) <<  8) |
		((((u32)Mem[addr+3]) & 0x000000ff) <<  0);
}

u64 MemoryBase::Read64(u32 addr)
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

u128 MemoryBase::Read128(u32 addr)
{
	u8* Mem = GetMem(addr);
	if(Mem == NULL) return u128::From((u32)0);

	u128 ret;

	ret.lo = Read64(addr+0);
	ret.hi = Read64(addr+8);

	return ret;
}