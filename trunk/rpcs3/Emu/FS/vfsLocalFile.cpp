#include "stdafx.h"
#include "vfsLocalFile.h"

static const wxFile::OpenMode vfs2wx_mode(vfsFileOpenMode mode)
{
	switch(mode)
	{
	case vfsFileRead:			return wxFile::read;
	case vfsFileWrite:			return wxFile::write;
	case vfsFileReadWrite:		return wxFile::read_write;
	case vfsFileWriteExcl:		return wxFile::write_excl;
	case vfsFileWriteAppend:	return wxFile::write_append;
	}

	return wxFile::read;
}

static const wxSeekMode vfs2wx_seek(vfsSeekMode mode)
{
	switch(mode)
	{
	case vfsSeekSet: return wxFromStart;
	case vfsSeekCur: return wxFromCurrent;
	case vfsSeekEnd: return wxFromEnd;
	}

	return wxFromStart;
}

vfsLocalFile::vfsLocalFile() : vfsFileBase()
{
}

vfsLocalFile::vfsLocalFile(const wxString path, vfsFileOpenMode mode) : vfsFileBase()
{
	Open(path, mode);
}

bool vfsLocalFile::Access(const wxString& path, vfsFileOpenMode mode)
{
	return wxFile::Access(path, vfs2wx_mode(mode));
}

bool vfsLocalFile::Open(const wxString& path, vfsFileOpenMode mode)
{
	Close();

	return m_file.Open(path, vfs2wx_mode(mode)) && vfsFileBase::Open(path, mode);
}

bool vfsLocalFile::Close()
{
	return m_file.Close() && vfsFileBase::Close();
}

u64 vfsLocalFile::GetSize()
{
	return m_file.Length();
}

u32 vfsLocalFile::Write(const void* src, u32 size)
{
	return m_file.Write(src, size);
}

u32 vfsLocalFile::Read(void* dst, u32 size)
{
	return m_file.Read(dst, size);
}

u64 vfsLocalFile::Seek(s64 offset, vfsSeekMode mode)
{
	return m_file.Seek(offset, vfs2wx_seek(mode));
}

u64 vfsLocalFile::Tell() const
{
	return m_file.Tell();
}

bool vfsLocalFile::IsOpened() const
{
	return m_file.IsOpened() && vfsFileBase::IsOpened();
}