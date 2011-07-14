#pragma once

class MemoryBlock
{
private:
	void* mem;
	u32 range_start;
	u32 range_end;
	u32 range_size;
	s8 number;

public:
	MemoryBlock()
	{
		Init();
	}

	~MemoryBlock()
	{
		Delete();
	}

private:
	void Init()
	{
		number = -1;

		range_start = 0;
		range_end = 0;
		range_size = 0;

		mem = NULL;
	}

	void InitMemory()
	{
		safe_delete(mem);
		mem = new u8[range_size];
		memset(mem, 0, sizeof(mem));
	}

public:
	void Delete()
	{
		safe_delete(mem);
		Init();
	}

	bool GetMemFromAddr(const u32 addr, void* dst)
	{
		if(!IsMyAddress(addr)) return false;
		dst = &((u8*)mem)[addr - GetStartAddr()];
		return true;
	}

	u8* GetMemFromAddr(const u32 addr)
	{
		if(!IsMyAddress(addr)) return NULL;

		return &((u8*)mem)[addr];
	}

	void SetNumber(const u8 num)
	{
		number = num;
	}

	void SetRange(const u32 start, const u32 size)
	{
		range_start = start;
		range_end = start + size + 1;
		range_size = size + 1;

		InitMemory();
	}

	virtual bool IsMyAddress(const u32 addr)
	{
		if(addr >= GetStartAddr() && addr < GetEndAddr())
		{
			return true;
		}

		return false;
	}

	__forceinline const u8 FastRead8(const u32 addr)
	{
		return ((u8*)mem)[addr - GetStartAddr()];
	}

	__forceinline const u16 FastRead16(const u32 addr)
	{
		return ((u16)FastRead8(addr) << 8) | (u16)FastRead8(addr + 1);
	}

	__forceinline const u32 FastRead32(const u32 addr)
	{
		return ((u32)FastRead16(addr) << 16) | (u32)FastRead16(addr + 2);
	}

	__forceinline const u32 FastRead64(const u32 addr)
	{
		return ((u64)FastRead32(addr) << 32) | (u64)FastRead32(addr + 4);
	}

	virtual bool Read8(const u32 addr, u8* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = 0;
			return false;
		}

		*value = FastRead8(addr);
		return true;
	}

	virtual bool Read16(const u32 addr, u16* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = 0;
			return false;
		}

		*value = FastRead16(addr);
		return true;
	}

	virtual bool Read32(const u32 addr, u32* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = 0;
			return false;
		}

		*value = FastRead32(addr);
		return true;
	}

	virtual bool Read64(const u32 addr, u64* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = 0;
			return false;
		}

		*value = FastRead64(addr);
		return true;
	}

	virtual bool Read128(const u32 addr, u128* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = u128::From((u64)0);
			return false;
		}

		value->lo = FastRead64(addr);
		value->hi = FastRead64(addr + 8);
		return true;
	}

	__forceinline void FastWrite8(const u32 addr, const u8 value)
	{
		((u8*)mem)[addr - GetStartAddr()] = value;
	}

	__forceinline void FastWrite16(const u32 addr, const u16 value)
	{
		const u32 _addr = addr - GetStartAddr();
		((u8*)mem)[_addr] = (u8)(value >> 8);
		((u8*)mem)[_addr + 1] = (u8)value;
	}

	__forceinline void FastWrite32(const u32 addr, const u32 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite16(_addr, (u16)(value >> 16));
		FastWrite16(_addr+2, (u16)value);
	}

	__forceinline void FastWrite64(const u32 addr, const u64 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite32(_addr, (u16)(value >> 32));
		FastWrite32(_addr+4, (u16)value);
	}

	__forceinline void FastWrite128(const u32 addr, const u128 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite32(_addr, value.hi);
		FastWrite32(_addr+8, value.lo);
	}

	virtual bool Write8(const u32 addr, const u8 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite8(addr, value);
		return true;
	}

	virtual bool Write16(const u32 addr, const u16 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite16(addr, value);
		return true;
	}

	virtual bool Write32(const u32 addr, const u32 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite32(addr, value);
		return true;
	}

	virtual bool Write64(const u32 addr, const u64 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite64(addr, value);
		return true;
	}

	virtual bool Write128(const u32 addr, const u128 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite128(addr, value);
		return true;
	}

	const u32 GetStartAddr() const { return range_start; }
	const u32 GetEndAddr() const { return range_end; }
	const u32 GetSize() const { return range_size; }
	const u8 GetNumber() const { return number; }
	void* GetMem() const { return mem; }
};

