#pragma once

#include <wx/string.h>
#include <wx/wx.h>
#include <wx/propdlg.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include <wx/wfstream.h>
#include <wx/dir.h>
#include <wx/generic/progdlgg.h>
#include <wx/spinctrl.h>
#include <wx/filepicker.h>

#include <wx/wxprec.h>

#define uint unsigned int

#define u8  unsigned __int8
#define u16 unsigned __int16
#define u32 unsigned __int32
#define u64 unsigned __int64

#define s8  signed __int8
#define s16 signed __int16
#define s32 signed __int32
#define s64 signed __int64

union u128
{
	struct
	{
		u64 hi;
		u64 lo;
	};

	u64 uint64[2];
	u32 uint32[4];
	u16 uint16[8];
	u8  uint8[16];

	operator u64() const { return uint64[0]; }
	operator u32() const { return uint32[0]; }
	operator u16() const { return uint16[0]; }
	operator u8()  const { return uint8[0];  }

	static u128 From( u64 src )
	{
		u128 ret = {src, 0};
		return ret;
	}

	static u128 From( u32 src )
	{
		u128 ret;
		ret.uint32[0] = src;
		ret.uint32[1] = 0;
		ret.hi = 0;
		return ret;
	}

	bool operator == ( const u128& right ) const
	{
		return (lo == right.lo) && (hi == right.hi);
	}

	bool operator != ( const u128& right ) const
	{
		return (lo != right.lo) || (hi != right.hi);
	}
};

//TODO: SSE style
/*
struct u128
{
	__m128 m_val;

	u128 GetValue128()
	{
		u128 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u64 GetValue64()
	{
		u64 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u32 GetValue32()
	{
		u32 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u16 GetValue16()
	{
		u16 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}

	u8 GetValue8()
	{
		u8 ret;
		_mm_store_ps( (float*)&ret, m_val );
		return ret;
	}
};
*/

#define _PRGNAME_ L"rpcs3"
#define _PRGVER_ L"0.0.0.1 Pre Alpha"