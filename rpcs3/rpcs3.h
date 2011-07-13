#pragma once

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"
#include "Emu/GameInfo.h"

class Rpcs3App : public wxApp
{
	MainFrame* m_main;

public:
    virtual bool OnInit();
	virtual void CleanUp();
	virtual void Exit();
};

extern Rpcs3App* TheApp;
static const u64 PS3_CLK = 3200000000;