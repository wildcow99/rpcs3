#pragma once
#include "Thread.h"

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

class CPUThread : public StepThread
{
public:
	bool isBranch;

	u32 PC;
	u32 nPC;

	float FPR[32];
	s64 GPR[32]; //General-Purpose Register
	s32 CR; //Condition Register

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

public:
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
		if(a == 0 || b == 0) return false;
		
		s64 c = 0;
		for(uint i=0; i<32; i++)
		{
			const s64 x = (a >> i) & 0x1;
			const s64 y = (b >> i) & 0x1;
			c = (x * y) | (x * c) | (y * c);
		}
		
		return c > 0;
	}

	inline void UpdateCR_LT(const bool set)
	{
		if(set)
		{
			CR |= CR0_LT;
		}
		else
		{
			CR &= ~CR0_LT;
		}
	}

	inline void UpdateCR_GT(const bool set)
	{
		if(set)
		{
			CR |= CR0_GT;
		}
		else
		{
			CR &= ~CR0_GT;
		}
	}

	inline void UpdateCR_EQ(const bool set)
	{
		if(set)
		{
			CR |= CR0_EQ;
		}
		else
		{
			CR &= ~CR0_EQ;
		}
	}

	inline void UpdateCR_SO(const bool set)
	{
		if(set)
		{
			CR |= CR0_SO;
		}
		else
		{
			CR &= ~CR0_SO;
		}
	}

	inline void UpdateCR0(const int value)
	{
		CR &= 0x0fffffff;
		if (!value)
		{
			UpdateCR_EQ(true);
		}
		else if (value & 0x80000000)
		{
			UpdateCR_LT(true);
		}
		else
		{
			UpdateCR_GT(true);
		}

		if (IsXER_SO()) UpdateCR_SO(true);
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
		UpdateXER_SO_OV(CheckOverflow(lvalue, value)
			|| (rvalue && IsCarryGen(lvalue + value, rvalue)));
	}

	const bool IsXER_CA() const { return (XER & XER_CA) > 0; }
	const bool IsXER_SO() const { return (XER & XER_SO) > 0; }

public:
	CPUThread(const u8 id);
	~CPUThread();

	virtual void Reset();
	virtual void NextPc();
	virtual void NextBranchPc();
	virtual void PrevPc();
	virtual void SetBranch(const u32 pc);
	virtual void SetPc(const u32 pc);

	virtual void SetError(const u32 error);

	virtual bool IsOk()		const { return m_error == 0; }
	virtual bool IsRunned()	const { return m_status == Runned; }
	virtual bool IsPaused()	const { return m_status == Paused; }
	virtual bool IsStoped()	const { return m_status == Stoped; }

	virtual u32  GetError() const { return m_error; }

	virtual void Run();
	virtual void Pause();
	virtual void Resume();
	virtual void Stop();
	virtual void SysResume();

private:
	enum Status
	{
		Runned,
		Paused,
		Stoped,
	};

	u32 m_status;
	u32 m_error;
	const u8 m_id;

	void* m_dec;

	virtual void Step();
};