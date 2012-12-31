#pragma once
#include "sysutil_video.h"
#include "GSRender.h"

struct GSInfo
{
	CellVideoOutDisplayMode mode;

	GSInfo()
	{
	}

	void Init()
	{
		mode.resolutionId = Ini.GSResolution.GetValue();
		mode.scanMode = CELL_VIDEO_OUT_SCAN_MODE_INTERLACE;
		mode.conversion = CELL_VIDEO_OUT_DISPLAY_CONVERSION_NONE;
		mode.aspect = Ini.GSAspectRatio.GetValue();
		mode.refreshRates = CELL_VIDEO_OUT_REFRESH_RATE_50HZ;
	}
};

struct gcmBuffer
{
	u32 offset;
	u32 pitch;
	u32 width;
	u32 height;
	bool update;

	gcmBuffer() : update(false)
	{
	}
};

extern gcmBuffer gcmBuffers[2];

class GSManager
{
	GSInfo m_info;
	GSRender* m_render;

public:
	GSManager();

	void Init();
	void Close();

	GSInfo& GetInfo() { return m_info; }
	GSRender& GetRender() { assert(m_render); return *m_render; }

	u8 GetState();
	u8 GetColorSpace();
};