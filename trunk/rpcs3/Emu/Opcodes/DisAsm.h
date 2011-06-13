#pragma once

#include "Emu/Opcodes/Opcodes.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

#include <wx/generic/progdlgg.h>

class DisAsmOpcodes : public Opcodes
{
	wxProgressDialog* m_prog_dial;
	DisAsmFrame* disasm_frame;
	wxFile op;

	bool m_dump_mode;

	virtual void Write(const wxString value)
	{
		if(m_dump_mode)
		{
			wxString mem = wxString::Format("%x		", CPU.PC);
			for(uint i=CPU.PC; i < CPU.PC + 4; ++i)
			{
				mem += wxString::Format("%02x", Memory.Read8(i));
				if(i < CPU.nPC - 1) mem += " ";
			}

			op.Write(mem + ": " + value + "\n");
		}
		else
		{
			disasm_frame->AddLine(value);
		}
	}

public:
	DisAsmOpcodes(bool DumpMode = false, const wxString output = wxEmptyString) : m_dump_mode(DumpMode)
	{
		if(m_dump_mode)
		{
			op.Open(output, op.write);
		}
		else
		{
			disasm_frame = new DisAsmFrame();
			disasm_frame->Show();
			op.Open("UnknownOpcodes.txt", op.write);
		}
	}

private:
	virtual void Exit()
	{
		if(!m_dump_mode)
		{
			disasm_frame->Close();
			delete disasm_frame;
		}
	}

	virtual void Step()
	{
	}

	virtual bool DoSysCall(const int WXUNUSED(code))
	{
		return false;
	}

	virtual void NOP()
	{
		Write( "NOP" );
	}

