#pragma once
#include "Emu/Cell/PPCThread.h"

enum
{
	XER_SO = 0x80000000,
	XER_OV = 0x40000000,
	XER_CA = 0x20000000,
};

enum
{
	CR_LT = 0x8,
	CR_GT = 0x4,
	CR_EQ = 0x2,
	CR_SO = 0x1,
};

enum FPSCR_EXP
{
	FPSCR_FX		= 0x80000000,
	FPSCR_FEX		= 0x40000000,
	FPSCR_VX		= 0x20000000,
	FPSCR_OX		= 0x10000000,

	FPSCR_UX		= 0x08000000,
	FPSCR_ZX		= 0x04000000,
	FPSCR_XX		= 0x02000000,
	FPSCR_VXSNAN	= 0x01000000,

	FPSCR_VXISI		= 0x00800000,
	FPSCR_VXIDI		= 0x00400000,
	FPSCR_VXZDZ		= 0x00200000,
	FPSCR_VXIMZ		= 0x00100000,

	FPSCR_VXVC		= 0x00080000,
	FPSCR_FR		= 0x00040000,
	FPSCR_FI		= 0x00020000,

	FPSCR_VXSOFT	= 0x00000400,
	FPSCR_VXSQRT	= 0x00000200,
	FPSCR_VXCVI		= 0x00000100,
};

enum FPSCR_RN
{
	FPSCR_RN_NEAR = 0,
	FPSCR_RN_ZERO = 1,
	FPSCR_RN_PINF = 2,
	FPSCR_RN_MINF = 3,
};

static const u64 DOUBLE_SIGN = 0x8000000000000000ULL;
static const u64 DOUBLE_EXP  = 0x7FF0000000000000ULL;
static const u64 DOUBLE_FRAC = 0x000FFFFFFFFFFFFFULL;
static const u64 DOUBLE_ZERO = 0x0000000000000000ULL;

union FPSCRhdr
{
	struct
	{
		u32 RN		:2;
		u32 NI		:1;
		u32 XE		:1;
		u32 ZE		:1;
		u32 UE		:1;
		u32 OE		:1;
		u32 VE		:1;
		u32 VXCVI	:1;
		u32 VXSQRT	:1;
		u32 VXSOFT	:1;
		u32			:1;
		u32 FPRF	:5;
		u32 FI		:1;
		u32 FR		:1;
		u32 VXVC	:1;
		u32 VXIMZ	:1;
		u32 VXZDZ	:1;
		u32 VXIDI	:1;
		u32 VXISI	:1;
		u32 VXSNAN	:1;
		u32 XX		:1;
		u32 ZX		:1;
		u32 UX		:1;
		u32 OX		:1;
		u32 VX		:1;
		u32 FEX		:1;
		u32 FX		:1;
	};

	u32 FPSCR;
};

union CRhdr
{
	u32 CR;

	struct
	{
		u8 cr0	: 4;
		u8 cr1	: 4;
		u8 cr2	: 4;
		u8 cr3	: 4;
		u8 cr4	: 4;
		u8 cr5	: 4;
		u8 cr6	: 4;
		u8 cr7	: 4;
	};
};

union XERhdr
{
	u64 XER;

	struct
	{
		u64	L	: 61;
		u64 CA	: 1;
		u64 OV	: 1;
		u64 SO	: 1;
	};
};

struct FPRdouble
{
	union
	{
		double d;
		u64 i;
	};

	FPRdouble() {}
	FPRdouble(double _d) : d(_d) {}
	FPRdouble(u64 _i) : i(_i) {}

	bool IsINF() { return IsINF(d); }
	bool IsNaN() { return IsNaN(d); }
	bool IsQNaN(){ return IsQNaN(d); }
	bool IsSNaN(){ return IsSNaN(d); }

	static bool IsINF(double d);
	static bool IsNaN(double d);
	static bool IsQNaN(double d);
	static bool IsSNaN(double d);

