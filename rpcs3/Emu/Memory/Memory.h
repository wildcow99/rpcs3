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
		//mem = malloc(range_size * sizeof(u8));
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

	template<typename T> bool Write(u32 addr, T value)
	{
		if(!IsMyAddress(addr)) return false;

		(((T*)mem)[addr - GetStartAddr()]) = value;
		return true;
	}

	template<typename T> bool Read(u32 addr, T* value)
	{
		if(!IsMyAddress(addr))
		{
			*value = 0;
			return false;
		}

		*value = (((T*)mem)[addr - GetStartAddr()]);
		return true;
	}

	virtual bool Read8(const u32 addr, u8* value)
	{
		return Read<u8>(addr, value);
	}

	virtual bool Read16(const u32 addr, u16* value)
	{
		return Read<u16>(addr, value);
	}

	virtual bool Read32(const u32 addr, u32* value)
	{
		return Read<u32>(addr, value);
	}

	virtual bool Read64(const u32 addr, u64* value)
	{
		return Read<u64>(addr, value);
	}

	virtual bool Read128(const u32 addr, u128* value)
	{
		return Read<u64>(addr, &value->lo) && Read<u64>(addr + 8, &value->hi);
	}

	virtual bool Write8(const u32 addr, const u8 value)
	{
		return Write<u8>(addr, value);
	}

	virtual bool Write16(const u32 addr, const u16 value)
	{
		return Write<u16>(addr, value);
	}

	virtual bool Write32(const u32 addr, const u32 value)
	{
		return Write<u32>(addr, value);
	}

	virtual bool Write64(const u32 addr, const u64 value)
	{
		return Write<u64>(addr, value);
	}

	virtual bool Write128(const u32 addr, const u128 value)
	{
		return Write<u64>(addr, value.lo) && Write<u64>(addr + 8, value.hi);
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