	virtual void UNK_SPECIAL(const int code, const int rs, const int rt, const int rd,
		const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		const wxString str = wxString::Format(
			"Unknown special opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, func, rs, rt, rd, sa, sa, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);

		if(!m_dump_mode) op.Write(str + "\n");
		Write(str);
	}

	virtual void SPECIAL2()
	{
		Write( "SPECIAL 2 (Unimplemented)" );
	}
	/*
	virtual void XOR(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format( "XOR r%d,r%d,r%d", rs, rt, rd ));
	}
	*/

	virtual void MULLI(const int rt, const int rs, const int imm_s16)
	{
		Write(wxString::Format( "MULLI r%d,r%d,%d", rt, rs, imm_s16 ));
	}

	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "SUBFIC r%d,r%d,%d", rs, rt, imm_s16 ));
	}

	virtual void CMPLWI(const int rs, const int rt, const int imm_u16)
	{
		if(rs != 0)
		{
			Write(wxString::Format( "CMPLWI cr%d,r%d,%d  #%x", rs/4, rt, imm_u16, imm_u16 ));
		}
		else
		{
			Write(wxString::Format( "CMPLWI r%d,%d  #%x", rt, imm_u16, imm_u16 ));
		}
	}

	virtual void CMPWI(const int rs, const int rt, const int rd)
	{
		if(rs != 0)
		{
			Write(wxString::Format( "CMPWI cr%d,r%d,r%d", rs/4, rt, rd ));
		}
		else
		{
			Write(wxString::Format( "CMPWI r%d,r%d", rt, rd ));
		}
	}

	virtual void ADDIC(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "ADDIC r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
	}

	virtual void ADDIC_(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "ADDIC. r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
	}
	
	virtual void G1()
	{
		Write( "G1 (Unimplemented)" );
	}

	virtual void ADDI(const int rt, const int rs, const int imm_u16)
	{
		Write(wxString::Format( "ADDI r%d,r%d,%d  #%x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void ADDIS(const int rt, const int rs, const int imm_s16)
	{
		Write(wxString::Format( "ADDI r%d,r%d,%d  #%x", rt, rs, imm_s16, imm_s16 ));
	}

	virtual void G2()
	{
		Write("G2 (Unimplemented)");
	}

	virtual void SC()
	{
		Write("SysCall");
	}

	virtual void G3()
	{
		Write("G3 (Unimplemented)");
	}

	virtual void BLR()
	{
		Write("BLR");
	}

	virtual void RLWINM()
	{
		Write("RLWINM (Unimplemented)");
	}

	virtual void ROTLW(const int rt, const int rs, const int rd)
	{
		Write(wxString::Format( "ROTLW r%d,r%d,r%d", rt, rs, rd ));
	}

	virtual void ORI(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void ORIS(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ORIS r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void XORI(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "XORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void XORIS(const int rt, const int rs, const int imm_u16)
	{
		if(rt == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "XORIS r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)
	{
		Write(wxString::Format( "CLRLDI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}

	virtual void MFLR(const int rs)
	{
		Write(wxString::Format( "MFLR r%d", rs ));
	}

	//g4 - 1f
	virtual void CMPW(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format("CMPW r%d,r%d,r%d", rs, rt, rd));
	}
	//g4_g1
	virtual void CMPLD(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format("CMPLD r%d,r%d,r%d", rs, rt, rd));
	}
	virtual void SUBF(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format("SUBF r%d,r%d,r%d", rs, rt, rd));
	}
	virtual void UNK_G4_G1(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		const wxString str = wxString::Format(
			"Unknown G4_G1 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);

		if(!m_dump_mode) op.Write(str + "\n");
		Write(str);
	}
	//
	virtual void ADD(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format("ADD r%d,r%d,r%d", rs, rt, rd));
	}
	virtual void XOR(const int rt, const int rs, const int rd)
	{
		Write(wxString::Format("XOR r%d,r%d,r%d", rt, rs, rd));
	}
	virtual void MR(const int rt, const int rs)
	{
		Write(wxString::Format("MR r%d,r%d", rt, rs));
	}
	//g4_g2
	virtual void MTLR(const int rs)
	{
		Write(wxString::Format("MTLR r%d", rs));
	}
	virtual void MTCTR(const int rs)
	{
		Write(wxString::Format("MTCTR r%d", rs));
	}
	virtual void UNK_G4_G2(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		const wxString str = wxString::Format(
			"Unknown G4_G2 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);

		if(!m_dump_mode) op.Write(str + "\n");
		Write(str);
	}
	//
	virtual void SRAWI(const int rt, const int rs, const int rd)
	{
		Write(wxString::Format("SRAWI r%d,r%d,%d", rt, rs, rd));
	}
	virtual void EXTSW(const int rt, const int rs)
	{
		Write(wxString::Format("EXTSW r%d,r%d", rt, rs));
	}
	virtual void LWZX(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format("LWZX r%d,r%d,r%d", rs, rt, rd));
	}
	virtual void UNK_G4(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		const wxString str = wxString::Format(
			"Unknown G4 opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);

		if(!m_dump_mode) op.Write(str + "\n");
		Write(str);
	}
	//

	virtual void LWZ(const int rt, const int rs, const int imm_s16)
	{
		Write(wxString::Format( "LWZ r%d,%d(r%d)  # %x", rt, imm_s16, rs, imm_s16 ));
	}

	virtual void LWZU(const int rt, const int rs, const int imm_u16)
	{
		Write(wxString::Format( "LWZU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LBZ(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LBZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LBZU(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LBZU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STW(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STWU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STB(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "STB r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STBU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "STBU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LHZ(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LHZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LHZU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "LHZU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STH(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "STH r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STHU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "STHU r%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LFS(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LFSU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "LFSU f%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void LFD(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STFS(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "STFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void STFSU(const int rs, const int rt, const int imm_u16)
	{
		Write(wxString::Format( "STFSU f%d,%d(r%d)  # %x", rs, imm_u16, rt, imm_u16 ));
	}

	virtual void STFD(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "STFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void LD(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "LD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void FDIVS(const int rs, const int rt, const int rd)
	{
		Write(wxString::Format( "FDIVS f%d, f%d, f%d", rs, rt, rd ));
	}

	virtual void STD(const int rs, const int rt, const int imm_s16)
	{
		Write(wxString::Format( "STD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}

	virtual void FCFID(const int rs, const int rd)
	{
		Write(wxString::Format( "FCFID f%d, f%d", rs, rd ));
	}

	virtual void G5()
	{
		Write( "G5 (Unimplemented)" );
	}

	virtual void UNK(const int code, const int opcode, const int rs, const int rt, const int rd,
		const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		const wxString str = wxString::Format(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);

		if(!m_dump_mode) op.Write(str + "\n");
		Write(str);
	}
};