	int Cmp(FPRdouble& f);
};

union VPR_reg
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

	VPR_reg(){Clear();}
	VPR_reg(const __m128i val){_u128._u64[0] = val.m128i_u64[0]; _u128._u64[1] = val.m128i_u64[1];}
	VPR_reg(const u128 val) {			_u128	= val; }
	VPR_reg(const u64  val) { Clear();	_u64[0] = val; }
	VPR_reg(const u32  val) { Clear();	_u32[0] = val; }
	VPR_reg(const u16  val) { Clear();	_u16[0] = val; }
	VPR_reg(const u8   val) { Clear();	_u8[0]	= val; }
	VPR_reg(const s128 val) {			_i128	= val; }
	VPR_reg(const s64  val) { Clear();	_i64[0] = val; }
	VPR_reg(const s32  val) { Clear();	_i32[0] = val; }
	VPR_reg(const s16  val) { Clear();	_i16[0] = val; }
	VPR_reg(const s8   val) { Clear();	_i8[0]	= val; }

	operator u128() const { return _u128; }
	operator s128() const { return _i128; }
	operator u64() const { return _u64[0]; }
	operator s64() const { return _i64[0]; }
	operator u32() const { return _u32[0]; }
	operator s32() const { return _i32[0]; }
	operator u16() const { return _u16[0]; }
	operator s16() const { return _i16[0]; }
	operator u8() const { return _u8[0]; }
	operator s8() const { return _i8[0]; }
	operator __m128i() { __m128i ret; ret.m128i_u64[0]=_u128._u64[0]; ret.m128i_u64[1]=_u128._u64[1]; return ret; }
	operator bool() const { return _u64[0] != 0 || _u64[1] != 0; }

	wxString ToString() const
	{
		return wxString::Format("%08x%08x%08x%08x", _u32[3], _u32[2], _u32[1], _u32[0]);
	}

	VPR_reg operator ^  (VPR_reg right) { return _mm_xor_si128(*this, right); }
	VPR_reg operator |  (VPR_reg right) { return _mm_or_si128 (*this, right); }
	VPR_reg operator &  (VPR_reg right) { return _mm_and_si128(*this, right); }

	VPR_reg operator ^  (__m128i right) { return _mm_xor_si128(*this, right); }
	VPR_reg operator |  (__m128i right) { return _mm_or_si128 (*this, right); }
	VPR_reg operator &  (__m128i right) { return _mm_and_si128(*this, right); }

	bool operator == (const VPR_reg& right){ return _u64[0] == right._u64[0] && _u64[1] == right._u64[1]; }

	bool operator == (const u128 right)	{ return _u64[0] == right._u64[0] && _u64[1] == right._u64[1]; }
	bool operator == (const s128 right)	{ return _i64[0] == right._i64[0] && _i64[1] == right._i64[1]; }
	bool operator == (const u64 right)	{ return _u64[0] == (u64)right && _u64[1] == 0; }
	bool operator == (const s64 right)	{ return _i64[0] == (s64)right && _i64[1] == 0; }
	bool operator == (const u32 right)	{ return _u64[0] == (u64)right && _u64[1] == 0; }
	bool operator == (const s32 right)	{ return _i64[0] == (s64)right && _i64[1] == 0; }
	bool operator == (const u16 right)	{ return _u64[0] == (u64)right && _u64[1] == 0; }
	bool operator == (const s16 right)	{ return _i64[0] == (s64)right && _i64[1] == 0; }
	bool operator == (const u8 right)	{ return _u64[0] == (u64)right && _u64[1] == 0; }
	bool operator == (const s8 right)	{ return _i64[0] == (s64)right && _i64[1] == 0; }

	bool operator != (const VPR_reg& right){ return !(*this == right); }
	bool operator != (const u128 right)	{ return !(*this == right); }
	bool operator != (const u64 right)	{ return !(*this == right); }
	bool operator != (const u32 right)	{ return !(*this == right); }
	bool operator != (const u16 right)	{ return !(*this == right); }
	bool operator != (const u8 right)	{ return !(*this == right); }
	bool operator != (const s128 right)	{ return !(*this == right); }
	bool operator != (const s64 right)	{ return !(*this == right); }
	bool operator != (const s32 right)	{ return !(*this == right); }
	bool operator != (const s16 right)	{ return !(*this == right); }
	bool operator != (const s8 right)	{ return !(*this == right); }

	void Clear() { memset(&_u128, 0, sizeof(_u128)); }
};
/*
struct VPR_table
{
	VPR_reg t[32];

	operator VPR_reg*() { return t; }

	VPR_reg& operator [] (int index)
	{
		return t[index];
	}
}
*/

