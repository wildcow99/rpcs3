#pragma once

#include <wx/config.h>

struct WindowInfo
{
	wxSize size;
	wxPoint position;

	WindowInfo() {}
	WindowInfo(const wxSize _size, const wxPoint _position = wxDefaultPosition)
		: size(_size)
		, position(_position)
	{
	}

	static WindowInfo& GetDefault()
	{
		return *new WindowInfo(wxDefaultSize, wxDefaultPosition);
	}
};

class Ini
{
protected:
	wxConfigBase*	m_Config;

	Ini();
	virtual void Save(wxString key, int value);
	virtual void Save(wxString key, bool value);
	virtual void Save(wxString key, wxSize value);
	virtual void Save(wxString key, wxPoint value);
	virtual void Save(wxString key, wxString value);
	virtual void Save(wxString key, WindowInfo value);

	virtual int Load(wxString key, int def_value);
	virtual bool Load(wxString key, bool def_value);
	virtual wxSize Load(wxString key, wxSize def_value);
	virtual wxPoint Load(wxString key, wxPoint def_value);
	virtual wxString Load(wxString key, wxString def_value);
	virtual WindowInfo Load(wxString key, WindowInfo def_value);
};

template<typename T> struct IniEntry : public Ini
{
	T m_value;
	wxString m_key;

	IniEntry() : Ini()
	{
	}

	void Init(const wxString key, const wxString patch)
	{
		m_key = key;
		m_Config->SetPath(patch);
	}

	void SetValue(const T value)
	{
		m_value = value;
	}

	T GetValue()
	{
		return m_value;
	}

	T LoadValue(const T defvalue)
	{
		return Ini::Load(m_key, defvalue)
	}

	void Save()
	{
		Ini::Save(m_key, m_value);
	}

	T Load(const T defvalue)
	{
		return (m_value = Ini::Load(m_key, defvalue));
	}
};

class EmuIni
{
private:
	const wxString m_DefPath;
	const wxString m_VideoPath;
	const wxString m_MainPath;

public:
	IniEntry<int> m_DecoderMode;
	IniEntry<int> m_RenderMode;

public:
	EmuIni()
		: m_DefPath("EmuSettings")
		, m_MainPath(m_DefPath + "\\" + "Main")
		, m_VideoPath(m_DefPath + "\\" + "Video")
	{
		m_DecoderMode.Init("DecoderMode", m_MainPath);
		m_RenderMode.Init("RenderMode", m_VideoPath);
	}

	void Load()
	{
		m_DecoderMode.Load(1);
		m_RenderMode.Load(1);
	}

	void Save()
	{
		m_DecoderMode.Save();
		m_RenderMode.Save();
	}
};

class GuiIni
{
private:
	const wxString  m_DefPath;

public:
	IniEntry<WindowInfo> m_MainWindow;
	IniEntry<WindowInfo> m_LogWindow;

public:
	GuiIni() : m_DefPath("GuiSettings")
	{
		m_MainWindow.Init("MainWindow", m_DefPath);
		m_LogWindow.Init("LogWindow", m_DefPath);
	}

	void Load()
	{
		m_MainWindow.Load(WindowInfo(wxSize(280, 180)));
		m_LogWindow.Load(WindowInfo(wxSize(600, 450)));
	}

	void Save()
	{
		m_MainWindow.Save();
		m_LogWindow.Save();
	}
};

class Inis
{
public:
	EmuIni Emu;
	GuiIni Gui;

	void Load()
	{
		Emu.Load();
		Gui.Load();
	}

	void Save()
	{
		Emu.Save();
		Gui.Save();
	}
};

extern Inis Ini;