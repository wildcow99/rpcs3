#pragma once

enum
{
	XER_SO = 0x80000000,
	XER_OV = 0x40000000,
	XER_CA = 0x20000000,
};

enum
{
	CR0_LT = 0x80000000,
	CR0_GT = 0x40000000,
	CR0_EQ = 0x20000000,
	CR0_SO = 0x10000000,
};

static const s32 MAX_INT_VALUE = 0x7fffffff;

class CPUCycle
{
public:
	bool isBranch;

	u32 PC;
	u32 nPC;

	float FPR[32];
	s64 GPR[32]; //General-Purpose Register
	s32 CR[8]; //Condition Register

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

	s32 XER; //SPR 0x001 : Fixed-Point Expection Register
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

	s32 LR;		//SPR 0x008 : Link Register
	s32 CTR;	//SPR 0x009 : Count Register

	s32 USPRG;	//SPR 0x100 : User-SPR General-Purpose Registers

	s32 SPRG[8]; //SPR 0x100 - 0x107 : SPR General-Purpose Registers

	//TBR : Time-Base Registers
	s32 TBU;	//TBR 0x10C
	s32 TBL;	//TBR 0x10D

	bool BO[5];
	// 0 : CR Test Control
	// : 0 - Test the CR bit specifed by BI opcode field for the value indicated by BO[1]
	// : 1 - Do not test CR
	// 1 : CR Test Value
	// : 0 - Test for CR[BI] == 0
	// : 1 - Test for CR[BI] == 1
	// 2 : CTR Test Control
	// : 0 - Decrement CTR by one, and test whether CTR satisfies the condition specifed by BO[3]
	// : 1 - Do not change or test CTR
	// 3 : CTR Test Value
	// : 0 - Test for CTR != 0
	// : 1 - Test for CTR == 0
	// 4 : Branch Prediction Reversal
	// : 0 - Apply standard branch prediction
	// : 1 - Reverse the standard branch prediction

	u64 cycle;

	CPUCycle()
	{
		Reset();
	}

	inline bool CheckOverflow(const s64 a, const s64 b)
	{
		return
			a > 0 && b > 0 && (MAX_INT_VALUE - b) < a ||
			a < 0 && b < 0 && (-MAX_INT_VALUE - b) > a;
	}

	inline bool CheckUnderflow(const s64 a, const s64 b)
	{
		return
			a > 0 && b < 0 && (MAX_INT_VALUE + b) < a ||
			a < 0 && b > 0 && (-MAX_INT_VALUE + b) > a;
	}

	inline bool IsCarryGen(const s64 a, const s64 b)
	{
		if(a == 0 || b == 0) return 0;
		
		s64 c = 0;
		for(uint i=0; i<32; i++)
		{
			const s64 x = (a >> i) & 0x1;
			const s64 y = (b >> i) & 0x1;
			c = (x * y) | (x * c) | (y * c);
		}
		
		return !!c;
	}

	inline void UpdateCR_LT(const uint n, const bool set)
	{
		if(set)
		{
			CR[n] |= CR0_LT;
		}
		else
		{
			CR[n] &= ~CR0_LT;
		}
	}

	inline void UpdateCR_GT(const uint n, const bool set)
	{
		if(set)
		{
			CR[n] |= CR0_GT;
		}
		else
		{
			CR[n] &= ~CR0_GT;
		}
	}

	inline void UpdateCR_EQ(const uint n, const bool set)
	{
		if(set)
		{
			CR[n] |= CR0_EQ;
		}
		else
		{
			CR[n] &= ~CR0_EQ;
		}
	}

	inline void UpdateCR_SO(const uint n, const bool set)
	{
		if(set)
		{
			CR[n] |= CR0_SO;
		}
		else
		{
			CR[n] &= ~CR0_SO;
		}
	}

	inline void UpdateCRn(const uint n, const int lvalue, const int rvalue)
	{
		UpdateCR_LT(n, lvalue < rvalue);
		UpdateCR_GT(n, lvalue > rvalue);
		UpdateCR_EQ(n, lvalue == rvalue);
		UpdateCR_SO(n, IsXER_SO());
	}

	inline void UpdateCR0(const int value)
	{
		UpdateCRn(0, value, 0);
	}

	inline void UpdateXER_CA(const bool set)
	{
		if(set)
		{
			XER |= XER_CA;
		}
		else
		{
			XER &= ~XER_CA;
		}
	}

	inline void UpdateXER_OV(const bool set)
	{
		if(set)
		{
			XER |= XER_OV;
		}
		else
		{
			XER &= ~XER_OV;
		}
	}
	
	inline void UpdateXER_SO_OV(const bool set)
	{
		UpdateXER_OV(set);
		if(set) XER |= XER_SO;
	}

	inline void UpdateXER_CA(const s64 lvalue, const s64 rvalue)
	{
		UpdateXER_CA(IsCarryGen(lvalue, rvalue));
	}

	inline void UpdateXER_CA(const s64 lvalue, const s64 value, const s64 rvalue)
	{
		UpdateXER_CA(IsCarryGen(lvalue, value) || (rvalue && IsCarryGen(lvalue + value, rvalue)));
	}

	inline void UpdateXER_OV(const s64 lvalue, const s64 value, const s64 rvalue)
	{
		UpdateXER_OV(CheckUnderflow(lvalue, rvalue));
	}

	inline void UpdateXER_SO_OV(const s64 lvalue, const s64 rvalue)
	{
		UpdateXER_SO_OV(CheckUnderflow(lvalue, rvalue));
	}

	inline void UpdateXER_SO_OV(const s64 lvalue, const s64 value, const s64 rvalue)
	{
		UpdateXER_SO_OV(CheckOverflow(lvalue, rvalue)
			|| (rvalue && IsCarryGen(lvalue + value, rvalue)));
	}

	const bool IsXER_CA() const { return !!(XER & XER_CA); }
	const bool IsXER_SO() const { return !!(XER & XER_SO); }

	virtual void Reset();
	virtual void NextPc();
	virtual void NextBranchPc();
	virtual void PrevPc();
	virtual void SetBranch(const u32 pc);
	virtual void SetPc(const u32 pc);
};

extern CPUCycle CPU;