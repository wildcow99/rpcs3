#pragma once

class MemoryBlock
{
private:
	u8* mem;
	u32 range_start;
	u32 range_end;
	u32 range_size;

public:
	MemoryBlock();
	~MemoryBlock();

private:
	void Init();
	void InitMemory();

public:
	void Delete();

	virtual bool IsNULL() { return false; }

	u32 FixAddr(const u32 addr) const;

	bool GetMemFromAddr(void* dst, const u32 addr, const u32 size);
	bool SetMemFromAddr(void* src, const u32 addr, const u32 size);
	bool GetMemFFromAddr(void* dst, const u32 addr);
	u8* GetMemFromAddr(const u32 addr);

	void SetRange(const u32 start, const u32 size);
	virtual bool IsMyAddress(const u32 addr);

	__forceinline const u8 FastRead8(const u32 addr) const;
	__forceinline const u16 FastRead16(const u32 addr) const;
	__forceinline const u32 FastRead32(const u32 addr) const;
	__forceinline const u64 FastRead64(const u32 addr) const;
	__forceinline const u128 FastRead128(const u32 addr);

	virtual bool Read8(const u32 addr, u8* value);
	virtual bool Read16(const u32 addr, u16* value);
	virtual bool Read32(const u32 addr, u32* value);
	virtual bool Read64(const u32 addr, u64* value);
	virtual bool Read128(const u32 addr, u128* value);

	__forceinline void FastWrite8(const u32 addr, const s8 value);
	__forceinline void FastWrite16(const u32 addr, const u16 value);
	__forceinline void FastWrite32(const u32 addr, const u32 value);
	__forceinline void FastWrite64(const u32 addr, const u64 value);
	__forceinline void FastWrite128(const u32 addr, const u128 value);

	virtual bool Write8(const u32 addr, const s8 value);
	virtual bool Write16(const u32 addr, const s16 value);
	virtual bool Write32(const u32 addr, const s32 value);
	virtual bool Write64(const u32 addr, const s64 value);
	virtual bool Write128(const u32 addr, const u128 value);

	const u32 GetStartAddr() const { return range_start; }
	const u32 GetEndAddr() const { return range_end; }
	const u32 GetSize() const { return range_size; }
	void* GetMem() const { return mem; }
};

class NullMemoryBlock : public MemoryBlock
{
	virtual bool IsNULL() { return true; }
	virtual bool IsMyAddress(const u32 addr) { return true; }

	virtual bool Read8(const u32 addr, u8* value);
	virtual bool Read16(const u32 addr, u16* value);
	virtual bool Read32(const u32 addr, u32* value);
	virtual bool Read64(const u32 addr, u64* value);
	virtual bool Read128(const u32 addr, u128* value);

	virtual bool Write8(const u32 addr, const u8 value);
	virtual bool Write16(const u32 addr, const u16 value);
	virtual bool Write32(const u32 addr, const u32 value);
	virtual bool Write64(const u32 addr, const u64 value);
	virtual bool Write128(const u32 addr, const u128 value);
};