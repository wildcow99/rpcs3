#include "stdafx.h"
#include "Thread.h"
#include "Gui/ConLog.h"
#include <wx/listctrl.h>

LogWriter ConLog;
wxListView* m_log;

void LogWriter::Init()
{
	m_logfile.Create(_PRGNAME_ ".log", true);
	if(!m_logfile.IsOpened())
	{
		ConLog.Error("Cann't create log file! (%s)", _PRGNAME_ ".log");
	}
}

void LogWriter::Task()
{
	if(m_log == NULL) return;
	static wxListView& log = *m_log;

	const int cur_item_count = log.GetItemCount();
	static const uint max_item_count = 1000;

	if(cur_item_count > max_item_count)
	{
		for(uint i=0; i<cur_item_count-max_item_count; ++i) log.DeleteItem(0);
	}

	const int cur_item = log.GetItemCount();

	log.InsertItem(cur_item, m_prefix);
	log.SetItem(cur_item, 1, m_value);

	log.SetItemTextColour(cur_item, m_txtcolour);
	//log.SetItemBackgroundColour(cur_item, m_bgcolour);

	if(log.GetFocusedItem() == cur_item - 1)
	{
		log.Focus(cur_item);
	}
}

void LogWriter::WriteToLog(wxString prefix, wxString value, wxColour colour/*, wxColour bgcolour*/)
{
	m_logfile.Write((prefix.IsEmpty() ? wxEmptyString : "[" + prefix + "]: ") + value + "\n");

	Thread::WaitForResult();

	m_txtcolour = colour;
	//m_bgcolour = bgcolour;

	m_prefix = prefix;
	m_value = value;

	Thread::Start();
}

void LogWriter::Write(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);

	va_end(list);

	WriteToLog("!", frmt, wxColour("White"));
}

void LogWriter::Error(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);

	va_end(list);

	WriteToLog("E", frmt, wxColour("Red"));
}

void LogWriter::Warning(const wxString fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	wxString frmt;
	frmt.PrintfV(fmt, list);

	va_end(list);

	WriteToLog("W", frmt, wxColour("Yellow"));
}

void LogWriter::SkipLn()
{
	WriteToLog(wxEmptyString, wxEmptyString, wxColour("Black"));
}
