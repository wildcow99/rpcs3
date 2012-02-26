#pragma once

#include "Emu/Cell/SPUOpcodes.h"
#include "Emu/Cell/DisAsm.h"
#include "Emu/Cell/SPUThread.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Memory/Memory.h"

#define START_OPCODES_GROUP(x) /*x*/
#define END_OPCODES_GROUP(x) /*x*/

class SPU_DisAsm 
	: public SPU_Opcodes
	, public DisAsm
{
public:
	PPCThread& CPU;

	SPU_DisAsm()
		: DisAsm(*(PPCThread*)NULL, DumpMode)
		, CPU(*(PPCThread*)NULL)
	{
	}

	SPU_DisAsm(PPCThread& cpu, DisAsmModes mode = NormalMode)
		: DisAsm(cpu, mode)
		, CPU(cpu)
	{
	}

	~SPU_DisAsm()
	{
	}

private:
	virtual void Exit()
	{
		if(m_mode == NormalMode && !disasm_frame->exit)
		{
			disasm_frame->Close();
		}

		this->~SPU_DisAsm();
	}

	virtual u32 DisAsmBranchTarget(const s32 imm)
	{
		return branchTarget(m_mode == NormalMode ? CPU.PC : dump_pc, imm);
	}

private:
	//0 - 10
	virtual void STOP(OP_uIMM code)
	{
		Write(wxString::Format("stop 0x%x", code));
	}
	virtual void LNOP()
	{
		Write("lnop");
	}
	virtual void RDCH(OP_REG rt, OP_REG ra)
	{
		//Unk/0x01a00c03 ->  27c:	01 a0 0c 03 	rdch	$3,$ch24
		Write(wxString::Format("rdch $%d,$ch%d", rt, ra));
	}
	virtual void RCHCNT(OP_REG rt, OP_REG ra)
	{
		//Unk/0x01e00c05 ->  260:	01 e0 0c 05 	rchcnt	$5,$ch24
		Write(wxString::Format("rchcnt $%d,$ch%d", rt, ra));
	}
	virtual void SF(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		Write(wxString::Format("sf $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void SHLI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		//Unk/0x0f608487 ->  1c0:	0f 60 84 87 	shli	$7,$9,2
		Write(wxString::Format("shli $%d,$%d,%d", rt, ra, i7));
	}
	virtual void A(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x18140384 ->  1c8:	18 14 03 84 	a		$4,$7,$80
		Write(wxString::Format("a $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void SPU_AND(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x18210282 ->  12c:	18 21 02 82 	and		$2,$5,$4
		Write(wxString::Format("and $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void LQX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x38940386 ->  1cc:	38 94 03 86 	lqx		$6,$7,$80
		Write(wxString::Format("lqx $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void WRCH(OP_REG ra, OP_REG rt)
	{
		//Unk/0x21a00b82 ->  25c:	21 a0 0b 82 	wrch	$ch23,$2
		Write(wxString::Format("wrch $ch%d,$%d", ra, rt));
	}
	virtual void STQX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x2881828b ->  24c:	28 81 82 8b 	stqx	$11,$5,$6
		Write(wxString::Format("stqx $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void BI(OP_REG ra)
	{
		Write(wxString::Format("bi $%d", ra));
	}
	virtual void BISL(OP_REG rt, OP_REG ra)
	{
		//Unk/0x35200280 ->  1d4:	35 20 02 80 	bisl	$0,$5
		Write(wxString::Format("bisl $%d,$%d", rt, ra));
	}
	virtual void HBR(OP_REG p, OP_REG ro, OP_REG ra)
	{
		//Unk/0x35800014 ->  204:	35 80 00 14 	hbr		0x254,$0
		Write(wxString::Format("hbr 0x%x,$%d", DisAsmBranchTarget(ro), ra));
	}
	virtual void CWX(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x3ac1828d ->  240:	3a c1 82 8d 	cwx		$13,$5,$6
		Write(wxString::Format("cwx $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void ROTQBY(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x3b810305 ->  1d0:	3b 81 03 05 	rotqby	$5,$6,$4
		Write(wxString::Format("rotqby $%d,$%d,$%d", rt, ra, rb));
	}
	virtual void ROTQBYI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		Write(wxString::Format("rotqbyi $%d,$%d,%d", rt, ra, i7));
	}
	virtual void SHLQBYI(OP_REG rt, OP_REG ra, OP_sIMM i7)
	{
		Write(wxString::Format("shlqbyi $%d,$%d,%d", rt, ra, i7));
	}
	virtual void SPU_NOP(OP_REG rt)
	{
		Write(wxString::Format("nop $%d", rt));
	}
	virtual void CLGT(OP_REG rt, OP_REG ra, OP_REG rb)
	{
		//Unk/0x5800c505 ->  124:	58 00 c5 05 	clgt	$5,$10,$3
		Write(wxString::Format("clgt $%d,$%d,$%d", rt, ra, rb));
	}

	//0 - 8
	virtual void BRZ(OP_REG rt, OP_sIMM i16)
	{
		//Unk/0x207ffe84 ->  26c:	20 7f fe 84 	brz		$4,0x260
		Write(wxString::Format("brz $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}
	virtual void BRHZ(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("brhz $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}
	virtual void BRHNZ(OP_REG rt, OP_sIMM i16)
	{
		//Unk/0x237ff982 ->  1e4:	23 7f f9 82 	brhnz	$2,0x1b0
		Write(wxString::Format("brhnz $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}
	virtual void STQR(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("stqr $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}
	virtual void BR(OP_sIMM i16)
	{
		Write(wxString::Format("br 0x%x", DisAsmBranchTarget(i16)));
	}
	virtual void FSMBI(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("fsmbi $%d,%d", rt, i16));
	}
	virtual void BRSL(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("brsl $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}
	virtual void IL(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("il $%d,%d", rt, i16));
	}
	virtual void LQR(OP_REG rt, OP_sIMM i16)
	{
		Write(wxString::Format("lqr $%d,0x%x", rt, DisAsmBranchTarget(i16)));
	}

	//0 - 7
	virtual void SPU_ORI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		Write(wxString::Format("ori $%d,$%d,%d", rt, ra, i10));
	}
	virtual void AI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		Write(wxString::Format("ai $%d,$%d,%d", rt, ra, i10));
	}
	virtual void AHI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		Write(wxString::Format("ahi $%d,$%d,%d", rt, ra, i10));
	}
	virtual void STQD(OP_REG rt, OP_sIMM i10, OP_REG ra)
	{
		Write(wxString::Format("stqd $%d,%d($%d)", rt, i10, ra));
	}
	virtual void LQD(OP_REG rt, OP_sIMM i10, OP_REG ra)
	{
		Write(wxString::Format("lqd $%d,%d($%d)", rt, i10, ra));
	}
	virtual void CLGTI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		//Unk/0x5c000184 ->  128:	5c 00 01 84 	clgti	$4,$3,0
		Write(wxString::Format("clgti $%d,$%d,%d", rt, ra, i10));
	}
	virtual void CLGTHI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		//Unk/0x5d13c482 ->  224:	5d 13 c4 82 	clgthi	$2,$9,79
		Write(wxString::Format("clgthi $%d,$%d,%d", rt, ra, i10));
	}
	virtual void CEQI(OP_REG rt, OP_REG ra, OP_sIMM i10)
	{
		//Unk/0x7c004284 ->  264:	7c 00 42 84 	ceqi	$4,$5,1
		Write(wxString::Format("ceqi $%d,$%d,%d", rt, ra, i10));
	}

	//0 - 6
	virtual void HBRR(OP_sIMM ro, OP_sIMM i16)
	{
		Write(wxString::Format("hbrr 0x%x,0x%x", DisAsmBranchTarget(ro), DisAsmBranchTarget(i16)));
	}
	virtual void ILA(OP_REG rt, OP_sIMM i18)
	{
		Write(wxString::Format("ila $%d,%d", rt, i18));
	}

	//0 - 3
	virtual void SELB(OP_REG rc, OP_REG ra, OP_REG rb, OP_REG rt)
	{
		Write(wxString::Format("selb $%d,$%d,$%d,$%d", rc, ra, rb, rt));
	}
	virtual void SHUFB(OP_REG rc, OP_REG ra, OP_REG rb, OP_REG rt)
	{
		Write(wxString::Format("shufb $%d,$%d,$%d,$%d", rc, ra, rb, rt));
	}

	virtual void UNK(const s32 code, const s32 opcode, const s32 gcode)
	{
		Write(wxString::Format("Unknown/Illegal opcode! (0x%08x)", code));
	}
};

#undef START_OPCODES_GROUP
#undef END_OPCODES_GROUP