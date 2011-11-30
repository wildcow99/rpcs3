#include "stdafx.h"
#include "SELF.h"

SELFLoader::SELFLoader(wxFile& f)
	: self_f(f)
	, LoaderBase()
{
}

SELFLoader::SELFLoader(const wxString& path)
	: self_f(*new wxFile(path))
	, LoaderBase()
{
}

bool SELFLoader::Load()
{
	//TODO
	ConLog.Error("Boot SELF not supported yet!");

	return false;
}