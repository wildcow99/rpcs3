#pragma once

#include "Emu/Opcodes/Opcodes.h"
#include "Gui/DisAsmFrame.h"

class DisAsmOpcodes : public Opcodes
{
	DisAsmFrame* disasm_frame;
	wxFile op;

public:
	DisAsmOpcodes()
	{
		disasm_frame = new DisAsmFrame();
		disasm_frame->Show();
		op.Open("UnknownOpcodes.txt", op.write);
	}

	virtual void NOP()
	{
		disasm_frame->AddLine("NOP");
	}

	virtual void SPECIAL()
	{
		disasm_frame->AddLine(wxString::Format( "SPECIAL (Unimplemented)" ));
	}

	virtual void SPECIAL2()
	{
		disasm_frame->AddLine(wxString::Format( "SPECIAL 2 (Unimplemented)" ));
	}

	virtual void VXOR(const int rs, const int rt, const int rd)
	{
		disasm_frame->AddLine(wxString::Format( "VXOR v%d,v%d,v%d", rs, rt, rd ));
	}

	virtual void MULLI(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "MULLI r%d,r%d,%d", rs, rt, imm_s16 ));
	}

	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "SUBFIC r%d,r%d,%d", rs, rt, imm_s16 ));
	}

	virtual void CMPLWI(const int rs, const int rt, const int sa)
	{
		if(rs != 0)
		{
			disasm_frame->AddLine(wxString::Format( "CMPLWI cr%d,r%d,%d", rs/4, rt, sa ));
		}
		else
		{
			disasm_frame->AddLine(wxString::Format( "CMPLWI r%d,%d", rt, sa ));
		}
	}

	virtual void CMPWI(const int rs, const int rt, const int imm_u16)
	{
		if(rs != 0)
		{
			disasm_frame->AddLine(wxString::Format( "CMPWI cr%d,r%d,%d  #%x", rs/4, rt, imm_u16, imm_u16 ));
		}
		else
		{
			disasm_frame->AddLine(wxString::Format( "CMPWI r%d,%d  #%x", rt, imm_u16, imm_u16 ));
		}
	}
	
	virtual void G1()
	{
		disasm_frame->AddLine(wxString::Format( "G1 (Unimplemented)" ));
	}

	virtual void ADDI(const int rt, const int rs, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "ADDI r%d,r%d,%d  #%x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void ADDIS(const int rt, const int rs, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "ADDI r%d,r%d,%d  #%x", rt, rs, imm_s16, imm_s16 ));
	}

	virtual void G2()
	{
		disasm_frame->AddLine(wxString::Format( "G2 (Unimplemented)" ));
	}

	virtual void SC()
	{
		disasm_frame->AddLine(wxString::Format( "SC (???)" ));
	}

	virtual void G3()
	{
		disasm_frame->AddLine(wxString::Format( "G3 (Unimplemented)" ));
	}

	virtual void BLR()
	{
		disasm_frame->AddLine(wxString::Format( "BLR" ));
	}

	virtual void RLWINM()
	{
		disasm_frame->AddLine(wxString::Format( "RLWINM (Unimplemented)" ));
	}

	virtual void ORI(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}
		disasm_frame->AddLine(wxString::Format( "ORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void ORIS(const int rt, const int rs, const int imm_s16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}
		disasm_frame->AddLine(wxString::Format( "ORIS r%d,r%d,%d  # %x", rt, rs, imm_s16, imm_s16 ));
	}

	virtual void XORI(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}
		disasm_frame->AddLine(wxString::Format( "XORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void XORIS(const int rt, const int rs, const int imm_s16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}
		disasm_frame->AddLine(wxString::Format( "XORIS r%d,r%d,%d  # %x", rt, rs, imm_s16, imm_s16 ));
	}

	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "CLRLDI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void MFLR(const int rs)
	{
		disasm_frame->AddLine(wxString::Format( "MFLR r%d", rs ));
	}

	virtual void G4()
	{
		disasm_frame->AddLine(wxString::Format( "G4 (Unimplemented)" ));
	}

	virtual void LWZ(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LWZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LWZU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "LWZU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LBZ(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LBZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LBZU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "LBZU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STW(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STWU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STB(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "STB r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STBU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "STBU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LHZ(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LHZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STH(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "STH r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STHU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "STHU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LFS(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LFSU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "LFSU f%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LFD(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STFS(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "STFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STFSU(const int rs, const int rt, const int imm_u16)
	{
		disasm_frame->AddLine(wxString::Format( "STFSU f%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STFD(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "STFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LD(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "LD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void FDIVS(const int rs, const int rt, const int rd)
	{
		disasm_frame->AddLine(wxString::Format( "FDIVS f%d, f%d, f%d", rs, rt, rd ));
	}

	virtual void STD(const int rs, const int rt, const int imm_s16)
	{
		disasm_frame->AddLine(wxString::Format( "STD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void FCFID(const int rs, const int rd)
	{
		disasm_frame->AddLine(wxString::Format( "FCFID f%d, f%d", rs, rd ));
	}

	virtual void G5()
	{
		disasm_frame->AddLine(wxString::Format( "G5 (Unimplemented)" ));
	}

	virtual void UNK(const int code, const int opcode, const int rs, const int rt, const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		wxString str = wxString::Format(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm s26: 0x%x : %d",
			code, opcode, rs, rt, sa, sa, func, func, rd, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
		
		if(code == 0)
		{
			NOP();
			return;
		}

		op.Write(str + "\n");
		disasm_frame->AddLine(str);
	}
};