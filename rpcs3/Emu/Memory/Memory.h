#pragma once

class MemoryBase
{
public:
	u8* MainRam;
	u8* Video_FrameBuffer;
	u8* Video_GPUdata;

	~MemoryBase()
	{
		Close();
	}

	void Init()
	{
		MainRam				= new u8[0x0FFFFFFF]; //256 MB
		Video_FrameBuffer	= new u8[0x0FBFFFFF]; //252 MB
		Video_GPUdata		= new u8[0x003FFFFF]; //4 MB
	}

	void Close()
	{
		safe_delete(MainRam);
		safe_delete(Video_FrameBuffer);
		safe_delete(Video_GPUdata);
	}

	u8* GetMem(u64& addr);

	void Write8(u64 addr, const u8 data);
	void Write16(u64 addr, const u16 data);
	void Write32(u64 addr, const u32 data);
	void Write64(u64 addr, const u64 data);
	void Write128(u64 addr, const u128 data);

	u8 Read8(u64 addr);
	u16 Read16(u64 addr);
	u32 Read32(u64 addr);
	u64 Read64(u64 addr);
	u128 Read128(u64 addr);
};

extern MemoryBase Memory;