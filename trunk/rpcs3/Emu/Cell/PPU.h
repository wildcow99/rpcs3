#pragma once
#include "Emu/Cell/CPU.h"

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

union CRhdr
{
	u32 CR;

	struct
	{
		u8 cr0 : 4;
		u8 cr1 : 4;
		u8 cr2 : 4;
		u8 cr3 : 4;
		u8 cr4 : 4;
		u8 cr5 : 4;
		u8 cr6 : 4;
		u8 cr7 : 4;
	};
};

static const s32 MAX_INT_VALUE = 0x7fffffff;

class PPUThread : public CPUThread
{
public:
	double FPR[32]; //Floating Point Register
	u32 FPSCR; //Floating Point Status and Control Register
	u64 GPR[32]; //General-Purpose Register

	CRhdr CR;
	//u32 CR; //Condition Register
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

	u64 XER; //SPR 0x001 : Fixed-Point Expection Register
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

	inline void UpdateCR(const u8 n, const u32 value, const bool set)
	{
		switch(n)
		{
		case 0: set ? CR.cr0 |= value : CR.cr0 &= ~value; break;
		case 1: set ? CR.cr1 |= value : CR.cr1 &= ~value; break;
		case 2: set ? CR.cr2 |= value : CR.cr2 &= ~value; break;
		case 3: set ? CR.cr3 |= value : CR.cr3 &= ~value; break;
		case 4: set ? CR.cr4 |= value : CR.cr4 &= ~value; break;
		case 5: set ? CR.cr5 |= value : CR.cr5 &= ~value; break;
		case 6: set ? CR.cr6 |= value : CR.cr6 &= ~value; break;
		case 7: set ? CR.cr7 |= value : CR.cr7 &= ~value; break;
		}
	}

	inline void SetCR(const u8 n, const u32 value)
	{
		switch(n)
		{
		case 0: CR.cr0 = value; break;
		case 1: CR.cr1 = value; break;
		case 2: CR.cr2 = value; break;
		case 3: CR.cr3 = value; break;
		case 4: CR.cr4 = value; break;
		case 5: CR.cr5 = value; break;
		case 6: CR.cr6 = value; break;
		case 7: CR.cr7 = value; break;
		}
	}

	inline void UpdateCR_LT(const u8 n, const bool set)
	{
		UpdateCR(n, CR_LT, set);
	}

	inline void UpdateCR_GT(const u8 n, const bool set)
	{
		UpdateCR(n, CR_GT, set);
	}

	inline void UpdateCR_EQ(const u8 n, const bool set)
	{
		UpdateCR(n, CR_EQ, set);
	}

	inline void UpdateCR_SO(const u8 n, const bool set)
	{
		UpdateCR(n, CR_SO, set);
	}

	inline u8 GetCR(const u8 n) const
	{
		switch(n)
		{
		case 0: return CR.cr0;
		case 1: return CR.cr1;
		case 2: return CR.cr2;
		case 3: return CR.cr3;
		case 4: return CR.cr4;
		case 5: return CR.cr5;
		case 6: return CR.cr6;
		case 7: return CR.cr7;
		}

		return 0;
	}

	inline bool IsCR_EQ(const u8 n) const
	{
		return (GetCR(n) & CR_EQ) > 0;
	}

	inline bool IsCR_GT(const u8 n) const
	{
		return (GetCR(n) & CR_GT) > 0;
	}

	inline bool IsCR_LT(const u8 n) const
	{
		return (GetCR(n) & CR_LT) > 0;
	}

	void UpdateCRn(const u8 n, const s32 a, const s32 b)
	{
		if		(a <  b) SetCR(n, CR_LT);
		else if	(a >  b) SetCR(n, CR_GT);
		else if	(a == b) SetCR(n, CR_EQ);

		UpdateCR_SO(n, IsXER_SO());
	}

	void UpdateCR0(const s32 val)
	{
		UpdateCRn(0, val, 0);
	}

	void UpdateCR1(const double val)
	{
		UpdateCRn(1, (val > 0.0f ? 1 : (val < 0.0f ? -1 : 0)), 0);
	}

	const u8 GetCR_Bit(const u8 bit) const { return 1 << (3 - (bit % 4)); }

	void UpdateCR(const u8 bit, bool set)
	{
		UpdateCR(bit, GetCR_Bit(bit), set);
	}

	const u8 IsCR(const u8 bit) const
	{
		return (GetCR(bit/4) & GetCR_Bit(bit)) ? 1 : 0;
	}

	inline void UpdateXER_CA(const bool set)
	{
		set
			? XER |=  XER_CA
			: XER &= ~XER_CA;
	}

	inline void UpdateXER_OV(const bool set)
	{
		set
			? XER |=  XER_OV
			: XER &= ~XER_OV;
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

	const bool IsADD_CA(const int r, const int a, const int b) const
	{
		return (u32)r < (u32)a;
	}
	const bool IsSUB_CA(const int r, const int a, const int b) const
	{
		return (u32)a >= (u32)b;
	}
	const bool IsADD_OV(const int r, const int a, const int b) const
	{
		return ((~(a ^ b) & (a ^ r)) & 0x80000000) > 0;
	}
	const bool IsSUB_OV(const int r, const int a, const int b) const
	{
		return (((a ^ b) & (a ^ r)) & 0x80000000) > 0;
	}

public:
	PPUThread(const u8 core);
	~PPUThread();

	virtual wxString RegsToString()
	{
		wxString ret = CPUThread::RegsToString();
		for(uint i=0; i<32; ++i) ret += wxString::Format("GPR[%d] = 0x%x\n", i, GPR[i]);
		ret += wxString::Format("CR = 0x%08x\n", CR);
		ret += wxString::Format("LR = 0x%x\n", LR);
		ret += wxString::Format("CTR = 0x%x\n", CTR);
		ret += wxString::Format("XER = 0x%x\n", XER);
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
	virtual void DoSysResume();

private:
	virtual void DoCode(const s32 code);
};