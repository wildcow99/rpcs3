#include "stdafx.h"
#include "Ini.h"

#include <wx/msw/iniconf.h>

_Ini ini;

_Ini::_Ini()
{
	m_Config = new wxIniConfig( wxEmptyString, wxEmptyString, wxGetCwd() + "\\rpcs3.ini", wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	m_Config->SetPath("EmuSettings");
}

void _Ini::Save(wxString key, int value)
{
	m_Config->Write(key, value);
}

void _Ini::Save(wxString key, wxString value)
{
	m_Config->Write(key, value);
}

int _Ini::Load(wxString key, int def_value)
{
	return m_Config->Read(key, def_value);
}

wxString _Ini::Load(wxString key, wxString def_value)
{
	return m_Config->Read(key, def_value);
}