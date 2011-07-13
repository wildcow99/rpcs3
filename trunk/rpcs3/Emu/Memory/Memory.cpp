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
	u8 ret;
	GetMemByAddr(addr).Read8(addr, &ret);
	return ret;
}

u16 MemoryBase::Read16(u32 addr)
{
	u16 ret;
	GetMemByAddr(addr).Read16(addr, &ret);
	return ret;
}

u32 MemoryBase::Read32(u32 addr)
{
	u32 ret;
	GetMemByAddr(addr).Read32(addr, &ret);
	return ret;
}

u64 MemoryBase::Read64(u32 addr)
{
	u64 ret;
	GetMemByAddr(addr).Read64(addr, &ret);
	return ret;
}

u128 MemoryBase::Read128(u32 addr)
{
	u128 ret;
	GetMemByAddr(addr).Read128(addr, &ret);
	return ret;
}