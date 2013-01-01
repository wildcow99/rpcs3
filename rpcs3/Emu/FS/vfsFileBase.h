#pragma once
#include "vfsStream.h"

enum vfsFileOpenMode
{
	vfsFileRead,
	vfsFileWrite,
	vfsFileReadWrite,
	vfsFileWriteExcl,
	vfsFileWriteAppend,
};

struct vfsFileBase : public vfsStream
{
protected:
	wxString m_path;
	vfsFileOpenMode m_mode;

public:
	vfsFileBase();
	virtual ~vfsFileBase();

	virtual bool Open(const wxString& path, vfsFileOpenMode mode);
	virtual bool Close();

	wxString GetPath() const;
	vfsFileOpenMode GetOpenMode() const;
};