#include "Emu/Opcodes/Opcodes.h"
#include "Emu/Display/Display.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/SysCalls/SysCalls.h"

#include "rpcs3.h"

#include <wx/generic/progdlgg.h>

class InterpreterOpcodes : public Opcodes
{
private:
	Display* display;

public:
	InterpreterOpcodes()
	{
		display = new Display();
	}

private:
	virtual void Exit()
	{
		delete display;
	}

	virtual void Step()
	{
		/*
		if(++CPU.cycle >= (/*PS3_CLK **//* 1000000ULL) / (60*100))
		{
			//display->Flip();
			CPU.cycle = 0;
		}
		*/
	}

	virtual bool DoSysCall(const int code)
	{
		switch(code)
		{
		case cellPadClearBuf:					ConLog.Warning( "SYSCALL: cellPadClearBuf" );				return true;
		case cellPadInfoPressMode:				ConLog.Warning( "SYSCALL: cellPadInfoPressMode" );			return true;
		case cellPadInit:						ConLog.Warning( "SYSCALL: cellPadInit" );					return true;
		case cellPadLddRegisterController:		ConLog.Warning( "SYSCALL: cellPadLddRegisterController" );	return true;
		case cellPadGetInfo:					ConLog.Warning( "SYSCALL: cellPadGetInfo" );				return true;
		case cellPadGetRawData:					ConLog.Warning( "SYSCALL: cellPadGetRawData" );				return true;
		case cellPadEnd:						ConLog.Warning( "SYSCALL: cellPadEnd" );					return true;
		case cellPadInfoSensorMode:				ConLog.Warning( "SYSCALL: cellPadInfoSensorMode" );			return true;
		case cellPadGetData:					ConLog.Warning( "SYSCALL: cellPadGetData" );				return true;
		case cellPadLddDataInsert:				ConLog.Warning( "SYSCALL: cellPadLddDataInsert" );			return true;
		case cellPadSetSensorMode:				ConLog.Warning( "SYSCALL: cellPadSetSensorMode" );			return true;
		case cellPadLddUnregisterController:	ConLog.Warning( "SYSCALL: cellPadLddUnregisterController" );return true;
		case cellPadSetPressMode:				ConLog.Warning( "SYSCALL: cellPadSetPressMode" );			return true;

		case cellVideoOutGetDeviceInfo:			ConLog.Warning( "SYSCALL: cellVideoOutGetDeviceInfo" );		return true;
		case cellVideoOutGetNumberOfDevice:		ConLog.Warning( "SYSCALL: cellVideoOutGetNumberOfDevice" );	return true;
		case cellVideoOutGetState:				ConLog.Warning( "SYSCALL: cellVideoOutGetState" );			return true;
		case cellVideoOutRegisterCallback:		ConLog.Warning( "SYSCALL: cellVideoOutRegisterCallback" );	return true;
		case cellVideoOutDebugSetMonitorType:	ConLog.Warning( "SYSCALL: cellVideoOutDebugSetMonitorType");return true;

		case cellRescGcmSurface2RescSrc:		ConLog.Warning( "SYSCALL: cellRescGcmSurface2RescSrc" );	return true;
		case cellRescGetNumColorBuffers:		ConLog.Warning( "SYSCALL: cellRescGetNumColorBuffers" );	return true;
		case cellRescSetWaitFlip:				ConLog.Warning( "SYSCALL: cellRescSetWaitFlip" );			return true;
		case cellRescSetDsts:					ConLog.Warning( "SYSCALL: cellRescSetDsts" );				return true;
		case cellRescResetFlipStatus:			ConLog.Warning( "SYSCALL: cellRescResetFlipStatus" );		return true;

		case cellRescSetPalInterpolateDropFlexRatio:
			ConLog.Warning( "SYSCALL: cellRescSetPalInterpolateDropFlexRatio" );			
		return true;

		case cellRescGetRegisterCount:			ConLog.Warning( "SYSCALL: cellRescGetRegisterCount" );		return true;
		case cellRescAdjustAspectRatio:			ConLog.Warning( "SYSCALL: cellRescAdjustAspectRatio" );		return true;

		case cellRescSetDisplayMode:
			ConLog.Write( "SYSCALL: cellRescSetDisplayMode" );
			display->SetMode();
		return true;

		case cellRescSetConvertAndFlip:
			ConLog.Write( "SYSCALL: cellRescSetConvertAndFlip" );
			display->Flip();
		return true;

		case cellRescExit:						ConLog.Warning( "SYSCALL: cellRescExit" );					return true;
		case cellRescSetFlipHandler:			ConLog.Warning( "SYSCALL: cellRescSetFlipHandler" );		return true;
		case cellRescInit:						ConLog.Warning( "SYSCALL: cellRescInit" );					return true;
		case cellRescGetBufferSize:				ConLog.Warning( "SYSCALL: cellRescGetBufferSize" );			return true;
		case cellRescGetLastFlipTime:			ConLog.Warning( "SYSCALL: cellRescGetLastFlipTime" );		return true;
		case cellRescSetSrc:					ConLog.Warning( "SYSCALL: cellRescSetSrc" );				return true;
		case cellRescSetRegisterCount:			ConLog.Warning( "SYSCALL: cellRescSetRegisterCount" );		return true;

		case cellRescSetBufferAddress:
			ConLog.Write( "SYSCALL: cellRescSetBufferAddress" );
			display->SetBufferAddr();
		return true;

		case cellRescGetFlipStatus:				ConLog.Warning( "SYSCALL: cellRescGetFlipStatus" );			return true;

		case cellRescVideoOutResolutionId2RescBufferMode:
			ConLog.Warning( "SYSCALL: cellRescVideoOutResolutionId2RescBufferMode" );
		return true;

		case cellRescSetVBlankHandler:			ConLog.Warning( "SYSCALL: cellRescSetVBlankHandler" );		return true;
		case cellRescCreateInterlaceTable:		ConLog.Warning( "SYSCALL: cellRescCreateInterlaceTable" );	return true;

		case cellVideoOutGetResolutionAvailability:
			ConLog.Warning( "SYSCALL: cellVideoOutGetResolutionAvailability" );
		return true;

		default: return false;
		};

		return false;
	}

