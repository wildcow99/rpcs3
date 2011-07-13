#pragma once

enum { XER_SO, XER_OV, XER_CA };
enum { CR0_LT, CR0_GT, CR0_EQ, CR0_SO }; 

class CPUCycle
{
public:
	uint PC;
	uint nPC;

	float FPR[32];
	s64 GPR[32]; //General-Purpose Register
	bool CR[8][4]; //Condition Register

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

	bool XER[32]; //SPR 0x001 : Fixed-Point Expection Register
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

	s32 LR[32];	//SPR 0x008 : Link Register
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

	u64  cycle;

	CPUCycle()
	{
		Reset();
	}

	virtual void UpdateCRn(const uint n, const int lvalue, const int rvalue)
	{
		CR[n][CR0_LT] = lvalue < rvalue;
		CR[n][CR0_GT] = lvalue > rvalue;
		CR[n][CR0_EQ] = lvalue == rvalue;
		CR[n][CR0_SO] = XER[XER_SO];
	}

	virtual void UpdateCR0(const int value)
	{
		UpdateCRn(0, value, 0);
	}

	virtual void Reset();
	virtual void NextPc();
	virtual void NextBranchPc();
	virtual void PrevPc();
	virtual void SetPc(const uint _pc);
};

extern CPUCycle CPU;