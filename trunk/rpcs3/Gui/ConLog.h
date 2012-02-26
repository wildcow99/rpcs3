#pragma once
#include "Thread.h"
#include <wx/listctrl.h>
#include "Ini.h"
#include "Gui/FrameBase.h"

class LogFrame 
	: public FrameBase
	, public StepThread
{
	wxListView& m_log;

public:
	LogFrame();
	~LogFrame();

	bool Close(bool force = false);

private:
	virtual void OnColBeginDrag(wxListEvent& event);
	virtual void OnResize(wxSizeEvent& event);
	virtual void Step();
};

class LogWriter
{
	wxFile m_logfile;
	wxColour m_txtcolour;

	wxString m_prefix;
	wxString m_value;

	virtual void WriteToLog(wxString prefix, wxString value, wxString colour);

public:
	LogWriter();

	virtual void Write(const wxString fmt, ...);
	virtual void Error(const wxString fmt, ...);
	virtual void Warning(const wxString fmt, ...);
	virtual void SkipLn();
};

extern LogWriter ConLog;
extern LogFrame* ConLogFrame;
