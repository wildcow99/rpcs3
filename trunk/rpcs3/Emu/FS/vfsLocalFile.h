#pragma once
#include "vfsFileBase.h"

class vfsLocalFile : public vfsFileBase
{
private:
	wxFile m_file;

public:
	vfsLocalFile();
	vfsLocalFile(const wxString path, vfsFileOpenMode mode = vfsFileRead);

	static bool Access(const wxString& path, vfsFileOpenMode mode = vfsFileRead);
	virtual bool Open(const wxString& path, vfsFileOpenMode mode = vfsFileRead);
	virtual bool Close();

	virtual u64 GetSize();

	virtual u32 Write(const void* src, u32 size);
	virtual u32 Read(void* dst, u32 size);

	virtual u64 Seek(s64 offset, vfsSeekMode mode = vfsSeekSet);
	virtual u64 Tell() const;

	virtual bool IsOpened() const;
};