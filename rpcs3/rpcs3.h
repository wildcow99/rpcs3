#pragma once

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"
#include "Emu/GameInfo.h"

class TheApp : public wxApp
{
public:
	LogFrame* m_log;
	MainFrame* m_main;

    virtual bool OnInit();
};

static const u64 PS3_CLK = 3200000000;