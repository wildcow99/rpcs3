#pragma once

#include "Emu/Cell/SPUOpcodes.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(x)

class SPU_DisAsm : public SPU_Opcodes
{
	wxProgressDialog* m_prog_dial;
	DisAsmFrame* disasm_frame;

	const bool m_dump_mode;

	virtual void Write(const wxString value)
	{
		if(m_dump_mode)
		{
			wxString mem = wxString::Format("%x		", dump_pc);
			for(u8 i=0; i < 4; ++i)
			{
				mem += wxString::Format("%02x", Memory.Read8(dump_pc + i));
				if(i < 3) mem += " ";
			}

			last_opcode = mem + ": " + value + "\n";
		}
		else
		{
			disasm_frame->AddLine(value);
		}
	}

public:
	wxString last_opcode;
	uint dump_pc;
	SPUThread* CPU;

	SPU_DisAsm(SPUThread* cpu, bool DumpMode = false)
		: m_dump_mode(DumpMode)
		, CPU(cpu)
	{
		if(m_dump_mode) return;

		disasm_frame = new DisAsmFrame(*CPU);
		disasm_frame->Show();
	}

	~SPU_DisAsm()
	{
	}

private:
	virtual void Exit()
	{
		if(!m_dump_mode && !disasm_frame->exit)
		{
			disasm_frame->Close();
		}

		this->~SPU_DisAsm();
	}

	virtual void Step()
	{
	}

	u32 DisAsmBranchTarget(const s32 imm)
	{
		//if(m_dump_mode) return branchTarget(dump_pc, imm);
		//return branchTarget(GetPPU().PC, imm);
		return 0;
	}
	
	void DisAsm_F4_RC(const wxString& op, OP_REG f0, OP_REG f1, OP_REG f2, OP_REG f3, bool rc)
	{
		Write(wxString::Format("%s%s f%d,f%d,f%d,f%d", op, rc ? "." : "", f0, f1, f2, f3));
	}
	void DisAsm_F3_RC(const wxString& op, OP_REG f0, OP_REG f1, OP_REG f2, bool rc)
	{
		Write(wxString::Format("%s%s f%d,f%d,f%d", op, rc ? "." : "", f0, f1, f2));
	}
	void DisAsm_F3(const wxString& op, OP_REG f0, OP_REG f1, OP_REG f2)
	{
		DisAsm_F3_RC(op, f0, f1, f2, false);
	}
	void DisAsm_F2_RC(const wxString& op, OP_REG f0, OP_REG f1, bool rc)
	{
		Write(wxString::Format("%s%s f%d,f%d", op, rc ? "." : "", f0, f1));
	}
	void DisAsm_F2(const wxString& op, OP_REG f0, OP_REG f1)
	{
		DisAsm_F2_RC(op, f0, f1, false);
	}
	void DisAsm_F1_RC(const wxString& op, OP_REG f0, bool rc)
	{
		Write(wxString::Format("%s%s f%d", op, rc ? "." : "", f0));
	}
	void DisAsm_R1(const wxString& op, OP_REG r0)
	{
		Write(wxString::Format("%s r%d", op, r0));
	}
	void DisAsm_R2_RC(const wxString& op, OP_REG r0, OP_REG r1, bool rc)
	{
		Write(wxString::Format("%s%s r%d,r%d", op, rc ? "." : "", r0, r1));
	}
	void DisAsm_R2(const wxString& op, OP_REG r0, OP_REG r1)
	{
		DisAsm_R2_RC(op, r0, r1, false);
	}
	void DisAsm_R3_RC(const wxString& op, OP_REG r0, OP_REG r1, OP_REG r2, bool rc)
	{
		Write(wxString::Format("%s%s r%d,r%d,r%d", op, rc ? "." : "", r0, r1, r2));
	}
	void DisAsm_R3(const wxString& op, OP_REG r0, OP_REG r1, OP_REG r2)
	{
		DisAsm_R3_RC(op, r0, r1, r2, false);
	}
	void DisAsm_R2_IMM(const wxString& op, OP_REG r0, OP_REG r1, OP_sIMM imm0)
	{
		Write(wxString::Format("%s r%d,r%d,%d  #%x", op, r0, r1, imm0, imm0));
	}
	void DisAsm_R1_IMM(const wxString& op, OP_REG r0, OP_sIMM imm0)
	{
		Write(wxString::Format("%s r%d,%d  #%x", op, r0, imm0, imm0));
	}
	void DisAsm_CR_R_IMM(const wxString& op, OP_REG cr0, OP_REG r0, OP_sIMM imm0)
	{
		Write(wxString::Format("%s cr%d,r%d,%d  #%x", op, cr0, r0, imm0, imm0));
	}
	void DisAsm_CR1_R2_RC(const wxString& op, OP_REG cr0, OP_REG r0, OP_REG r1, bool rc)
	{
		Write(wxString::Format("%s%s cr%d,r%d,r%d", op, rc ? "." : "", cr0, r0, r1 ));
	}
	void DisAsm_CR1_R2(const wxString& op, OP_REG cr0, OP_REG r0, OP_REG r1)
	{
		DisAsm_CR1_R2_RC(op, cr0, r0, r1, false);
	}
	void DisAsm_INT3(const wxString& op, const int i0, const int i1, const int i2)
	{
		Write(wxString::Format("%s %d,%d,%d", op, i0, i1, i2 ));
	}
	void DisAsm_BRANCH(const wxString& op, const int pc)
	{
		Write(wxString::Format("%s 0x%x", op, DisAsmBranchTarget(pc)));
	}
	void DisAsm_B2_BRANCH(const wxString& op, OP_REG b0, OP_REG b1, const int pc)
	{
		Write(wxString::Format("%s %d,%d,0x%x ", op, b0, b1, DisAsmBranchTarget(pc)));
	}
	
private:
	virtual void NOP()
	{
		Write( "nop" );
	}
	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM simm16)
	{
		if(ra == 0)
		{
			DisAsm_R1_IMM("li", rt, simm16);
		}
		else
		{
			DisAsm_R2_IMM("ai", rt, ra, simm16);
		}
	}
	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		Write(wxString::Format("Unknown/Illegal opcode! (0x%08x : 0x%x : 0x%x)", code, opcode, gcode));
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP