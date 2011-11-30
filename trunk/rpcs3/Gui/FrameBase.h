#pragma once

class FrameBase : public wxFrame
{
protected:
	IniEntry<WindowInfo> m_ini;
	WindowInfo m_default_info;

	FrameBase(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxString& framename = "UnknownFrame",
		const wxString& ininame = wxEmptyString,
		wxSize defsize = wxDefaultSize,
		wxPoint defposition = wxDefaultPosition,
		long style = wxDEFAULT_FRAME_STYLE)
		: wxFrame(parent, id, framename, defposition, defsize, style)
		, m_default_info(defsize, defposition)
	{
		m_ini.Init(ininame.IsEmpty() ? framename : ininame, "GuiSettings");
		LoadInfo();
	}

	~FrameBase()
	{
		m_ini.SetValue(WindowInfo(GetSize(), GetPosition()));
		m_ini.Save();
	}

	void SetSizerAndFit(wxSizer *sizer, bool deleteOld = true, bool loadinfo = true)
	{
		wxFrame::SetSizerAndFit(sizer, deleteOld);
		if(loadinfo) LoadInfo();
	}

	void LoadInfo()
	{
		const WindowInfo& info = m_ini.LoadValue(m_default_info);
		SetSize(info.size);
		SetPosition(info.position);
	}
};