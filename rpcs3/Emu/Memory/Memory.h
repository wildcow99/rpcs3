#pragma once

class MemoryBlock
{
private:
	u8* mem;
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

	virtual bool IsNULL() { return false; }

	bool GetMemFromAddr(void* dst, const u32 addr, const u32 size)
	{
		if(!IsMyAddress(addr)) return false;
		if((addr - GetStartAddr()) + size > GetSize()) return false;
		memcpy(dst, &mem[addr - GetStartAddr()], size);
		return true;
	}

	bool SetMemFromAddr(void* src, const u32 addr, const u32 size)
	{
		if(!IsMyAddress(addr)) return false;
		if((addr - GetStartAddr()) + size > GetSize()) return false;
		memcpy(&mem[addr - GetStartAddr()], src, size);
		return true;
	}

	bool GetMemFFromAddr(void* dst, const u32 addr)
	{
		if(!IsMyAddress(addr)) return false;
		dst = &mem[addr - GetStartAddr()];
		return true;
	}

	u8* GetMemFromAddr(const u32 addr)
	{
		if(!IsMyAddress(addr)) return NULL;

		return &mem[addr];
	}

	void SetNumber(const u8 num)
	{
		number = num;
	}

	void SetRange(const u32 start, const u32 size)
	{
		range_start = start;
		range_size = size;
		range_end = start + size + 1;

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
		return mem[addr - GetStartAddr()];
	}

	__forceinline const u16 FastRead16(const u32 addr)
	{
		return ((s16)FastRead8(addr) << 8) | (s16)FastRead8(addr + 1);
	}

	__forceinline const u32 FastRead32(const u32 addr)
	{
		return ((s32)FastRead16(addr) << 16) | (s32)FastRead16(addr + 2);
	}

	__forceinline const u64 FastRead64(const u32 addr)
	{
		return ((s64)FastRead32(addr) << 32) | (s64)FastRead32(addr + 4);
	}

	__forceinline const u128 FastRead128(const u32 addr)
	{
		u128 ret;
		ret.hi = (u64)FastRead32(addr);
		ret.lo = (u64)FastRead32(addr + 8);
		return ret;
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

		*value = FastRead128(addr);
		return true;
	}

	__forceinline void FastWrite8(const u32 addr, const s8 value)
	{
		mem[addr - GetStartAddr()] = value;
	}

	__forceinline void FastWrite16(const u32 addr, const s16 value)
	{
		const u32 _addr = addr - GetStartAddr();
		mem[_addr] = (s8)(value >> 8);
		mem[_addr+1] = (s8)value;
	}

	__forceinline void FastWrite32(const u32 addr, const s32 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite16(_addr, (s16)(value >> 16));
		FastWrite16(_addr+2, (s16)value);
	}

	__forceinline void FastWrite64(const u32 addr, const s64 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite32(_addr, (s32)(value >> 32));
		FastWrite32(_addr+4, (s32)value);
	}

	__forceinline void FastWrite128(const u32 addr, const u128 value)
	{
		const u32 _addr = addr - GetStartAddr();
		FastWrite64(_addr, value.hi);
		FastWrite64(_addr+8, value.lo);
	}

	virtual bool Write8(const u32 addr, const s8 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite8(addr, value);
		return true;
	}

	virtual bool Write16(const u32 addr, const s16 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite16(addr, value);
		return true;
	}

	virtual bool Write32(const u32 addr, const s32 value)
	{
		if(!IsMyAddress(addr)) return false;

		FastWrite32(addr, value);
		return true;
	}

	virtual bool Write64(const u32 addr, const s64 value)
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
	virtual bool IsNULL() { return true; }

	virtual bool IsMyAddress(const u32 addr)
	{
		return true;
	}

	virtual bool Read8(const u32 addr, u8* WXUNUSED(value))
	{
		ConLog.Error("Read8 from null block: [%08x]", addr);
		Emu.Pause();
		return false;
	}

	virtual bool Read16(const u32 addr, u16* WXUNUSED(value))
	{
		ConLog.Error("Read16 from null block: [%08x]", addr);
		Emu.Pause();
		return false;
	}

	virtual bool Read32(const u32 addr, u32* WXUNUSED(value))
	{
		ConLog.Error("Read32 from null block: [%08x]", addr);
		Emu.Pause();
		return false;
	}

	virtual bool Read64(const u32 addr, u64* WXUNUSED(value))
	{
		ConLog.Error("Read64 from null block: [%08x]", addr);
		Emu.Pause();
		return false;
	}

	virtual bool Read128(const u32 addr, u128* WXUNUSED(value))
	{
		ConLog.Error("Read128 from null block: [%08x]", addr);
		Emu.Pause();
		return false;
	}

	virtual bool Write8(const u32 addr, const u8 value)
	{
		ConLog.Error("Write8 to null block: [%08x]: %x", addr, value);
		Emu.Pause();
		return false;
	}

	virtual bool Write16(const u32 addr, const u16 value)
	{
		ConLog.Error("Write16 to null block: [%08x]: %x", addr, value);
		Emu.Pause();
		return false;
	}

	virtual bool Write32(const u32 addr, const u32 value)
	{
		ConLog.Error("Write32 to null block: [%08x]: %x", addr, value);
		Emu.Pause();
		return false;
	}

