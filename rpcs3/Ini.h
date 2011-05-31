#pragma once

#include <wx/config.h>

class _Ini
{
private:
	wxConfigBase*	m_Config;

public:
	_Ini();
	virtual void Save(wxString key, int value);
	virtual void Save(wxString key, wxString value);

	virtual int Load(wxString key, int def_value);
	virtual wxString Load(wxString key, wxString def_value);
};

extern _Ini ini;