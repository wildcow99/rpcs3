#include "stdafx.h"
#include "Thread.h"
#include "Gui/ConLog.h"
#include <wx/listctrl.h>
#include "Ini.h"

LogWriter ConLog;
LogFrame* ConLogFrame;

struct LogData
{
	wxArrayString prefix;
	wxArrayString text;
	wxArrayString colour;
};

LogData ConLogData;

LogWriter::LogWriter()
{
	if(!m_logfile.Open(_PRGNAME_ ".log", wxFile::write))
	{
		ConLog.Error("Cann't create log file! (%s)", _PRGNAME_ ".log");
	}
}

void LogWriter::Task()
{
	/*
	const int cur_item_count = m_log.GetItemCount();
	static const uint max_item_count = 1000;

	if(cur_item_count > max_item_count)
	{
		for(uint i=0; i<cur_item_count-max_item_count; ++i) m_log.DeleteItem(0);
	}

	const int cur_item = m_log.GetItemCount();

	m_log.InsertItem(cur_item, m_prefix);
	m_log.SetItem(cur_item, 1, m_value);

	m_log.SetItemTextColour(cur_item, m_txtcolour);
	//log.SetItemBackgroundColour(cur_item, m_bgcolour);

	if(m_log.GetFocusedItem() == cur_item - 1) m_log.Focus(cur_item);
	*/
}

void LogWriter::WriteToLog(wxString prefix, wxString value, wxString colour/*, wxColour bgcolour*/)
{
	if(m_logfile.IsOpened())
		m_logfile.Write((prefix.IsEmpty() ? wxEmptyString : "[" + prefix + "]: ") + value + "\n");

	ConLogData.colour.Add(colour);
	ConLogData.prefix.Add(prefix);
	ConLogData.text.Add(value);

	//ConLogFrame->Post();
}

void LogWriter::Write(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);

	va_end(list);

	WriteToLog("!", frmt, "White");
}

void LogWriter::Error(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);
	
	va_end(list);

	WriteToLog("E", frmt, "Red");
}

void LogWriter::Warning(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);

	va_end(list);

	WriteToLog("W", frmt, "Yellow");
}

void LogWriter::SkipLn()
{
	WriteToLog(wxEmptyString, wxEmptyString, "Black");
}

LogFrame::LogFrame() 
	: wxFrame(NULL, wxID_ANY, "Log Console")
	, StepThread()
	, m_log(*new wxListView(this))
{
	runned = true;
	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );

	s_panel.Add(&m_log);

	m_log.InsertColumn(0, wxEmptyString);
	m_log.InsertColumn(1, "Log");
	m_log.SetBackgroundColour(wxColour("Black"));

	m_log.SetColumnWidth(0, 18);

	SetSizerAndFit( &s_panel );

	SetSize(Ini.Gui.m_LogWindow.GetValue().size);
	SetPosition(Ini.Gui.m_LogWindow.GetValue().position);

	Connect( wxEVT_SIZE, wxSizeEventHandler(LogFrame::OnResize) );
	Connect( m_log.GetId(), wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, wxListEventHandler( LogFrame::OnColBeginDrag ));

	Show();
	StepThread::Start(true);
	StepThread::DoStep();
}

LogFrame::~LogFrame()
{
	Ini.Gui.m_LogWindow.SetValue(WindowInfo(GetSize(), GetPosition()));
	StepThread::Exit();
	runned = false;
}

void LogFrame::Post()
{
	if(!runned) m_sem_waitdata.Post();
}

void LogFrame::Step()
{
	//runned = false;

	for(;;)
	{
		while
		(
			ConLogData.colour.GetCount() == 0 &&
			ConLogData.prefix.GetCount() == 0 &&
			ConLogData.text.  GetCount() == 0
		)
		{
			//static uint count = 0;
			ThreadAdv::Sleep(200);
			/*
			if(++count > 5)
			{
				runned = false;
				count = 0;
				m_sem_waitdata.Wait();
				runned = true;
			}
			*/
		}
		
		if
		(
			ConLogData.colour.GetCount() == 0 ||
			ConLogData.prefix.GetCount() == 0 ||
			ConLogData.text.GetCount()   == 0
		)
		{
			while 
			(
				ConLogData.colour.GetCount() == 0 ||
				ConLogData.prefix.GetCount() == 0 ||
				ConLogData.text.GetCount()   == 0
			) ThreadAdv::Sleep(50);
		}

		if(!runned)
		{
			ThreadAdv::Exit();
		}

		StepThread::SetCancelState(false);
		{
			const wxColour colour = wxColour(ConLogData.colour[0]);
			const wxString prefix = ConLogData.prefix[0];
			const wxString text = ConLogData.text[0];

			ConLogData.colour.RemoveAt(0);
			ConLogData.prefix.RemoveAt(0);
			ConLogData.text.RemoveAt(0);

			const int cur_item_count = m_log.GetItemCount();
			static const uint max_item_count = 1000;

			if(cur_item_count > max_item_count)
			{
				for(uint i=0; i<cur_item_count-max_item_count; ++i) m_log.DeleteItem(0);
			}

			const int cur_item = m_log.GetItemCount();

			m_log.InsertItem(cur_item, prefix);
			m_log.SetItem(cur_item, 1, text);

			m_log.SetItemTextColour(cur_item, colour);
			//log.SetItemBackgroundColour(cur_item, m_bgcolour);

			if(m_log.GetFocusedItem() == cur_item - 1) m_log.Focus(cur_item);
			
		}
		StepThread::SetCancelState(true);
	}

	ThreadAdv::Sleep(1);
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