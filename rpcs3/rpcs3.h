#pragma once

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"
#include "Emu/GameInfo.h"
#include "Emu/Cell/PPCThread.h"

class Rpcs3App : public wxApp
{
public:
	MainFrame* m_MainFrame;

    virtual bool OnInit();
	virtual void Exit();
};

DECLARE_APP(Rpcs3App)

//extern CPUThread& GetCPU(const u8 core);

extern Rpcs3App* TheApp;
static const u64 PS3_CLK = 3200000000;