class NullMemoryBlock : public MemoryBlock
{
	virtual bool IsMyAddress(const u32 addr)
	{
		return true;
	}

	virtual bool Read8(const u32 addr, u8* value)
	{
		ConLog.Error("Read8 from null block: [%08x]", addr);
		return false;
	}

	virtual bool Read16(const u32 addr, u16* value)
	{
		ConLog.Error("Read16 from null block: [%08x]", addr);
		return false;
	}

	virtual bool Read32(const u32 addr, u32* value)
	{
		ConLog.Error("Read32 from null block: [%08x]", addr);
		return false;
	}

	virtual bool Read64(const u32 addr, u64* value)
	{
		ConLog.Error("Read64 from null block: [%08x]", addr);
		return false;
	}

	virtual bool Read128(const u32 addr, u128* value)
	{
		ConLog.Error("Read128 from null block: [%08x]", addr);
		return false;
	}

	virtual bool Write8(const u32 addr, const u8 value)
	{
		ConLog.Error("Write8 to null block: [%08x]: %x", addr, value);
		return false;
	}

	virtual bool Write16(const u32 addr, const u16 value)
	{
		ConLog.Error("Write16 to null block: [%08x]: %x", addr, value);
		return false;
	}

	virtual bool Write32(const u32 addr, const u32 value)
	{
		ConLog.Error("Write32 to null block: [%08x]: %x", addr, value);
		return false;
	}

	virtual bool Write64(const u32 addr, const u64 value)
	{
		ConLog.Error("Write64 to null block: [%08x]: %x", addr, value);
		return false;
	}

	virtual bool Write128(const u32 addr, const u128 value)
	{
		ConLog.Error("Write128 to null block: [%08x]: %x_%x", addr, value.hi, value.lo);
		return false;
	}
};

class MemoryBase
{
	NullMemoryBlock NullMem;

public:
	MemoryBlock MainRam;
	MemoryBlock Video_FrameBuffer;
	MemoryBlock Video_GPUdata;

	u8 mem_count;

	bool m_inited;

	MemoryBase()
	{
		m_inited = false;
	}

	~MemoryBase()
	{
		Close();
	}

	MemoryBlock& GetMemByNum(const u8 num)
	{
		if(num == MainRam.			GetNumber()) return MainRam;
		if(num == Video_FrameBuffer.GetNumber()) return Video_FrameBuffer;
		if(num == Video_GPUdata.	GetNumber()) return Video_GPUdata;

		ConLog.Error("Unknown memory number! (%d)", num);
		return NullMem;
	}

	MemoryBlock& GetMemByAddr(const u32 addr)
	{
		for(uint i=0; i<mem_count; ++i)
		{
			MemoryBlock& mem = GetMemByNum(i);
			if(mem.IsMyAddress(addr)) return mem;
		}

		ConLog.Error("Unknown memory address! (%d)", addr);
		return NullMem;
	}

	void Init()
	{
		if(m_inited) return;
		m_inited = true;

		ConLog.Write("Initing memory...");

		MainRam.SetRange(0x00000000, 0x0FFFFFFF); //256 MB
		Video_FrameBuffer.SetRange(MainRam.GetEndAddr(), 0x0FBFFFFF); //252 MB
		Video_GPUdata.SetRange(Video_FrameBuffer.GetEndAddr(), 0x003FFFFF); //4 MB

		mem_count = 0;

		MainRam.SetNumber(mem_count++);
		Video_FrameBuffer.SetNumber(mem_count++);
		Video_GPUdata.SetNumber(mem_count++);
	}

	void Close()
	{
		if(!m_inited) return;
		m_inited = false;

		ConLog.Write("Closing memory...");

		MainRam.Delete();
		Video_FrameBuffer.Delete();
		Video_GPUdata.Delete();
	}

	void Reset()
	{
		if(!m_inited) return;

		ConLog.Write("Resetting memory...");
		Close();
		Init();
	}
	//u8* GetMem(u32& addr);
	void Write8(u32 addr, const u8 data);
	void Write16(u32 addr, const u16 data);
	void Write32(u32 addr, const u32 data);
	void Write64(u32 addr, const u64 data);
	void Write128(u32 addr, const u128 data);

	u8 Read8(u32 addr);
	u16 Read16(u32 addr);
	u32 Read32(u32 addr);
	u64 Read64(u32 addr);
	u128 Read128(u32 addr);
};

extern MemoryBase Memory;