	virtual void NOP()
	{
	}

	// Special
	virtual void UNK_SPECIAL(const int code, const int rs, const int rt, const int rd,
		const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown special opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, func, rs, rt, rd, sa, sa, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
	//

	virtual void SPECIAL2()
	{
		ConLog.Error("SPECIAL 2");
	}

	virtual void XOR(const int rd, const int rs, const int rt)
	{
		 if (rd != 0) CPU.GPR[rd] = CPU.GPR[rs] ^ CPU.GPR[rt];
	}

	virtual void MULLI(const int rt, const int rs, const int imm_s16)
	{
		if (rt != 0) CPU.GPR[rt] = ((s64)CPU.GPR[rs] * (s64)imm_s16) & 0xffffffffLL;
	}

	virtual void SUBFIC(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("SUBFIC");
	}

	virtual void CMPLWI(const int rs, const int rt, const int imm_u16)
	{
		CPU.UpdateCR0(CPU.GPR[rt], imm_u16);
		if(rs != 0) for(uint i=0; i<4; ++i) CPU.CR[rs/4][i] = CPU.CR[0][i];
	}

	virtual void CMPWI(const int rs, const int rt, const int rd)
	{
		CPU.UpdateCR0(CPU.GPR[rt], CPU.GPR[rd]);
		if(rs != 0) for(uint i=0; i<4; ++i) CPU.CR[rs/4][i] = CPU.CR[0][i];
	}

	virtual void ADDIC(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;
		CPU.GPR[rs] = (CPU.GPR[rs] | 0) + imm_s16;
		CPU.XER[XER_CA] = 1;
	}

	virtual void ADDIC_(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;
		ConLog.Warning("CHECK ME: ADDIC.");

		const int value = (CPU.GPR[rs] | 0) + imm_s16;
		CPU.GPR[rs] = value;
		CPU.XER[XER_CA] = 1;
		CPU.UpdateCR0(value);
	}

	//g1 - 0e
	virtual void G1()
	{
		ConLog.Error("G1");
	}
	//
	//virtual void ADDI(const int rt, const int rs, const int imm_u16)=0;
	//
	virtual void ADDIS(const int rt, const int rs, const int imm_s16)
	{
		if(rt != 0) CPU.GPR[rt] = (CPU.GPR[rs] | 0) + (imm_s16 << 16);
	}
	//g2 - 10
	virtual void G2()
	{
		ConLog.Error("G2");
	}

	//
	virtual void SC()
	{
		CPU.NextPc();

		const u32 code =  (Memory.Read32(CPU.PC) >> 6) & 0x000fffff;

		if(DoSysCall(code)) return;

		ConLog.Warning( "SYSCODE: %08X", code );
	}

	//g3 - 12
	virtual void G3()
	{
		ConLog.Error("G3");
	}
	//
	virtual void BLR()
	{
		ConLog.Error("BLR");
	}

	virtual void RLWINM()
	{
		ConLog.Error("RLWINM");
	}

	virtual void ROTLW(const int rt, const int rs, const int rd)
	{
		ConLog.Error("ROTLW");
	}

	virtual void ORI(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | imm_u16;
	}

	virtual void ORIS(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] | (imm_u16 << 16);
	}

