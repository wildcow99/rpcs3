#include "stdafx.h"
#include "Emu/GS/sysutil_video.h"
#include "Emu/SysCalls/SysCalls.h"

//cellVideo
SysCallBase sc_v("cellVideo");

int cellVideoOutGetState(u32 videoOut, u32 deviceIndex, u32 state_addr)
{
	sc_v.Log("cellVideoOutGetState(videoOut=0x%x, deviceIndex=0x%x, state_addr=0x%x)", videoOut, deviceIndex, state_addr);

	if(deviceIndex) return CELL_VIDEO_OUT_ERROR_DEVICE_NOT_FOUND;

	CellVideoOutState state;
	memset(&state, 0, sizeof(CellVideoOutState));

	switch(videoOut)
	{
		case CELL_VIDEO_OUT_PRIMARY:
		{
			state.colorSpace = Emu.GetGSManager().GetColorSpace();
			state.state = Emu.GetGSManager().GetState();
			state.displayMode.resolutionId = Emu.GetGSManager().GetInfo().mode.resolutionId;
			state.displayMode.scanMode = Emu.GetGSManager().GetInfo().mode.scanMode;
			state.displayMode.conversion = Emu.GetGSManager().GetInfo().mode.conversion;
			state.displayMode.aspect = Emu.GetGSManager().GetInfo().mode.aspect;
			state.displayMode.refreshRates = re(Emu.GetGSManager().GetInfo().mode.refreshRates);

			Memory.WriteData(state_addr, state);
		}
		return CELL_VIDEO_OUT_SUCCEEDED;

		case CELL_VIDEO_OUT_SECONDARY:
		{
			state.colorSpace = CELL_VIDEO_OUT_COLOR_SPACE_RGB;
			state.state = CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED;

			Memory.WriteData(state_addr, state);
		}
		return CELL_VIDEO_OUT_SUCCEEDED;
	}

	return CELL_VIDEO_OUT_ERROR_UNSUPPORTED_VIDEO_OUT;
}

int cellVideoOutGetResolution(u32 resolutionId, u32 resolution_addr)
{
	sc_v.Log("cellVideoOutGetResolution(resolutionId=%d, resolution_addr=0x%x)",
		resolutionId, resolution_addr);

	if(!Memory.IsGoodAddr(resolution_addr, sizeof(CellVideoOutResolution))) return CELL_EFAULT;
	
	u32 num = ResolutionIdToNum(resolutionId);
	if(!num) return CELL_EINVAL;

	CellVideoOutResolution res;
	re(res.width, ResolutionTable[num].width);
	re(res.height, ResolutionTable[num].height);

	Memory.WriteData(resolution_addr, res);
	return CELL_OK;
}

SysCallBase sc_sysutil("cellSysutil");

int cellSysutilCheckCallback()
{
	//sc_sysutil.Warning("cellSysutilCheckCallback()");
	Emu.GetCallbackManager().m_exit_callback.Check();

	return CELL_OK;
}

int cellSysutilRegisterCallback(int slot, u64 func_addr, u64 userdata)
{
	sc_sysutil.Warning("cellSysutilRegisterCallback(slot=%d, func_addr=0x%llx, userdata=0x%llx)", slot, func_addr, userdata);
	Emu.GetCallbackManager().m_exit_callback.Register(slot, func_addr, userdata);

	wxGetApp().m_MainFrame->UpdateUI();

	return CELL_OK;
}

int cellSysutilUnregisterCallback(int slot)
{
	sc_sysutil.Warning("cellSysutilUnregisterCallback(slot=%d)", slot);
	Emu.GetCallbackManager().m_exit_callback.Unregister(slot);

	wxGetApp().m_MainFrame->UpdateUI();

	return CELL_OK;
}