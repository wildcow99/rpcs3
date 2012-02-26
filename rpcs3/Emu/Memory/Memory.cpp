#include "stdafx.h"
#include "Memory.h"
#include "MemoryBlock.h"

MemoryBase Memory;

//MemoryBlock
MemoryBlock::MemoryBlock()
{
	Init();
}

MemoryBlock::~MemoryBlock()
{
	Delete();
}

void MemoryBlock::Init()
{
	range_start = 0;
	range_end = 0;
	range_size = 0;

	mem = NULL;
}

void MemoryBlock::InitMemory()
{
	safe_delete(mem);
	mem = new u8[range_size + 1];
	memset(mem, 0, range_size + 1);
}

void MemoryBlock::Delete()
{
	safe_delete(mem);
	Init();
}

u32 MemoryBlock::FixAddr(const u32 addr) const
{
	return addr - GetStartAddr();
}

bool MemoryBlock::GetMemFromAddr(void* dst, const u32 addr, const u32 size)
{
	if(!IsMyAddress(addr)) return false;
	if(FixAddr(addr) + size > GetSize()) return false;
	memcpy(dst, &mem[FixAddr(addr)], size);
	return true;
}

bool MemoryBlock::SetMemFromAddr(void* src, const u32 addr, const u32 size)
{
	if(!IsMyAddress(addr)) return false;
	if(FixAddr(addr) + size > GetSize()) return false;
	memcpy(&mem[FixAddr(addr)], src, size);
	return true;
}

bool MemoryBlock::GetMemFFromAddr(void* dst, const u32 addr)
{
	if(!IsMyAddress(addr)) return false;
	dst = &mem[FixAddr(addr)];
	return true;
}

u8* MemoryBlock::GetMemFromAddr(const u32 addr)
{
	if(!IsMyAddress(addr)) return NULL;

	return &mem[FixAddr(addr)];
}

void MemoryBlock::SetRange(const u32 start, const u32 size)
{
	range_start = start;
	range_size = size;
	range_end = start + size;

	InitMemory();
}

bool MemoryBlock::IsMyAddress(const u32 addr)
{
	return addr >= GetStartAddr() && addr < GetEndAddr();
}

__forceinline const u8 MemoryBlock::FastRead8(const u32 addr) const
{
	return mem[addr];
}

__forceinline const u16 MemoryBlock::FastRead16(const u32 addr) const
{
	return ((u16)FastRead8(addr) << 8) | (u16)FastRead8(addr + 1);
}

__forceinline const u32 MemoryBlock::FastRead32(const u32 addr) const
{
	return ((u32)FastRead16(addr) << 16) | (u32)FastRead16(addr + 2);
}

__forceinline const u64 MemoryBlock::FastRead64(const u32 addr) const
{
	return ((u64)FastRead32(addr) << 32) | (u64)FastRead32(addr + 4);
}

__forceinline const u128 MemoryBlock::FastRead128(const u32 addr)
{
	u128 ret;
	ret.hi = FastRead64(addr);
	ret.lo = FastRead64(addr + 8);
	return ret;
}

bool MemoryBlock::Read8(const u32 addr, u8* value)
{
	if(!IsMyAddress(addr))
	{
		*value = 0;
		return false;
	}

	*value = FastRead8(FixAddr(addr));
	return true;
}

bool MemoryBlock::Read16(const u32 addr, u16* value)
{
	if(!IsMyAddress(addr))
	{
		*value = 0;
		return false;
	}

	*value = FastRead16(FixAddr(addr));
	return true;
}

bool MemoryBlock::Read32(const u32 addr, u32* value)
{
	if(!IsMyAddress(addr))
	{
		*value = 0;
		return false;
	}

	*value = FastRead32(FixAddr(addr));
	return true;
}

bool MemoryBlock::Read64(const u32 addr, u64* value)
{
	if(!IsMyAddress(addr))
	{
		*value = 0;
		return false;
	}

	*value = FastRead64(FixAddr(addr));
	return true;
}

bool MemoryBlock::Read128(const u32 addr, u128* value)
{
	if(!IsMyAddress(addr))
	{
		*value = u128::From32(0);
		return false;
	}

	*value = FastRead128(FixAddr(addr));
	return true;
}

__forceinline void MemoryBlock::FastWrite8(const u32 addr, const s8 value)
{
	mem[addr] = value;
}

