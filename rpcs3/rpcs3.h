#pragma once

#include "Gui/ConLog.h"
#include "Gui/MainFrame.h"

class TheApp : public wxApp
{
public:
	LogFrame* m_log;
	MainFrame* m_main;

    virtual bool OnInit();
};