#include "stdafx.h"
#include "vfsFileBase.h"

vfsFileBase::vfsFileBase() : vfsStream()
{
}

vfsFileBase::~vfsFileBase()
{
	Close();
}

bool Access(const wxString& path, vfsFileOpenMode mode)
{
	return false;
}

bool vfsFileBase::Open(const wxString& path, vfsFileOpenMode mode)
{
	m_path = path;
	m_mode = mode;

	vfsStream::Reset();

	return true;
}

bool vfsFileBase::Close()
{
	m_path = wxEmptyString;

	return vfsStream::Close();
}

wxString vfsFileBase::GetPath() const
{
	return m_path;
}

vfsFileOpenMode vfsFileBase::GetOpenMode() const
{
	return m_mode;
}