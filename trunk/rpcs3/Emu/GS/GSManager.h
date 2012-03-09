#pragma once
#include "sysutil_video.h"
#include "GSRender.h"

struct GSInfo
{
	CellVideoOutResolution outresolution;
	CellVideoOutDisplayMode mode;

	GSInfo()
	{
		outresolution.width = 800;
		outresolution.height = 600;

		mode.resolutionId = CELL_VIDEO_OUT_RESOLUTION_576;
		mode.scanMode = CELL_VIDEO_OUT_SCAN_MODE_INTERLACE;
		mode.conversion = CELL_VIDEO_OUT_DISPLAY_CONVERSION_NONE;
		mode.aspect = CELL_VIDEO_OUT_ASPECT_4_3;
		mode.refreshRates = CELL_VIDEO_OUT_REFRESH_RATE_50HZ;
	}
};

class GSManager
{
	GSInfo m_info;
	GSRender* m_render;

public:
	GSManager();

	void Init();
	void Close();

	GSInfo& GetInfo() { return m_info; }

	u8 GetState();
	u8 GetColorSpace();
};