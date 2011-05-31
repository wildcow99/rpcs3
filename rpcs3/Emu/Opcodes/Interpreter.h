#include "Emu/Opcodes/Opcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

#include <wx/generic/progdlgg.h>

class InterpreterOpcodes : public Opcodes
{
private:
	Display* display;

public:
	InterpreterOpcodes()
	{
		display = new Display();
		display->Show();
	}

private:
	virtual void SPECIAL()
	{
	}

	virtual void SPECIAL2()
	{
	}

	virtual void VXOR(const int rs, const int rt, const int rd)
	{
	}

	virtual void MULLI(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void CMPLWI(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void CMPWI(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void ADDIC(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void ADDIC_(const int rs, const int rt, const int imm_s16)
	{
	}

	//g1 - 0e
	virtual void G1()
	{
	}
	//
	//virtual void ADDI(const int rt, const int rs, const int imm_u16)=0;
	//
	virtual void ADDIS(const int rt, const int rs, const int imm_s16)
	{
	}
	//g2 - 10
	virtual void G2()
	{
	}

	//
	virtual void SC()
	{
	}

	//g3 - 12
	virtual void G3()
	{
	}
	//
	virtual void BLR()
	{
	}

	virtual void RLWINM()
	{
	}

	virtual void ROTLW(const int rt, const int rs, const int rd)
	{
	}

	virtual void ORI(const int rt, const int rs, const int imm_u16)
	{
	}

	virtual void ORIS(const int rt, const int rs, const int imm_s16)
	{
	}

	virtual void XORI(const int rt, const int rs, const int imm_u16)
	{
	}

	virtual void XORIS(const int rt, const int rs, const int imm_s16)
	{
	}

	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)
	{
	}

	//g4 - 1f
	virtual void G4()
	{
	}

	//virtual void MFLR(const int rs)=0;
	//
	virtual void LWZ(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void LWZU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void LBZ(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void LBZU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void STW(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STWU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void STB(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STBU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void LHZ(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void LHZU(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STH(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STHU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void LFS(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void LFSU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void LFD(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STFS(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void STFSU(const int rs, const int rt, const int imm_u16)
	{
	}

	virtual void STFD(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void LD(const int rs, const int rt, const int imm_s16)
	{
	}

	virtual void FDIVS(const int rs, const int rt, const int rd)
	{
	}

	virtual void STD(const int rs, const int rt, const int imm_s16)
	{
	}

	//g5 - 3f
	virtual void G5()
	{
	}
	//virtual void FCFID(const int rs, const int rd)=0;
	//
	
	virtual void UNK(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error(wxString::Format(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm s26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		));
	}
};