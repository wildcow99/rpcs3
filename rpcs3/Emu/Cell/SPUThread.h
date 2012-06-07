#pragma once
#include "PPCThread.h"

union SPU_GPR_hdr
{
	//__m128i _m128i;

	u128 _u128;
	s128 _i128;
	u64 _u64[2];
	s64 _i64[2];
	u32 _u32[4];
	s32 _i32[4];
	u16 _u16[8];
	s16 _i16[8];
	u8  _u8[16];
	s8  _i8[16];

	SPU_GPR_hdr() {}
	SPU_GPR_hdr(const __m128i val){_u128._u64[0] = val.m128i_u64[0]; _u128._u64[1] = val.m128i_u64[1];}
	SPU_GPR_hdr(const u128 val) {			_u128	= val; }
	SPU_GPR_hdr(const u64  val) { Reset(); _u64[0]	= val; }
	SPU_GPR_hdr(const u32  val) { Reset(); _u32[0]	= val; }
	SPU_GPR_hdr(const u16  val) { Reset(); _u16[0]	= val; }
	SPU_GPR_hdr(const u8   val) { Reset(); _u8[0]	= val; }
	SPU_GPR_hdr(const s128 val) {			_i128	= val; }
	SPU_GPR_hdr(const s64  val) { Reset(); _i64[0]	= val; }
	SPU_GPR_hdr(const s32  val) { Reset(); _i32[0]	= val; }
	SPU_GPR_hdr(const s16  val) { Reset(); _i16[0]	= val; }
	SPU_GPR_hdr(const s8   val) { Reset(); _i8[0]	= val; }
	
	wxString ToString() const
	{
		return wxString::Format("%08x%08x%08x%08x", _u32[3], _u32[2], _u32[1], _u32[0]);
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}

	operator __m128i() { __m128i ret; ret.m128i_u64[0]=_u128._u64[0]; ret.m128i_u64[1]=_u128._u64[1]; return ret; }

	SPU_GPR_hdr operator ^ (__m128i right)	{ return _mm_xor_si128(*this, right); }
	SPU_GPR_hdr operator | (__m128i right)	{ return _mm_or_si128 (*this, right); }
	SPU_GPR_hdr operator & (__m128i right)	{ return _mm_and_si128(*this, right); }
	SPU_GPR_hdr operator << (int right)		{ return _mm_slli_epi32(*this, right); }
	SPU_GPR_hdr operator << (__m128i right) { return _mm_sll_epi32(*this, right); }
	SPU_GPR_hdr operator >> (int right)		{ return _mm_srai_epi32(*this, right); }
	SPU_GPR_hdr operator >> (__m128i right) { return _mm_sra_epi32(*this, right); }

	SPU_GPR_hdr operator ^= (__m128i right) { return *this = *this ^ right; }
	SPU_GPR_hdr operator |= (__m128i right) { return *this = *this | right; }
	SPU_GPR_hdr operator &= (__m128i right) { return *this = *this & right; }
	SPU_GPR_hdr operator <<= (int right)	{ return *this = *this << right; }
	SPU_GPR_hdr operator <<= (__m128i right){ return *this = *this << right; }
	SPU_GPR_hdr operator >>= (int right)	{ return *this = *this >> right; }
	SPU_GPR_hdr operator >>= (__m128i right){ return *this = *this >> right; }
};

struct Channel
{
	SPU_GPR_hdr data;
	bool used;

	Channel() : used(false)
	{
	}
};

class SPUThread : public PPCThread
{
public:
	SPU_GPR_hdr GPR[128]; //General-Purpose Register

	Channel CH[128]; //channel

	u32 LSA; //local storage address

public:
	SPUThread();
	~SPUThread();

	virtual wxString RegsToString()
	{
		wxString ret = PPCThread::RegsToString();
		for(uint i=0; i<128; ++i) ret += wxString::Format("GPR[%d] = 0x%s\n", i, GPR[i].ToString());
		return ret;
	}

public:
	virtual void InitRegs(); 
	virtual u64 GetFreeStackSize() const;

protected:
	virtual void DoReset();
	virtual void DoRun();
	virtual void DoPause();
	virtual void DoResume();
	virtual void DoStop();

private:
	virtual void DoCode(const s32 code);
};