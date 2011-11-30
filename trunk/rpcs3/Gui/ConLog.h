#pragma once
#include "Thread.h"
#include <wx/listctrl.h>
#include "Ini.h"
#include "Gui/FrameBase.h"

class LogFrame 
	: public FrameBase
	, public StepThread
{
	Semaphore m_sem_waitdata;

public:
	LogFrame();
	~LogFrame();
	virtual void Post();

	bool runned;

private:
	wxListView& m_log;

	virtual void OnColBeginDrag(wxListEvent& event);
	virtual void OnResize(wxSizeEvent& event);
	virtual void Step();
};

class LogWriter
{
	wxFile m_logfile;

	wxColour m_bgcolour;
	wxColour m_txtcolour;

	wxString m_prefix;
	wxString m_value;

	virtual void Task();

	virtual void WriteToLog(wxString prefix, wxString value, wxString colour/*, wxColour bgcolour = wxColour(L"Black")*/);

public:
	LogWriter();

	virtual void Write(const wxString fmt, ...);
	virtual void Error(const wxString fmt, ...);
	virtual void Warning(const wxString fmt, ...);
	virtual void SkipLn();
};

extern LogWriter ConLog;
extern LogFrame* ConLogFrame;
