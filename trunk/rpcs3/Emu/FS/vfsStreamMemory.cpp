#include "stdafx.h"
#include "vfsStreamMemory.h"

vfsStreamMemory::vfsStreamMemory() : vfsStream()
{
}

vfsStreamMemory::vfsStreamMemory(u64 addr)
{
	Open(addr);
}

void vfsStreamMemory::Open(u64 addr)
{
	Seek(addr);
}

u32 vfsStreamMemory::Write(const void* src, u32 size)
{
	if(!Memory.IsGoodAddr(Tell(), size)) return 0;

	memcpy(&Memory[Tell()], src, size);

	return vfsStream::Write(src, size);
}

u32 vfsStreamMemory::Read(void* dst, u32 size)
{
	if(!Memory.IsGoodAddr(Tell(), size)) return 0;

	memcpy(dst, &Memory[Tell()], size);

	return vfsStream::Read(dst, size);
}
