#pragma once
#include "Thread.h"
#include <wx/listctrl.h>

class LogWriter
{
	wxFile m_logfile;

	wxColour m_bgcolour;
	wxColour m_txtcolour;

	wxString m_prefix;
	wxString m_value;

	virtual void Task();

	virtual void WriteToLog(wxString prefix, wxString value, wxColour colour/*, wxColour bgcolour = wxColour(L"Black")*/);

public:
	LogWriter()
	{
	}

	virtual void Init();

	virtual void Write(const wxString fmt, ...);
	virtual void Error(const wxString fmt, ...);
	virtual void Warning(const wxString fmt, ...);
	virtual void SkipLn();
};

extern LogWriter ConLog;
extern wxListView* m_log;

class LogFrame : public wxFrame
{
public:
	LogFrame() : wxFrame(NULL, wxID_ANY, "Log Console")
	{
		m_log = new wxListView(this);

		wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );

		s_panel.Add(m_log);

		m_log->InsertColumn(0, wxEmptyString);
		m_log->InsertColumn(1, "Log");
		m_log->SetBackgroundColour(wxColour("Black"));

		m_log->SetColumnWidth(0, 15);

		SetSizerAndFit( &s_panel );

		SetSize(wxSize(600, 450));

		Connect( wxEVT_SIZE, wxSizeEventHandler(LogFrame::OnResize) );
		Connect( m_log->GetId(), wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, wxListEventHandler( LogFrame::OnColBeginDrag ));
	}

	virtual void OnColBeginDrag(wxListEvent& event)
	{
		event.Veto();
	}

	virtual void OnResize(wxSizeEvent& WXUNUSED(event))
	{
		const wxSize size( GetClientSize() );

		m_log->SetSize( size );
		m_log->SetColumnWidth(1, size.GetWidth() - 4 - m_log->GetColumnWidth(0));
	}
};