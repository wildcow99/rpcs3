#include "stdafx.h"
#include "Memory.h"

MemoryBase Memory;

void MemoryBase::Write8(u32 addr, const u8 data)
{
	GetMemByAddr(addr).Write8(addr, data);
}

void MemoryBase::Write16(u32 addr, const u16 data)
{
	GetMemByAddr(addr).Write16(addr, data);
}

void MemoryBase::Write32(u32 addr, const u32 data)
{
	GetMemByAddr(addr).Write32(addr, data);
}

void MemoryBase::Write64(u32 addr, const u64 data)
{
	GetMemByAddr(addr).Write64(addr, data);
}

void MemoryBase::Write128(u32 addr, const u128 data)
{
	GetMemByAddr(addr).Write128(addr, data);
}

u8 MemoryBase::Read8(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read8(addr, NULL);
		return 0;
	}
	return mem.FastRead8(addr);
}

u16 MemoryBase::Read16(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read16(addr, NULL);
		return 0;
	}
	return mem.FastRead16(addr);
}

u32 MemoryBase::Read32(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read32(addr, NULL);
		return 0;
	}
	return mem.FastRead32(addr);
}

u64 MemoryBase::Read64(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read64(addr, NULL);
		return 0;
	}
	return mem.FastRead64(addr);
}

u128 MemoryBase::Read128(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read128(addr, NULL);
		return u128::From((u64)0);
	}
	return mem.FastRead128(addr);
}