	virtual bool Write64(const u32 addr, const u64 value)
	{
		ConLog.Error("Write64 to null block: [%08x]: %x", addr, value);
		Emu.Pause();
		return false;
	}

	virtual bool Write128(const u32 addr, const u128 value)
	{
		ConLog.Error("Write128 to null block: [%08x]: %x_%x", addr, value.hi, value.lo);
		Emu.Pause();
		return false;
	}
};

class MemoryFlags
{
	wxArrayInt m_memflags;
	u32 FStub_lo;
	u32 FStub_hi;
	u32 FNID_lo;
	u32 FNID_hi;
	u32 opd_lo;
	u32 opd_hi;

public:
	void Add(const u32 addr)
	{
		m_memflags.Add(addr);
	}

	void Clear()
	{
		m_memflags.Clear();
		FStub_lo = 0;
		FStub_hi = 0;
		FNID_lo = 0;
		FNID_hi = 0;
		opd_lo = 0;
		opd_hi = 0;
	}

	bool IsFStubRange(const u32 addr) const
	{
		return addr >= FStub_lo && addr <= FStub_hi;
	}

	bool IsFNIDRange(const u32 addr) const
	{
		return addr >= FNID_lo && addr <= FNID_hi;
	}

	bool IsOPDRange(const u32 addr) const
	{
		return addr >= opd_lo && addr <= opd_hi;
	}

	u32 FindAddr(const u32 addr)
	{
		if(IsFStubRange(addr))
		{
			const u32 FNID_addr = FNID_lo + (addr - FStub_lo);
			if(IsFNIDRange(FNID_addr))
			{
				return FNID_addr;
			}
		}

		return addr;
	}

	void SetFStubRange(const u32 from, const u32 size)
	{
		FStub_lo = from;
		FStub_hi = from + size;
	}

	void SetFNIDRange(const u32 from, const u32 size)
	{
		FNID_lo = from;
		FNID_hi = from + size;
	}

	void SetOPDRange(const u32 from, const u32 size)
	{
		opd_lo = from;
		opd_hi = from + size;
	}

	bool IsFlag(const u32 addr, const u32 pc, s64& gpr)
	{
		for(uint i=0; i<m_memflags.GetCount(); ++i)
		{
			if(m_memflags[i] == addr)
			{
				if(IsOPDRange(pc)) gpr = m_memflags[i+1];
				return true;
			}
		}

		return false;
	}

	u64 GetCount() const { return m_memflags.GetCount(); }
};

class MemoryBase
{
	NullMemoryBlock NullMem;

public:
	MemoryBlock MainRam;
	MemoryBlock Video_FrameBuffer;
	MemoryBlock Video_GPUdata;

	MemoryFlags MemFlags;

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

	static s16 Reverse16(const s16 val)
	{
		return ((val >> 8) & 0xff) | ((val << 8) & 0xff00);
	}

	static s32 Reverse32(const s32 val)
	{
		return
			((val >> 24) & 0x000000ff) |
			((val >>  8) & 0x0000ff00) |
			((val <<  8) & 0x00ff0000) |
			((val << 24) & 0xff000000);
	}

	static s64 Reverse64(const s64 val)
	{
		return
			((val >> 56) & 0x00000000000000ff) |
			((val >> 40) & 0x000000000000ff00) |
			((val >> 24) & 0x0000000000ff0000) |
			((val >>  8) & 0x00000000ff000000) |
			((val <<  8) & 0x000000ff00000000) |
			((val << 24) & 0x0000ff0000000000) |
			((val << 40) & 0x00ff000000000000) |
			((val << 56) & 0xff00000000000000);
	}

	MemoryBlock& GetMemByNum(const u8 num)
	{
		if(num == MainRam.			GetNumber()) return MainRam;
		if(num == Video_FrameBuffer.GetNumber()) return Video_FrameBuffer;
		if(num == Video_GPUdata.	GetNumber()) return Video_GPUdata;

		return NullMem;
	}

	MemoryBlock& GetMemByAddr(const u32 addr)
	{
		for(uint i=0; i<mem_count; ++i)
		{
			MemoryBlock& mem = GetMemByNum(i);
			if(mem.IsMyAddress(addr)) return mem;
		}

		return NullMem;
	}

	u8* GetMemFromAddr(const u32 addr)
	{
		return GetMemByAddr(addr).GetMemFromAddr(addr);
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

	bool IsGoodAddr(const u32 addr)
	{
		for(uint i=0; i<mem_count; ++i)
		{
			if(GetMemByNum(i).IsMyAddress(addr)) return true;
		}

		return false;
	}

	void Close()
	{
		if(!m_inited) return;
		m_inited = false;

		ConLog.Write("Closing memory...");

		for(uint i=0; i<mem_count; ++i)
		{
			GetMemByNum(i).Delete();
		}

		MemFlags.Clear();
	}

	void Reset()
	{
		if(!m_inited) return;

		ConLog.Write("Resetting memory...");
		Close();
		Init();
	}

	void Write8(const u32 addr, const u8 data);
	void Write16(const u32 addr, const u16 data);
	void Write32(const u32 addr, const u32 data);
	void Write64(const u32 addr, const u64 data);
	void Write128(const u32 addr, const u128 data);

	u8 Read8(const u32 addr);
	u16 Read16(const u32 addr);
	u32 Read32(const u32 addr);
	u64 Read64(const u32 addr);
	u128 Read128(const u32 addr);
};

extern MemoryBase Memory;