#pragma once

class MemoryBase
{
public:
	u8* MainRam;
	u8* VideoMem;

	~MemoryBase()
	{
		Close();
	}

	void Init()
	{
		MainRam  = new u8[0x0FFFFFFF]; //256 MB
		VideoMem = new u8[0x0FFFFFFF]; //256 MB
	}

	void Close()
	{
		free(MainRam);
		free(VideoMem);

		MainRam = NULL;
		VideoMem = NULL;
	}

	u8* GetMem(uint& addr);

	void Write32(uint addr, const u32 data);

	u8 Read8(uint addr);
	u16 Read16(uint addr);
	u32 Read32(uint addr);
	u64 Read64(uint addr);
	u128 Read128(uint addr);
};

extern MemoryBase Memory;