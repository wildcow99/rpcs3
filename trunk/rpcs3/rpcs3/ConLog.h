#pragma once

class LogPanel : public Thread
{
	wxFile m_logfile;

	wxColour m_bgcolour;
	wxColour m_txtcolour;

	wxString m_prefix;
	wxString m_value;

	virtual void Task();

	virtual void WriteToLog(wxString prefix, wxString value, wxColour colour/*, wxColour bgcolour = wxColour(L"Black")*/);

public:
	virtual void Init();

	virtual void Write(const wxChar* fmt, ...);
	virtual void Write(const char* fmt, ...);
	virtual void Error(const wxChar* fmt, ...);
	virtual void Warning(const wxChar* fmt, ...);
	virtual void SkipLn();
};

extern LogPanel ConLog;