	virtual void XORI(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ imm_u16;
	}

	virtual void XORIS(const int rt, const int rs, const int imm_u16)
	{
		if (rt != 0) CPU.GPR[rt] = CPU.GPR[rs] ^ (imm_u16 << 16);
	}

	virtual void CLRLDI(const int rt, const int rs, const int imm_u16)
	{
		ConLog.Error("CLRLDI");
	}

	//g4 - 1f
	virtual void G4()
	{
		ConLog.Error("G4");
	}

	//virtual void MFLR(const int rs)=0;
	//
	virtual void LWZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read32(CPU.GPR[rt] + imm_s16);
	}

	virtual void LWZU(const int rt, const int rs, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read32(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void LBZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read8(CPU.GPR[rt] + imm_s16);
	}

	virtual void LBZU(const int rt, const int rs, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read8(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void STW(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write32(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STWU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write32(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void STB(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write8(CPU.GPR[rs] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STBU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write8(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void LHZ(const int rs, const int rt, const int imm_s16)
	{
		CPU.GPR[rs] = Memory.Read16(CPU.GPR[rt] + imm_s16);
	}

	virtual void LHZU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0 || rs == rt) return;

		const int addr = CPU.GPR[rs] + imm_s16;

		CPU.GPR[rt] = Memory.Read16(addr);
		CPU.GPR[rs] = addr;
	}

	virtual void STH(const int rs, const int rt, const int imm_s16)
	{
		Memory.Write16(CPU.GPR[rt] + imm_s16, CPU.GPR[rs]);
	}

	virtual void STHU(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;

		const int addr = CPU.GPR[rt] + imm_s16;

		Memory.Write16(addr, CPU.GPR[rs]);
		CPU.GPR[rs] = addr;
	}

	virtual void LFS(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("LFS");
	}

	virtual void LFSU(const int rs, const int rt, const int imm_u16)
	{
		ConLog.Error("LFSU");
	}

	virtual void LFD(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("LFD");
	}

	virtual void STFS(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("STFS");
	}

	virtual void STFSU(const int rs, const int rt, const int imm_u16)
	{
		ConLog.Error("STFSU");
	}

	virtual void STFD(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("STFD");
	}

	virtual void LD(const int rs, const int rt, const int imm_s16)
	{
		if(rs == 0) return;
		CPU.GPR[rs] = Memory.Read64(CPU.GPR[rt] + imm_s16);
	}

	virtual void FDIVS(const int rs, const int rt, const int rd)
	{
		ConLog.Error("FDIVS");
	}

	virtual void STD(const int rs, const int rt, const int imm_s16)
	{
		ConLog.Error("STD");
	}

	//g5 - 3f
	virtual void G5()
	{
		ConLog.Error("G5");
	}
	//virtual void FCFID(const int rs, const int rd)=0;
	//
	
	virtual void UNK(const int code, const int opcode, const int rs, const int rt,
		const int rd, const int sa, const int func, const int imm_s16, const int imm_u16, const int imm_u26)
	{
		ConLog.Error
		(
			"Unknown opcode! - (%08x - %02x) - rs: r%d, rt: r%d, rd: r%d, sa: 0x%x : %d, func: 0x%x : %d, imm s16: 0x%x : %d, imm u16: 0x%x : %d, imm u26: 0x%x : %d",
			code, opcode, rs, rt, rd, sa, sa, func, func, imm_s16, imm_s16, imm_u16, imm_u16, imm_u26, imm_u26
		);
	}
};