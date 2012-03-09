#pragma once

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"
#include "Emu/GameInfo.h"
#include "Emu/Cell/PPCThread.h"

template<typename T> T min(const T a, const T b) { return a < b ? a : b; }
template<typename T> T max(const T a, const T b) { return a > b ? a : b; }

#define re(val) MemoryBase::Reverse(val)

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