#pragma once
#include "MemoryBlock.h"

class MemoryFlags
{
	wxArrayInt m_memflags;
	u64 FStub_lo;
	u64 FStub_hi;
	u64 Stub_lo;
	u64 Stub_hi;
	u64 FNID_lo;
	u64 FNID_hi;
	u64 opd_lo;
	u64 opd_hi;

public:
	void Add(const u32 addr) {m_memflags.Add(addr);}

	void Clear()
	{
		m_memflags.Clear();
		FStub_lo = 0;
		FStub_hi = 0;
		Stub_lo = 0;
		Stub_hi = 0;
		FNID_lo = 0;
		FNID_hi = 0;
		opd_lo = 0;
		opd_hi = 0;
	}

	bool IsFStubRange(const u64 addr) const
	{
		return addr >= FStub_lo && addr < FStub_hi;
	}

	bool IsStubRange(const u64 addr) const
	{
		return addr >= Stub_lo && addr < Stub_hi;
	}

	bool IsFNIDRange(const u64 addr) const
	{
		return addr >= FNID_lo && addr < FNID_hi;
	}

	bool IsOPDRange(const u64 addr) const
	{
		return addr >= opd_lo && addr < opd_hi;
	}

	u32 FindAddr(const u64 addr)
	{
		if(!IsStubRange(addr)) return addr;
		return FNID_lo + (((addr - Stub_lo) / 0x20) * 4);
	}

	void SetFStubRange(const u64 from, const u64 size)
	{
		FStub_lo = from;
		FStub_hi = from + size;
	}

	void SetStubRange(const u64 from, const u64 size)
	{
		Stub_lo = from;
		Stub_hi = from + size;
	}

	void SetFNIDRange(const u64 from, const u64 size)
	{
		FNID_lo = from;
		FNID_hi = from + size;
	}

	void SetOPDRange(const u64 from, const u64 size)
	{
		opd_lo = from;
		opd_hi = from + size;
	}

	bool IsFlag(const u64 addr, const u64 pc, u64& gpr)
	{
		for(uint i=0; i<GetCount()-1; i+=2)
		{
			if(m_memflags[i] != addr) continue;
			/*if(IsOPDRange(pc))*/ gpr = m_memflags[i+1];
			return true;
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
	ArrayF<MemoryBlock> MemoryBlocks;

	MemoryFlags MemFlags;

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
		if(num >= MemoryBlocks.GetCount()) return NullMem;
		return MemoryBlocks.Get(num);
	}

	MemoryBlock& GetMemByAddr(const u32 addr)
	{
		for(uint i=0; i<MemoryBlocks.GetCount(); ++i)
		{
			if(MemoryBlocks.Get(i).IsMyAddress(addr)) return MemoryBlocks.Get(i);
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
		Video_FrameBuffer.SetRange(MainRam.GetEndAddr(), 0x0FFFFFFF); //252 MB
		Video_GPUdata.SetRange(Video_FrameBuffer.GetEndAddr(), 0x003FFFFF); //4 MB
		MemoryBlocks.Add(MainRam);
		MemoryBlocks.Add(Video_FrameBuffer);
		MemoryBlocks.Add(Video_GPUdata);
	}

	bool IsGoodAddr(const u32 addr)
	{
		for(uint i=0; i<MemoryBlocks.GetCount(); ++i)
		{
			if(MemoryBlocks.Get(i).IsMyAddress(addr)) return true;
		}

		return false;
	}

	void Close()
	{
		if(!m_inited) return;
		m_inited = false;

		ConLog.Write("Closing memory...");

		for(uint i=0; i<MemoryBlocks.GetCount(); ++i)
		{
			MemoryBlocks[i].Delete();
		}

		MemoryBlocks.Clear();
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

	wxString ReadString(const u32 addr, const u64 len)
	{
		wxString str;
		str.Clear();
		wxStringBuffer buf(str, len);
		memcpy((wxChar*)buf, (const void*)GetMemFromAddr(addr), len);

		return str;
	}

	wxString ReadString(const u32 addr)
	{
		wxString buf = wxEmptyString;

		for(u32 i=addr; ; i++)
		{
			const u8 c = Read8(i);
			if(c == 0) break;
			buf += c;
		}

		return buf;
	}

	void WriteString(const u32 addr, const wxString& str)
	{
		for(u32 i=0; i<str.Length(); i++)
		{
			Write8(addr + i, str[i]);
		}

		Write8(addr + str.Length(), 0);
	}
};

extern MemoryBase Memory;