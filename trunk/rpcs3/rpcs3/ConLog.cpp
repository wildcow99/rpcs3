#include "stdafx.h"
#include "Thread.h"
#include "ConLog.h"
#include <wx/listctrl.h>

LogPanel ConLog;

void LogPanel::Init()
{
	m_logfile.Create(_PRGNAME_ L".log", true);
	if(!m_logfile.IsOpened())
	{
		ConLog.Error(L"Cann'ot create log file! (%s)", _PRGNAME_ L".log");
	}

	SetName(L"Logger");
	Start();
}

void LogPanel::ExecuteTaskInThread()
{

	static wxListView& log = *GetApp().frame->m_log;

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

	if(log.GetFocusedItem() == cur_item -1)
	{
		log.Focus(cur_item);
	}
}

void LogPanel::WriteToLog(wxString prefix, wxString value, wxColour colour/*, wxColour bgcolour*/)
{
	m_logfile.Write(value + L"\n");

	WaitForResult();

	m_txtcolour = colour;
	//m_bgcolour = bgcolour;

	m_prefix = prefix;
	m_value = value;

	Start();
}

void LogPanel::Write(const wxChar* fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	FastFormatUnicode frmt;
	frmt.WriteV(fmt, list);

	va_end(list);

	WriteToLog(L"!", frmt, wxColour(L"White"));
}

void LogPanel::Write(const char* fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	FastFormatUnicode frmt;
	frmt.WriteV(fmt, list);

	va_end(list);

	WriteToLog(L"!", frmt, wxColour(L"White"));
}

void LogPanel::Error(const wxChar* fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	FastFormatUnicode frmt;
	frmt.WriteV(fmt, list);

	va_end(list);

	WriteToLog(L"E", frmt, wxColour(L"Red"));
}

void LogPanel::Warning(const wxChar* fmt, ...)
{
	va_list list;

	va_start(list, fmt);

	FastFormatUnicode frmt;
	frmt.WriteV(fmt, list);

	va_end(list);

	WriteToLog(L"W", frmt, wxColour(L"Yellow"));
}

void LogPanel::SkipLn()
{
	WriteToLog(wxEmptyString, wxEmptyString, wxColour(L"Black"));
}
