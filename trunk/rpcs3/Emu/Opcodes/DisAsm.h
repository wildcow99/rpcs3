#pragma once

#include "Emu/Opcodes/Opcodes.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

#include <wx/generic/progdlgg.h>

#define START_OPCODES_GROUP(x)
#define END_OPCODES_GROUP(name, name_string) \
	virtual void UNK_##name##(\
		const int code, const int opcode, OP_REG rs, OP_REG rt, OP_REG rd, OP_REG sa, const u32 func,\
		OP_REG crfs, OP_REG crft, OP_REG crm, OP_REG bd, OP_REG lk, OP_REG ms, OP_REG mt, OP_REG mfm,\
		OP_REG ls, const s32 imm_m, const s32 imm_s16, const u32 imm_u16, const u32 imm_u26) \
	{\
		Write(wxString::Format( \
			"Unknown "##name_string##" opcode! - (%08x - %02x) - " \
			"rs: r%d, rt: r%d, rd: r%d, " \
			"sa: 0x%x : %d, func: 0x%x : %d, " \
			"crfs: 0x%x : %d, crft: 0x%x : %d, " \
			"crm: 0x%x : %d, bd: 0x%x : %d, " \
			"lk: 0x%x : %d, ms: 0x%x : %d, " \
			"mt: 0x%x : %d, mfm: 0x%x : %d, " \
			"ls: 0x%x : %d, " \
			"imm m: 0x%x : %d, imm s16: 0x%x : %d, " \
			"imm u16: 0x%x : %d, imm u26: 0x%x : %d, " \
			"Branch: 0x%x, Jump: 0x%x", \
			code, opcode, \
			rs, rt, rd, \
			sa, sa, func, func, \
			crfs, crfs, crft, crft, \
			crm, crm, bd, bd, \
			lk, lk, ms, ms, \
			mt, mt, mfm, mfm, \
			ls, ls, \
			imm_m, imm_m, imm_s16, imm_s16, \
			imm_u16, imm_u16, imm_u26, imm_u26, \
			(m_dump_mode ? branchTarget(dump_pc, imm_u26) : branchTarget(imm_u26)), \
			(m_dump_mode ? jumpTarget  (dump_pc, imm_u26) : jumpTarget  (imm_u26)) \
		)); \
	}

class DisAsmOpcodes : public Opcodes
{
	wxProgressDialog* m_prog_dial;
	DisAsmFrame* disasm_frame;

	bool m_dump_mode;