static const s32 MAX_INT_VALUE = 0x7fffffff;

class PPUThread : public PPCThread
{
public:
	FPRdouble FPR[32]; //Floating Point Register
	FPSCRhdr FPSCR; //Floating Point Status and Control Register
	u64 GPR[32]; //General-Purpose Register
	VPR_reg VPR[32];
	u32 vpcr;

	CRhdr CR; //Condition Register
	//CR0
	// 0 : LT - Negative (is negative)
	// : 0 - Result is not negative
	// : 1 - Result is negative
	// 1 : GT - Positive (is positive)
	// : 0 - Result is not positive
	// : 1 - Result is positive
	// 2 : EQ - Zero (is zero)
	// : 0 - Result is not equal to zero
	// : 1 - Result is equal to zero
	// 3 : SO - Summary overflow (copy of the final state xer[S0])
	// : 0 - No overflow occurred
	// : 1 - Overflow occurred

	//CRn 
	// 0 : LT - Less than (rA > X)
	// : 0 - rA is not less than
	// : 1 - rA is less than
	// 1 : GT - Greater than (rA < X)
	// : 0 - rA is not greater than
	// : 1 - rA is greater than
	// 2 : EQ - Equal to (rA == X)
	// : 0 - Result is not equal to zero
	// : 1 - Result is equal to zero
	// 3 : SO - Summary overflow (copy of the final state XER[S0])
	// : 0 - No overflow occurred
	// : 1 - Overflow occurred

	//SPR : Special-Purpose Registers

	XERhdr XER; //SPR 0x001 : Fixed-Point Expection Register
	// 0 : SO - Summary overflow
	// : 0 - No overflow occurred
	// : 1 - Overflow occurred
	// 1 : OV - Overflow
	// : 0 - No overflow occurred
	// : 1 - Overflow occurred
	// 2 : CA - Carry
	// : 0 - Carry did not occur
	// : 1 - Carry occured
	// 3 - 24 : Reserved
	// 25 - 31 : TBC
	// Transfer-byte count

	u64 LR;		//SPR 0x008 : Link Register
	u64 CTR;	//SPR 0x009 : Count Register

	s32 USPRG;	//SPR 0x100 : User-SPR General-Purpose Registers

	s32 SPRG[8]; //SPR 0x100 - 0x107 : SPR General-Purpose Registers

	//TBR : Time-Base Registers
	u64 TB;	//TBR 0x10C - 0x10D

	u64 reserve_addr;
	bool reserve;

	u64 cycle;

public:
	PPUThread();
	~PPUThread();

	inline u8 GetCR(const u8 n) const
	{
		switch(n)
		{
		case 7: return CR.cr0;
		case 6: return CR.cr1;
		case 5: return CR.cr2;
		case 4: return CR.cr3;
		case 3: return CR.cr4;
		case 2: return CR.cr5;
		case 1: return CR.cr6;
		case 0: return CR.cr7;
		}

		return 0;
	}

	inline void SetCR(const u8 n, const u32 value)
	{
		switch(n)
		{
		case 7: CR.cr0 = value; break;
		case 6: CR.cr1 = value; break;
		case 5: CR.cr2 = value; break;
		case 4: CR.cr3 = value; break;
		case 3: CR.cr4 = value; break;
		case 2: CR.cr5 = value; break;
		case 1: CR.cr6 = value; break;
		case 0: CR.cr7 = value; break;
		}
	}

	inline void SetCRBit(const u8 n, const u32 bit, const bool value)
	{
		switch(n)
		{
		case 7: value ? CR.cr0 |= bit : CR.cr0 &= ~bit; break;
		case 6: value ? CR.cr1 |= bit : CR.cr1 &= ~bit; break;
		case 5: value ? CR.cr2 |= bit : CR.cr2 &= ~bit; break;
		case 4: value ? CR.cr3 |= bit : CR.cr3 &= ~bit; break;
		case 3: value ? CR.cr4 |= bit : CR.cr4 &= ~bit; break;
		case 2: value ? CR.cr5 |= bit : CR.cr5 &= ~bit; break;
		case 1: value ? CR.cr6 |= bit : CR.cr6 &= ~bit; break;
		case 0: value ? CR.cr7 |= bit : CR.cr7 &= ~bit; break;
		}
	}

	inline void SetCR_EQ(const u8 n, const bool value) { SetCRBit(n, CR_EQ, value); }
	inline void SetCR_GT(const u8 n, const bool value) { SetCRBit(n, CR_GT, value); }
	inline void SetCR_LT(const u8 n, const bool value) { SetCRBit(n, CR_LT, value); }	
	inline void SetCR_SO(const u8 n, const bool value) { SetCRBit(n, CR_SO, value); }

	inline bool IsCR_EQ(const u8 n) const { return (GetCR(n) & CR_EQ) ? 1 : 0; }
	inline bool IsCR_GT(const u8 n) const { return (GetCR(n) & CR_GT) ? 1 : 0; }
	inline bool IsCR_LT(const u8 n) const { return (GetCR(n) & CR_LT) ? 1 : 0; }

	template<typename T> void UpdateCRn(const u8 n, const T a, const T b)
	{
		if		(a <  b) SetCR(n, CR_LT);
		else if	(a >  b) SetCR(n, CR_GT);
		else if	(a == b) SetCR(n, CR_EQ);

		SetCR_SO(n, XER.SO);
	}

	template<typename T> void UpdateCR0(const T val)
	{
		UpdateCRn<T>(0, val, 0);
	}

	const u8 GetCRBit(const u32 bit) const { return 1 << (3 - (bit % 4)); }

	void SetCRBit(const u32 bit, bool set) { SetCRBit(bit/4, GetCRBit(bit), set); }

	const u8 IsCR(const u32 bit) const { return (GetCR(bit/4) & GetCRBit(bit)) ? 1 : 0; }

	bool IsCarry(const u64 a, const u64 b) { return b > (~a); }

	void SetFPSCRException(const FPSCR_EXP mask)
	{
		if ((FPSCR.FPSCR & mask) != mask) FPSCR.FX = 1;
		FPSCR.FPSCR |= mask;
	}

	void SetFPSCR_FI(const u32 val)
	{
		if(val) SetFPSCRException(FPSCR_XX);
        FPSCR.FI = val;
	}

	virtual wxString RegsToString()
	{
		wxString ret = PPCThread::RegsToString();
		for(uint i=0; i<32; ++i) ret += wxString::Format("GPR[%d] = 0x%llx\n", i, GPR[i]);
		for(uint i=0; i<32; ++i) ret += wxString::Format("FPR[%d] = %llf\n", i, FPR[i]);
		ret += wxString::Format("CR = 0x%08x\n", CR);
		ret += wxString::Format("LR = 0x%llx\n", LR);
		ret += wxString::Format("CTR = 0x%llx\n", CTR);
		ret += wxString::Format("XER = 0x%llx\n", XER);
		return ret;
	}

public:
	virtual void _InitStack(); 
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