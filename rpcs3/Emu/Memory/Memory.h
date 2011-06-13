#pragma once

class MemoryBase
{
public:
	u8* MainRam;
	u8* Video_FrameBuffer;
	u8* Video_GPUdata;

	bool m_inited;

	MemoryBase()
	{
		m_inited = false;
	}

	~MemoryBase()
	{
		Close();
	}

	void Init()
	{
		if(m_inited) return;
		m_inited = true;

		MainRam				= new u8[0x0FFFFFFF]; //256 MB
		Video_FrameBuffer	= new u8[0x0FBFFFFF]; //252 MB
		Video_GPUdata		= new u8[0x003FFFFF]; //4 MB
	}

	void Close()
	{
		if(!m_inited) return;
		m_inited = false;

		safe_delete(MainRam);
		safe_delete(Video_FrameBuffer);
		safe_delete(Video_GPUdata);
	}

	u8* GetMem(u32& addr);

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