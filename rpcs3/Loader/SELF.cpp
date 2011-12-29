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
	if(!self_f.IsOpened()) return false;

	self_f.Seek(0);
	hdr.Load(self_f);
	if(!hdr.CheckMagic()) return false;
	
	//TODO
	ConLog.Error("Boot SELF not supported yet!");

	return false;
}