__forceinline void MemoryBlock::FastWrite16(const u32 addr, const u16 value)
{
	FastWrite8(addr, (u8)(value >> 8));
	FastWrite8(addr+1, (u8)value);
}

__forceinline void MemoryBlock::FastWrite32(const u32 addr, const u32 value)
{
	FastWrite16(addr, (u16)(value >> 16));
	FastWrite16(addr+2, (u16)value);
}

__forceinline void MemoryBlock::FastWrite64(const u32 addr, const u64 value)
{
	FastWrite32(addr, (u32)(value >> 32));
	FastWrite32(addr+4, (u32)value);
}

__forceinline void MemoryBlock::FastWrite128(const u32 addr, const u128 value)
{
	FastWrite64(addr, value.hi);
	FastWrite64(addr+8, value.lo);
}

bool MemoryBlock::Write8(const u32 addr, const s8 value)
{
	if(!IsMyAddress(addr)) return false;

	FastWrite8(FixAddr(addr), value);
	return true;
}

bool MemoryBlock::Write16(const u32 addr, const s16 value)
{
	if(!IsMyAddress(addr)) return false;

	FastWrite16(FixAddr(addr), value);
	return true;
}

bool MemoryBlock::Write32(const u32 addr, const s32 value)
{
	if(!IsMyAddress(addr)) return false;

	FastWrite32(FixAddr(addr), value);
	return true;
}

bool MemoryBlock::Write64(const u32 addr, const s64 value)
{
	if(!IsMyAddress(addr)) return false;

	FastWrite64(FixAddr(addr), value);
	return true;
}

bool MemoryBlock::Write128(const u32 addr, const u128 value)
{
	if(!IsMyAddress(addr)) return false;

	FastWrite128(FixAddr(addr), value);
	return true;
}

//NullMemoryBlock
bool NullMemoryBlock::Read8(const u32 addr, u8* WXUNUSED(value))
{
	ConLog.Error("Read8 from null block: [%08x]", addr);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Read16(const u32 addr, u16* WXUNUSED(value))
{
	ConLog.Error("Read16 from null block: [%08x]", addr);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Read32(const u32 addr, u32* WXUNUSED(value))
{
	ConLog.Error("Read32 from null block: [%08x]", addr);
	//ConLog.Error("pc: 0x%x", (*(PPCThread*)Emu.GetCPU().GetIDs().GetIDData(1).m_data).PC);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Read64(const u32 addr, u64* WXUNUSED(value))
{
	ConLog.Error("Read64 from null block: [%08x]", addr);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Read128(const u32 addr, u128* WXUNUSED(value))
{
	ConLog.Error("Read128 from null block: [%08x]", addr);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Write8(const u32 addr, const u8 value)
{
	ConLog.Error("Write8 to null block: [%08x]: %x", addr, value);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Write16(const u32 addr, const u16 value)
{
	ConLog.Error("Write16 to null block: [%08x]: %x", addr, value);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Write32(const u32 addr, const u32 value)
{
	ConLog.Error("Write32 to null block: [%08x]: %x", addr, value);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Write64(const u32 addr, const u64 value)
{
	ConLog.Error("Write64 to null block: [%08x]: %x", addr, value);
	Emu.Pause();
	return false;
}

bool NullMemoryBlock::Write128(const u32 addr, const u128 value)
{
	ConLog.Error("Write128 to null block: [%08x]: %x_%x", addr, value.hi, value.lo);
	Emu.Pause();
	return false;
}

//MemoryBase
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
	return mem.FastRead8(mem.FixAddr(addr));
}

u16 MemoryBase::Read16(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read16(addr, NULL);
		return 0;
	}
	return mem.FastRead16(mem.FixAddr(addr));
}

u32 MemoryBase::Read32(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read32(addr, NULL);
		return 0;
	}
	return mem.FastRead32(mem.FixAddr(addr));
}

u64 MemoryBase::Read64(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read64(addr, NULL);
		return 0;
	}
	return mem.FastRead64(mem.FixAddr(addr));
}

u128 MemoryBase::Read128(u32 addr)
{
	MemoryBlock& mem = GetMemByAddr(addr);
	if(mem.IsNULL())
	{
		mem.Read128(addr, NULL);
		return u128::From32(0);
	}
	return mem.FastRead128(mem.FixAddr(addr));
}