	virtual void Write(const wxString value)
	{
		if(m_dump_mode)
		{
			wxString mem = wxString::Format("%x		", dump_pc);
			for(uint i=dump_pc; i < dump_pc + 4; ++i)
			{
				mem += wxString::Format("%02x", Memory.Read8(i));
				if(i < dump_pc + 3) mem += " ";
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

	DisAsmOpcodes(bool DumpMode = false) : m_dump_mode(DumpMode)
	{
		if(m_dump_mode) return;

		disasm_frame = new DisAsmFrame();
		disasm_frame->Show();
	}

private:
	virtual void Exit()
	{
		if(!m_dump_mode && !disasm_frame->exit)
		{
			disasm_frame->Close();
			safe_delete(disasm_frame);
		}
	}

	virtual void Step()
	{
	}

	virtual void NOP()
	{
		Write( "NOP" );
	}

	START_OPCODES_GROUP(SPECIAL)
	END_OPCODES_GROUP(SPECIAL, "special");

	START_OPCODES_GROUP(SPECIAL2)
	END_OPCODES_GROUP(SPECIAL2, "special 2");
		/*
		virtual void XOR(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format( "XOR r%d,r%d,r%d", rs, rt, rd ));
		}
		*/
		virtual void MULLI(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
		{
			Write(wxString::Format( "MULLI r%d,r%d,%d", rt, rs, imm_s16 ));
		}
		virtual void SUBFIC(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "SUBFIC r%d,r%d,%d", rs, rt, imm_s16 ));
		}
		virtual void CMPLDI(OP_REG rs, OP_REG rt, OP_REG imm_u16)
		{
			if(rs != 0)
			{
				Write(wxString::Format( "CMPLDI cr%d,r%d,%d  #%x", rs/4, rt, imm_u16, imm_u16 ));
			}
			else
			{
				Write(wxString::Format( "CMPLDI r%d,%d  #%x", rt, imm_u16, imm_u16 ));
			}
		}
		virtual void CMPDI(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format( "CMPDI r%d,r%d,r%d", rs, rt, rd ));
		}
		virtual void ADDIC(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "ADDIC r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
		}
		virtual void ADDIC_(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "ADDIC. r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
		}
	START_OPCODES_GROUP(G_0x04)
		START_OPCODES_GROUP(G_0x04_0x0)
			virtual void MULHHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULHHWU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MULHHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULHHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x0, "G_0x04_0x0");
		START_OPCODES_GROUP(G_0x04_0x1)
			virtual void MULHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULHHW r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MULHHW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULHHW. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHW r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACHHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACHHW r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x1, "G_0x04_0x1");
		START_OPCODES_GROUP(G_0x04_0x2)
			virtual void MACHHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x2, "G_0x04_0x2");
		START_OPCODES_GROUP(G_0x04_0x3)
			virtual void MACHHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWS r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWS_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWS. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACHHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACHHWS r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x3, "G_0x04_0x3");

		START_OPCODES_GROUP(G_0x04_0x4)
			virtual void MULCHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULCHWU r%d,r%d,r%d", rs, rt, rd ));
			}
				virtual void MULCHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULCHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x4, "G_0x04_0x4");

		START_OPCODES_GROUP(G_0x04_0x5)
			virtual void MULCHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULCHW r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHW r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x5, "G_0x04_0x5");

		START_OPCODES_GROUP(G_0x04_0x6)
			virtual void MACCHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWSU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWSU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x6, "G_0x04_0x6");

		START_OPCODES_GROUP(G_0x04_0x7)
			virtual void MACCHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWS r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACCHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACCHWS r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACCHWS_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACCHWS. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x7, "G_0x04_0x7");

		START_OPCODES_GROUP(G_0x04_0xc)
			virtual void MULLHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULLHWU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MULLHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MULLHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACLHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACLHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0xc, "G_0x04_0xc");

		START_OPCODES_GROUP(G_0x04_0xd)
			virtual void NMACLHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACLHW r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0xd, "G_0x04_0xd");

		START_OPCODES_GROUP(G_0x04_0xe)
			virtual void MACLHWSU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWSU r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACLHWSU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWSU. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0xe, "G_0x04_0xe");

		START_OPCODES_GROUP(G_0x04_0xf)
			virtual void MACLHWS(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWS r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0xf, "G_0x04_0xf");

		START_OPCODES_GROUP(G_0x04_0x10)
			virtual void MACHHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x10, "G_0x04_0x10");

		START_OPCODES_GROUP(G_0x04_0x12)
			virtual void MACHHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x12, "G_0x04_0x12");

		START_OPCODES_GROUP(G_0x04_0x13)
			virtual void MACHHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACHHWSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACHHWSO. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACHHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACHHWSO r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x13, "G_0x04_0x13");

		START_OPCODES_GROUP(G_0x04_0x14)
			virtual void MACCHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x14, "G_0x04_0x14");

		START_OPCODES_GROUP(G_0x04_0x15)
			virtual void MACCHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWO. r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACCHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACCHWO r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x15, "G_0x04_0x15");
		
		START_OPCODES_GROUP(G_0x04_0x16)
			virtual void MACCHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWSUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACCHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACCHWSUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x16, "G_0x04_0x16");
		
		START_OPCODES_GROUP(G_0x04_0x17)
			virtual void NMACCHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACCHWSO r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x17, "G_0x04_0x17");
		
		START_OPCODES_GROUP(G_0x04_0x18)
			virtual void MFVSCR(OP_REG rs)
			{
				Write(wxString::Format( "MFVSCR r%d", rs ));
			}
		END_OPCODES_GROUP(G_0x04_0x18, "G_0x04_0x18");

		START_OPCODES_GROUP(G_0x04_0x1c)
			virtual void MACLHWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACLHWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x1c, "G_0x04_0x1c");

		START_OPCODES_GROUP(G_0x04_0x1d)
			virtual void NMACLHWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACLHWO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACLHWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACLHWO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x1d, "G_0x04_0x1d");

		START_OPCODES_GROUP(G_0x04_0x1e)
			virtual void MACLHWSUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWSUO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void MACLHWSUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "MACLHWSUO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x1e, "G_0x04_0x1e");

		START_OPCODES_GROUP(G_0x04_0x1f)
			virtual void NMACLHWSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACLHWSO r%d,r%d,r%d", rs, rt, rd ));
			}
			virtual void NMACLHWSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format( "NMACLHWSO. r%d,r%d,r%d", rs, rt, rd ));
			}
		END_OPCODES_GROUP(G_0x04_0x1f, "G_0x04_0x1f");
	END_OPCODES_GROUP(G_0x04, "G_0x04");

	START_OPCODES_GROUP(G1)
		virtual void LI(OP_REG rs, OP_sREG imm_s16)
		{
			Write(wxString::Format( "LI r%d,%d  #%x", rs, imm_s16, imm_s16 ));
		}
		virtual void ADDI(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			if(rt == 0)
			{
				Write(wxString::Format( "ADDI r%d,%d  #%x", rs, imm_s16, imm_s16 ));
			}
			else
			{
				Write(wxString::Format( "ADDI r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
			}
		}
	END_OPCODES_GROUP(G1, "G1");

	START_OPCODES_GROUP(G_0x0f)
		START_OPCODES_GROUP(G_0x0f_0x0)
		virtual void ADDIS(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
		{
			if(rt == 0)
			{
				Write(wxString::Format( "ADDIS r%d,%d  #%x", rs, imm_s16, imm_s16 ));
			}
			else
			{
				Write(wxString::Format( "ADDIS r%d,r%d,%d  #%x", rs, rt, imm_s16, imm_s16 ));
			}
		}
		virtual void LIS(OP_REG rs, OP_sREG imm_s16)
		{
			Write(wxString::Format( "LIS r%d,%d", rs, imm_s16 ));
		}
		END_OPCODES_GROUP(G_0x0f_0x0, "G_0x0f_0x0");
	END_OPCODES_GROUP(G_0x0f, "G_0x0f");
	START_OPCODES_GROUP(G2)
	END_OPCODES_GROUP(G2, "G2");

	virtual void SC(const u32 sys)
	{
		Write(wxString::Format("SysCall %d", sys));
	}

	START_OPCODES_GROUP(BRANCH)
		virtual void B(const u32 imm_u26)
		{
			Write(wxString::Format
				("B 0x%x", m_dump_mode ? branchTarget(dump_pc, imm_u26) : branchTarget(imm_u26))
			);
		}
		virtual void BL(const u32 imm_u26)
		{
			Write(wxString::Format
				("BL 0x%x", m_dump_mode ? branchTarget(dump_pc, imm_u26) : branchTarget(imm_u26))
			);
		}
	END_OPCODES_GROUP(BRANCH, "BRANCH");

	START_OPCODES_GROUP(G3_S)
		/*
		virtual void BLR()
		{
			Write("BLR");
		}
		*/
	END_OPCODES_GROUP(G3_S, "G3_S");

	START_OPCODES_GROUP(G3_S0)
	virtual void RLWINM_()
	{
		Write("RLWINM. (Unimplemented)");
	}
	virtual void CLRLWI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "CLRLWI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void CLRLWI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "CLRLWI. r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	END_OPCODES_GROUP(G3_S0, "G3_S0");

	START_OPCODES_GROUP(G3_S0_G0)
		virtual void RLWINM()
		{
			Write("RLWINM (Unimplemented)");
		}
		virtual void ROTLWI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			if(rt == 0 && rs == 0)
			{
				NOP();
				return;
			}

			Write(wxString::Format( "ROTLWI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
		}
		virtual void ROTLWI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
		{
			if(rt == 0 && rs == 0)
			{
				NOP();
				return;
			}

			Write(wxString::Format( "ROTLWI. r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
		}
	END_OPCODES_GROUP(G3_S0_G0, "G3_S0_G0");
	//virtual void RLWINM()
	//{
	//	Write("RLWINM (Unimplemented)");
	//}
	virtual void ROTLW(OP_REG rt, OP_REG rs, OP_REG rd)
	{
		Write(wxString::Format( "ROTLW r%d,r%d,r%d", rt, rs, rd ));
	}
	virtual void ORI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void ORIS(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ORIS r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void XORI(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "XORI r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void XORIS(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "XORIS r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void ANDI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ANDI. r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	virtual void ANDIS_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	{
		if(rt == 0 && rs == 0)
		{
			NOP();
			return;
		}

		Write(wxString::Format( "ANDIS. r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	}
	//virtual void LRLDIÑ(OP_REG rt, OP_REG rs, OP_REG imm_u16)
	//{
	//	Write(wxString::Format( "LRLDIÑ r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
	//}
	START_OPCODES_GROUP(G_0x1e)
		START_OPCODES_GROUP(G_0x1e_G_0x14)
			virtual void CLRLDI_(OP_REG rt, OP_REG rs, OP_REG imm_u16)
			{
				if(rt == 0 && rs == 0)
				{
					NOP();
					return;
				}

				Write(wxString::Format( "CLRLDI. r%d,r%d,%d  # %x", rt, rs, imm_u16, imm_u16 ));
			}
		END_OPCODES_GROUP(G_0x1e_G_0x14, "G_0x1e_G_0x14");
	END_OPCODES_GROUP(G_0x1e, "G_0x1e");
	START_OPCODES_GROUP(G4)
		START_OPCODES_GROUP(G4_G0)
			//virtual void CMPW(OP_REG rs, OP_REG rt, OP_REG rd)
			//{
			//	Write(wxString::Format("CMPW r%d,r%d,r%d", rs, rt, rd));
			//}
			virtual void LVSL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVSL r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LVEBX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: LVEBX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHDU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHDU r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHDU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHDU. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDC(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDC r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHWU r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHWU. r%d,r%d,r%d", rs, rt, rd));
			}
			//virtual void MFCR(OP_REG rs)
			//{
			//	Write(wxString::Format("MFCR r%d", rs));
			//}
			START_OPCODES_GROUP(G4_G0_0x26)
				virtual void MFCR(OP_REG rs)
				{
					Write(wxString::Format("MFCR r%d", rs));
				}
				virtual void MFOCRF(OP_REG rs, OP_sREG imm_s16)
				{
					Write(wxString::Format( "MFOCRF r%d,%d", rs, imm_s16 ));
				}
			END_OPCODES_GROUP(G4_G0_0x26, "G4_G0_0x26");
			virtual void LWARX(OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				Write(wxString::Format( "LWARX r%d,r%d,%d", rs, rd, imm_s16 ));
			}
			virtual void LDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LDX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void CNTLZW(OP_REG rt, OP_REG rs)
			{
				Write(wxString::Format("CNTLZW r%d,r%d", rt, rs));
			}
			virtual void AND(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("AND r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void AND_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("AND. r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void MASKG(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("MASKG r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G0, "G4_G0");
		START_OPCODES_GROUP(G4_G1)
			virtual void CMPLD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("CMPLD r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LVSR(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVSR r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LVEHX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVEHX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SUBF(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("SUBF r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SUBF_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("SUBF. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LDUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LDUX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LWZUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LWZUX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void CNTLZD(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("CNTLZD r%d,r%d", rs, rt));
			}
			virtual void ANDC(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("ANDC r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G1, "G4_G1");
		//virtual void MULHW(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	Write(wxString::Format("MULHW r%d,r%d,r%d", rs, rt, rd));
		//}
		START_OPCODES_GROUP(G4_G_0x2)
			virtual void LVEWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVEWX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHD r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHD. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHW r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULHW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULHW. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DLMZB(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("DLMZB r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void DLMZB_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("DLMZB. r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void LDARX(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("LDARX r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void DCBF(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("DCBF r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void LBZX(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("LBZX r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G_0x2, "G4_G_0x2");
		START_OPCODES_GROUP(G4_G2)
			virtual void LVX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void NEG(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("NEG r%d,r%d", rs, rt));
			}
			virtual void MUL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MUL r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MUL_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MUL. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MTSRDIN(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("MTSRDIN r%d,r%d", rs, rd));
			}
			//virtual void NOT(OP_REG rt, OP_REG rd)
			//{
			//	Write(wxString::Format("NOT r%d,r%d", rt, rd));
			//}
		END_OPCODES_GROUP(G4_G2, "G4_G2");
		START_OPCODES_GROUP(G4_G3)
			virtual void WRTEE(OP_REG rs)
			{
				Write(wxString::Format("WRTEE r%d", rs));
			}
			virtual void STVEBX(OP_REG rs, OP_REG rt, OP_REG rd) //FIXME
			{
				Write(wxString::Format("STVEBX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SUBFE(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("SUBFE r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SUBFE_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("SUBFE. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDE(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDE r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDE_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDE. r%d,r%d,r%d", rs, rt, rd));
			}
			//virtual void MTOCRF(OP_REG rs) //FIXME
			//{
			//	Write(wxString::Format("Unimplemented: MTOCRF r%d", rs));
			//}
			virtual void STDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STDX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void STWCX_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STWCX. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void STWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STWX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SLQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SLQ r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void SLQ_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SLQ. r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void SLE_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SLE. r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G3, "G4_G3");
		virtual void STDUX(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("STDUX r%d,r%d,r%d", rs, rt, rd));
		}
		//virtual void ADDZE(OP_REG rs, OP_REG rt)
		//{
		//	Write(wxString::Format("ADDZE r%d,r%d", rs, rt));
		//}
		START_OPCODES_GROUP(G4_G4)
			virtual void STVX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STVX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLD r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLD. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDME(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("ADDME r%d,r%d", rs, rt));
			}
			virtual void ADDME_(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("ADDME. r%d,r%d", rs, rt));
			}
			virtual void MULLW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLW r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLW_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLW. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DCBTST(OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DCBTST r%d,r%d", rt, rd));
			}
			virtual void SLLIQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SLLIQ r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G4, "G4_G4");
		START_OPCODES_GROUP(G4_G_ADD)
			virtual void ICBT(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: ICBT (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DOZ(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DOZ r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DOZ_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DOZ. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADD r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADD. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LSCBX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LSCBX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LSCBX_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LSCBX. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DCBT(OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DCBT r%d,r%d", rt, rd));
			}
			virtual void LHZX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LHZX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void EQV(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: EQV (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void EQV_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: EQV. (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G_ADD, "G4_G_ADD");
		START_OPCODES_GROUP(G4_G_ADDZE)
			virtual void ADDZE(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("ADDZE r%d,r%d", rs, rt));
			}
			virtual void ADDZE_(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("ADDZE. r%d,r%d", rs, rt));
			}
		END_OPCODES_GROUP(G4_G_ADDZE, "G4_G_ADDZE");
		//virtual void XOR(OP_REG rt, OP_REG rs, OP_REG rd)
		//{
		//	Write(wxString::Format("XOR r%d,r%d,r%d", rt, rs, rd));
		//}
		virtual void SRW(OP_REG rt, OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("SRW r%d,r%d,r%d", rt, rs, rd));
		}
		//virtual void MFLR(OP_REG rs)
		//{
		//	Write(wxString::Format("MFLR r%d", rs));
		//}
		START_OPCODES_GROUP(G4_G_0x9)
			virtual void DOZI(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
			{
				Write(wxString::Format( "DOZI r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
			}
			virtual void LHZUX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LHZUX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void XOR(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("XOR r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G_0x9, "G4_G_0x9");
		START_OPCODES_GROUP(G4_G_0xa)
			virtual void MFDCR(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: MFDCR (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIV(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIV r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIV_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIV. r%d,r%d,r%d", rs, rt, rd));
			}
			//virtual void MFCTR(OP_REG rs)
			//{
			//	Write(wxString::Format("MFCTR r%d", rs));
			//}
			START_OPCODES_GROUP(G4_G_0xa_0x26)
			
			virtual void MFSPR(OP_REG rs, OP_sREG imm_s16)
			{
				Write(wxString::Format( "MFSPR r%d,%d", rs, imm_s16 ));
			}
			virtual void MFLR(OP_REG rs)
			{
				Write(wxString::Format("MFLR r%d", rs));
			}
			virtual void MFCTR(OP_REG rs)
			{
				Write(wxString::Format("MFCTR r%d", rs));
			}
			END_OPCODES_GROUP(G4_G_0xa_0x26, "G4_G_0xa_0x26");
			virtual void LWAX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LWAX r%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G_0xa, "G4_G_0xa");
		START_OPCODES_GROUP(G4_G_0xb)
			virtual void LVXL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVXL v%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MFTB(OP_REG rs)
			{
				Write(wxString::Format("MFTB r%d", rs));
			}
		END_OPCODES_GROUP(G4_G_0xb, "G4_G_0xb");
		START_OPCODES_GROUP(G4_G5)
			virtual void STHX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STHX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ORC(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("ORC r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void ORC_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("ORC. r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G5, "G4_G5");
		
		//virtual void MR(OP_REG rt, OP_REG rs)
		//{
		//	Write(wxString::Format("MR r%d,r%d", rt, rs));
		//}
		START_OPCODES_GROUP(G4_G6)
			virtual void MTDCR(OP_REG rs) //FIXME
			{
				Write(wxString::Format("Unimplemented: MTDCR r%d", rs));
			}
			virtual void DCCCI(OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DCCCI r%d,r%d", rt, rd));
			}
			virtual void DIVDU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVDU r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVDU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVDU. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVWU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVWU r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVWU_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVWU. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DCBI(OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DCBI r%d,r%d", rt, rd));
			}
			virtual void NAND(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("NAND r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void NAND_(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("NAND. r%d,r%d,r%d", rt, rs, rd));
			}
			START_OPCODES_GROUP(G4_G6_G1)
				//virtual void MTSPR(OP_REG rs)
				//{
				//	Write(wxString::Format("MTSPR r%d", rs));
				//}
				virtual void MTLR(OP_REG rs)
				{
					Write(wxString::Format("MTLR r%d", rs));
				}
				virtual void MTCTR(OP_REG rs)
				{
					Write(wxString::Format("MTCTR r%d", rs));
				}
			END_OPCODES_GROUP(G4_G6_G1, "G4_G6_G1");
		END_OPCODES_GROUP(G4_G6, "G4_G6");
		//virtual void DIVW(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	Write(wxString::Format("DIVW r%d,r%d,r%d", rs, rt, rd));
		//}
		START_OPCODES_GROUP(G4_G_0xf)
			virtual void DIVD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVD r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVD_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVD. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVW(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVW r%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G_0xf, "G4_G_0xf");
		START_OPCODES_GROUP(G4_G_ADDCO)
			virtual void MCRXR(OP_REG rs)
			{
				Write(wxString::Format("MCRXR r%d", rs));
			}
			virtual void LVLX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVLX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDCO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDCO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDCO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDCO. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LSWX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: LSWX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LWBRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: LWBRX (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LFSX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LFSX r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MASKIR(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("MASKIR r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G_ADDCO, "G4_G_ADDCO");
		//virtual void SUBFO(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	Write(wxString::Format("SUBFO r%d,r%d,r%d", rs, rt, rd));
		//}
		//virtual void LWSYNC() //FIXME
		//{
		//	Write(wxString::Format("Unipmlemented: LWSYNC"));
		//}
		START_OPCODES_GROUP(G4_G_0x11)
		virtual void LVRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVRX f%d,r%d,r%d", rs, rt, rd));
			}
		virtual void SUBFO(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("SUBFO r%d,r%d,r%d", rs, rt, rd));
		}
		END_OPCODES_GROUP(G4_G_0x11, "G4_G_0x11");
		START_OPCODES_GROUP(G4_G_0x12)
			START_OPCODES_GROUP(G4_G_0x12_0x26)
				virtual void MFSR(OP_REG rs, OP_sREG imm_s16)
				{
					Write(wxString::Format( "MFSR r%d,%d", rs, imm_s16 ));
				}
			END_OPCODES_GROUP(G4_G_0x12_0x26, "G4_G_0x12_0x26");
			virtual void LWSYNC() //FIXME
			{
				Write(wxString::Format("Unipmlemented: LWSYNC"));
			}
			virtual void LFDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LFDX f%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G_0x12, "G4_G_0x12");
		START_OPCODES_GROUP(G4_G7)
			virtual void NEGO() //FIXME
			{
				Write(wxString::Format("Unimplemented: NEGO"));
			}
			virtual void MULO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULO f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULO. f%d,r%d,r%d", rs, rt, rd));
			}
			START_OPCODES_GROUP(G4_G7_0x26)
				virtual void MFSRI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MFSRI f%d,r%d,r%d", rs, rt, rd));
			}
			END_OPCODES_GROUP(G4_G7_0x26, "G4_G7_0x26");
		END_OPCODES_GROUP(G4_G7, "G4_G7");
		//virtual void STFSX(OP_REG rs, OP_REG rt, OP_REG rd) //FIXME
		//{
		//	Write(wxString::Format("STFSX f%d,r%d,r%d", rs, rt, rd));
		//}
		START_OPCODES_GROUP(G4_G_ADDEO)
			virtual void ADDEO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDEO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDEO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDEO. r%d,r%d,r%d", rs, rt, rd));
			}
			START_OPCODES_GROUP(G4_G_ADDEO_0x26)
				virtual void MFSRIN(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("MFSRIN r%d,r%d", rs, rd));
			}
				END_OPCODES_GROUP(G4_G_ADDEO_0x26, "G4_G_ADDEO_0x26");
		END_OPCODES_GROUP(G4_G_ADDEO, "G4_G_ADDEO");
		START_OPCODES_GROUP(G4_G8)
			virtual void ADDZEO_(OP_REG rs, OP_REG rt, OP_REG rd) //temp
			{
				Write(wxString::Format("Unimplemented: ADDZEO. (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void STSWI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STSWI r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void STFDX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("STFDX f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void SRLQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SRLQ r%d,r%d,r%d", rt, rs, rd));
			}
			virtual void SREQ(OP_REG rt, OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("SREQ r%d,r%d,r%d", rt, rs, rd));
			}
		END_OPCODES_GROUP(G4_G8, "G4_G8");
		START_OPCODES_GROUP(G4_G_ADDO)
			virtual void LVLXL(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LVLXL f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DOZO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DOZO f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDO f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ADDO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("ADDO. f%d,r%d,r%d", rs, rt, rd));
			}
			virtual void LHBRX(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("LHBRX f%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G_ADDO, "G4_G_ADDO");
		START_OPCODES_GROUP(G4_G9)
			virtual void MULLDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLDO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLDO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLDO. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLWO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLWO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MULLWO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("MULLWO. r%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G9, "G4_G9");
		//virtual void SRAW(OP_REG rt, OP_REG rs, OP_REG rd)
		//{
		//	Write(wxString::Format("SRAW r%d,r%d,r%d", rt, rs, rd));
		//}
		virtual void SRAWI(OP_REG rt, OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("SRAWI r%d,r%d,r%d", rt, rs, rd));
		}
		//virtual void EIEIO()
		//{
		//	Write(wxString::Format("EIEIO"));
		//}
		START_OPCODES_GROUP(G4_G_0x1a)
			virtual void DIVO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVO. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void EIEIO()
			{
				Write(wxString::Format("EIEIO"));
			}
		END_OPCODES_GROUP(G4_G_0x1a, "G4_G_0x1a");
		START_OPCODES_GROUP(G4_G10)
			virtual void ABSO_() //FIXME
			{
				Write(wxString::Format("Unimplemented: ABSO."));
			}
			virtual void DIVSO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVSO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVSO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVSO. r%d,r%d,r%d", rs, rt, rd));
			}
		END_OPCODES_GROUP(G4_G10, "G4_G10");
		START_OPCODES_GROUP(G4_G_0x1e)
			virtual void ICCCI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: ICCCI (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void RLDCL_(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				Write(wxString::Format( "RLDCL. r%d,r%d,r%d,%d", rt, rs, rd, imm_s16 ));
			}
			virtual void DIVDUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVDUO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVDUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVDUO. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVWUO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVWUO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVWUO_(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVWUO. r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void ICBI(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: ICBI (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void RLDCL(OP_REG rt, OP_REG rs, OP_REG rd, OP_sREG imm_s16)
			{
				Write(wxString::Format( "RLDCL r%d,r%d,r%d,%d", rt, rs, rd, imm_s16 ));
			}
			virtual void EXTSW(OP_REG rt, OP_REG rs)
			{
				Write(wxString::Format("EXTSW r%d,r%d", rt, rs));
			}
			virtual void EXTSW_(OP_REG rt, OP_REG rs)
			{
				Write(wxString::Format("EXTSW. r%d,r%d", rt, rs));
			}
		END_OPCODES_GROUP(G4_G_0x1e, "G4_G_0x1e");
		START_OPCODES_GROUP(G4_G_0x1f)
			virtual void ICREAD(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("Unimplemented: ICREAD (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DIVDO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DIVDO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void DCBZ(OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("DCBZ r%d,r%d", rt, rd));
			}
		END_OPCODES_GROUP(G4_G_0x1f, "G4_G_0x1f");
		virtual void EXTSH(OP_REG rt, OP_REG rs)
		{
			Write(wxString::Format("EXTSH r%d,r%d", rt, rs));
		}
		virtual void EXTSB(OP_REG rt, OP_REG rs)
		{
			Write(wxString::Format("EXTSB r%d,r%d", rt, rs));
		}
		//virtual void EXTSW(OP_REG rt, OP_REG rs)
		//{
		//	Write(wxString::Format("EXTSW r%d,r%d", rt, rs));
		//}
		//virtual void DCBZ(OP_REG rt, OP_REG rd)
		//{
		//	Write(wxString::Format("DCBZ r%d,r%d", rt, rd));
		//}
		virtual void LWZX(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("LWZX r%d,r%d,r%d", rs, rt, rd));
		}
	END_OPCODES_GROUP(G4, "G4");
	virtual void LWZ(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LWZ r%d,%d(r%d)  # %x", rt, imm_s16, rs, imm_s16 ));
	}
	virtual void LWZU(OP_REG rt, OP_REG rs, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LWZU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LBZ(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LBZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LBZU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LBZU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STW(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STWU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STW r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STB(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STB r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STBU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STBU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LHZ(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LHZ r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LHZU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LHZU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LHA(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LHA r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LHAU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LHAU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STH(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STH r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STHU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STHU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LMW(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LMW f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LFS(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LFSU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LFSU f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LFD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void LFDU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "LFDU f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STFS(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STFS f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STFSU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STFSU f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	virtual void STFD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		Write(wxString::Format( "STFD f%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	}
	//virtual void LD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	//{
	//	Write(wxString::Format( "LD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
	//}
	START_OPCODES_GROUP(G_0x3a)
		virtual void LDU(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "LDU r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
		}
		virtual void LWA(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "LWA r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
		}
		virtual void LD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
		{
			Write(wxString::Format( "LD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
		}
	END_OPCODES_GROUP(G_0x3a, "G_0x3a");
	START_OPCODES_GROUP(G4_S)
	/*
	virtual void FDIVS(OP_REG rs, OP_REG rt, OP_REG rd)
	{
		Write(wxString::Format( "FDIVS f%d, f%d, f%d", rs, rt, rd ));
	}
	*/
		virtual void FSUBS(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FSUBS r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FSUBS_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FSUBS. r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FADDS(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FADDS r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FSQRTS(OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("FSQRTS r%d,r%d", rs, rd));
		}
		virtual void FRES(OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("FRES r%d,r%d", rs, rd));
		}
		virtual void FMULS(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			Write(wxString::Format("FMULS r%d,r%d,r%d", rs, rt, sa));
		}
		virtual void FMULS_(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			Write(wxString::Format("FMULS. r%d,r%d,r%d", rs, rt, sa));
		}
		virtual void FMADDS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMADDS r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMADDS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMADDS. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMSUBS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMSUBS r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMSUBS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMSUBS. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMSUBS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMSUBS r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMSUBS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMSUBS. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMADDS(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMADDS r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMADDS_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMADDS. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
	END_OPCODES_GROUP(G4_S, "G4_S");
	virtual void STD(OP_REG rs, OP_REG rt, OP_sREG imm_s16)
	{
		if(imm_s16 < 0)
		{
			Write(wxString::Format( "STDU r%d,%d(r%d)  # %x", rs, imm_s16 - 1, rt, imm_s16 - 1 ));
		}
		else
		{
			Write(wxString::Format( "STD r%d,%d(r%d)  # %x", rs, imm_s16, rt, imm_s16 ));
		}
	}
	//virtual void FCFID(OP_REG rs, OP_REG rd)
	//{
	//	Write(wxString::Format( "FCFID f%d, f%d", rs, rd ));
	//}
	START_OPCODES_GROUP(G5)
		START_OPCODES_GROUP(G5_G0x0)
			virtual void FCMPU(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("FCMPU r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void FCMPO(OP_REG rs, OP_REG rt, OP_REG rd)
			{
				Write(wxString::Format("FCMPO r%d,r%d,r%d", rs, rt, rd));
			}
			virtual void MCRFS(OP_REG rs, OP_REG rt)
			{
				Write(wxString::Format("MCRFS r%d,r%d", rs, rt));
			}
		END_OPCODES_GROUP(G5_G0x0, "G5_G0x0");
		START_OPCODES_GROUP(G5_G0x10)
			virtual void FNEG(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FNEG r%d,r%d", rs, rd));
			}
			virtual void FMR(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FMR r%d,r%d", rs, rd));
			}
			virtual void FNABS(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FNABS r%d,r%d", rs, rd));
			}
			virtual void FABS(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FABS r%d,r%d", rs, rd));
			}
		END_OPCODES_GROUP(G5_G0x10, "G5_G0x10");
		START_OPCODES_GROUP(G5_G0x1c)
			virtual void FCTIW(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FCTIW r%d,r%d", rs, rd));
			}
			virtual void FCTID(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FCTID r%d,r%d", rs, rd));
			}
			virtual void FCFID(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FCFID r%d,r%d", rs, rd));
			}
		END_OPCODES_GROUP(G5_G0x1c, "G5_G0x1c");
		START_OPCODES_GROUP(G5_G0x1e)
			virtual void FCTIWZ(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FCTIWZ r%d,r%d", rs, rd));
			}
			virtual void FCTIDZ(OP_REG rs, OP_REG rd)
			{
				Write(wxString::Format("FCTIDZ r%d,r%d", rs, rd));
			}
		END_OPCODES_GROUP(G5_G0x1e, "G5_G0x1e");
		//virtual void MFFS(OP_REG rs, OP_REG rt, OP_REG rd)
		//{
		//	Write(wxString::Format("Unimplemented: MFFS (unk, rs/rt/rd) r%d,r%d,r%d", rs, rt, rd));
		//}
		virtual void FDIV(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FDIV r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FDIV_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FDIV. r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FSUB(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FSUB r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FSUB_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FSUB. r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FADD(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FADD r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FADD_(OP_REG rs, OP_REG rt, OP_REG rd)
		{
			Write(wxString::Format("FADD. r%d,r%d,r%d", rs, rt, rd));
		}
		virtual void FSQRT(OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("FSQRT r%d,r%d", rs, rd));
		}
		virtual void FSEL(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FSEL r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FSEL_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FSEL. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMUL(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			Write(wxString::Format("FMUL f%d,f%d,f%d", rs, rt, sa));
		}
		virtual void FMUL_(OP_REG rs, OP_REG rt, OP_REG sa)
		{
			Write(wxString::Format("FMUL. f%d,f%d,f%d", rs, rt, sa));
		}
		virtual void FRSQRTE(OP_REG rs, OP_REG rd)
		{
			Write(wxString::Format("FRSQRTE r%d,r%d", rs, rd));
		}
		virtual void FMSUB(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMSUB r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMSUB_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMSUB. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FMADD(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FMADD r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMSUB(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMSUB r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMSUB_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMSUB. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMADD(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMADD r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
		virtual void FNMADD_(OP_REG rs, OP_REG rt, OP_REG sa, OP_REG rd)
		{
			Write(wxString::Format("FNMADD. r%d,r%d,r%d,r%d", rs, rt, sa, rd));
		}
	END_OPCODES_GROUP(G5, "G5");
	
	virtual void UNK(OP_REG code, OP_REG opcode, OP_REG rs, OP_REG rt, OP_REG rd,
		OP_REG sa, OP_REG func, OP_sREG imm_s16, OP_REG imm_u16, OP_REG imm_u26)
	{
		Write(wxString::Format(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		));
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP