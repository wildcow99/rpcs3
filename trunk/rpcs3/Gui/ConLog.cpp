#include "stdafx.h"

#include "Utilites/Thread.h"
#include "Gui/ConLog.h"
#include <wx/listctrl.h>
#include "Ini.h"

LogWriter ConLog;
LogFrame* ConLogFrame;

static const uint max_item_count = 500;

struct LogData
{
	wxString m_prefix;
	wxString m_text;
	wxString m_colour;

	LogData(const wxString& prefix, const wxString& text, const wxString& colour)
		: m_prefix(prefix)
		, m_text(text)
		, m_colour(colour)
	{
	}

	~LogData()
	{
		m_prefix.Clear();
		m_text.Clear();
		m_colour.Clear();
	}
};

Array<LogData> ConLogData;

LogWriter::LogWriter()
{
	if(!m_logfile.Open(_PRGNAME_ ".log", wxFile::write))
	{
		wxMessageBox("Cann't create log file! (" _PRGNAME_ ".log)", wxMessageBoxCaptionStr, wxICON_ERROR);
	}
}

void LogWriter::WriteToLog(wxString prefix, wxString value, wxString colour/*, wxColour bgcolour*/)
{
	if(m_logfile.IsOpened())
		m_logfile.Write((prefix.IsEmpty() ? wxEmptyString : "[" + prefix + "]: ") + value + "\n");

	if(!ConLogFrame || ConLogFrame->IsBeingDeleted() || !ConLogFrame->IsShown()) return;
	
	if(ConLogData.GetCount() > max_item_count) ConLogData.RemoveAt(0, ConLogData.GetCount()-max_item_count);
	ConLogData.Add(new LogData(prefix, value, colour));

	ConLogFrame->DoStep();
}

wxString FormatV(const wxString fmt, va_list args)
{
	int length = 256;
	wxString str;
	
	for(;;)
	{
		str.Clear();
		wxStringBuffer buf(str, length+1);
		memset(buf, 0, length+1);
		if(vsnprintf(buf, length, fmt, args) != -1) break;
		length *= 2;
	}

	return str;
}

void LogWriter::Write(const wxString fmt, ...)
{
	va_list list;
	va_start(list, fmt);

	const wxString& frmt = FormatV(fmt, list);

	va_end(list);

	WriteToLog("!", frmt, "White");
}

void LogWriter::Error(const wxString fmt, ...)
{
	va_list list;
	va_start(list, fmt);

	const wxString& frmt = FormatV(fmt, list);

	va_end(list);

	WriteToLog("E", frmt, "Red");
}

void LogWriter::Warning(const wxString fmt, ...)
{
	va_list list;
	va_start(list, fmt);

	const wxString& frmt = FormatV(fmt, list);

	va_end(list);

	WriteToLog("W", frmt, "Yellow");
}

void LogWriter::SkipLn()
{
	WriteToLog(wxEmptyString, wxEmptyString, "Black");
}

BEGIN_EVENT_TABLE(LogFrame, FrameBase)
	EVT_CLOSE(LogFrame::OnQuit)
END_EVENT_TABLE()

LogFrame::LogFrame()
	: FrameBase(NULL, wxID_ANY, "Log Console", wxEmptyString, wxSize(600, 450), wxDefaultPosition)
	, StepThread()
	, m_log(*new wxListView(this))
{
	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );

	s_panel.Add(&m_log);

	m_log.InsertColumn(0, wxEmptyString);
	m_log.InsertColumn(1, "Log");
	m_log.SetBackgroundColour(wxColour("Black"));

	m_log.SetColumnWidth(0, 18);

	SetSizerAndFit( &s_panel );

	Connect( wxEVT_SIZE, wxSizeEventHandler(LogFrame::OnResize) );
	Connect( m_log.GetId(), wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, wxListEventHandler( LogFrame::OnColBeginDrag ));

	Show();
	StepThread::Start(true);
	StepThread::DoStep();
}

LogFrame::~LogFrame()
{
}

bool LogFrame::Close(bool force)
{
	ConLogFrame = NULL;
	StepThread::Exit();
	ConLogData.Clear();

	return FrameBase::Close(force);
}

void LogFrame::Step()
{
	if(ConLogData.GetCount() == 0) return;
	
	while(ConLogData.GetCount())
	{
		StepThread::SetCancelState(false);
		if(ConLogData.GetCount() > max_item_count)
		{
			ConLogData.RemoveAt(0, ConLogData.GetCount() - max_item_count);
		}

		const wxColour colour = wxColour(ConLogData[0].m_colour);
		const wxString prefix = ConLogData[0].m_prefix;
		const wxString text = ConLogData[0].m_text;
		ConLogData.RemoveAt(0);

		int cur_item = m_log.GetItemCount();
		if(cur_item > max_item_count)
		{
			for(uint i=0; i<cur_item-max_item_count; ++i) m_log.DeleteItem(0);
			cur_item = m_log.GetItemCount();
		}

		m_log.InsertItem(cur_item, prefix);
		m_log.SetItem(cur_item, 1, text);
		m_log.SetItemTextColour(cur_item, colour);
		::SendMessage((HWND)m_log.GetHWND(), WM_VSCROLL, SB_PAGEDOWN, 0);
		StepThread::SetCancelState(true);
		ThreadAdv::TestCancel();
		Sleep(1);
	}
	Sleep(1);
}

void LogFrame::OnColBeginDrag(wxListEvent& event)
{
	event.Veto();
}

void LogFrame::OnResize(wxSizeEvent& WXUNUSED(event))
{
	const wxSize size( GetClientSize() );

	m_log.SetSize( size );
	m_log.SetColumnWidth(1, size.GetWidth() - 4 - m_log.GetColumnWidth(0));
}

void LogFrame::OnQuit(wxCloseEvent& event)
{
	ConLogFrame = NULL;
	StepThread::Exit();
	ConLogData.Clear();
	event.Skip();
}