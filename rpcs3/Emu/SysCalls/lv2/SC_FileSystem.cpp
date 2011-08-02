#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

enum Lv2FsOflag
{
	LV2_O_RDONLY	= 0x000000,
	LV2_O_WRONLY	= 0x000001,
	LV2_O_RDWR		= 0x000002,
	LV2_O_ACCMODE	= 0x000003,
	LV2_O_CREAT		= 0x000100,
	LV2_O_EXCL		= 0x000200,
	LV2_O_TRUNC		= 0x001000,
	LV2_O_APPEND	= 0x002000,
	LV2_O_MSELF		= 0x010000,
};

#pragma pack(1)
struct Lv2FsStat
{
	s32 st_mode;
	s32 st_uid;
	s32 st_gid;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	u64 st_size;
	u64 st_blksize;
};
#pragma pack()

struct Lv2FsUtimbuf
{
	time_t actime;
	time_t modtime;
};

struct Lv2FsDirent
{
	u8 d_type;
	u8 d_namlen;
	char d_name[256];
};

class FSFiles : private SysCallBase
{
	SysCallsArraysList<wxFile> fs_files;

public:
	FSFiles() : SysCallBase("FSFiles")
	{
	}

	u64 Open(const wxString& patch, const s32 oflags, const s32 mode)
	{
		const uint id = fs_files.Add();
		wxFile::OpenMode o_mode;

		if(oflags & LV2_O_RDONLY)
		{
			o_mode = wxFile::read;
		}
		else if(oflags & LV2_O_RDWR)
		{
			o_mode = wxFile::read_write;
		}
		else if(oflags & LV2_O_WRONLY)
		{
			if(oflags & LV2_O_EXCL)
			{
				o_mode = wxFile::write_excl;
			}
			else if(oflags & LV2_O_APPEND)
			{
				o_mode = wxFile::write_append;
			}
			else
			{
				o_mode = wxFile::write;
			}
		}
		else
		{
			ConLog.Error("%s error: '%s' has unknown flags! flags: 0x%08x", module_name, patch, oflags);
			return -1;
		}

		if(o_mode == wxFile::read && !wxFile::Access(patch, wxFile::read))
		{
			ConLog.Error("%s error: %s not found! flags: 0x%08x", module_name, patch, oflags);
			return -1;
		}
		
		fs_files.GetDataById(id)->Open(patch, o_mode);
		return id;
	}

	ssize_t Read(const u64 id, void* buf, const u64 size)
	{
		bool error;
		wxFile& data = *fs_files.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return 0;
		}

		return data.Read(buf, size);
	}

	size_t Write(const u64 id, const void* buf, const u64 size)
	{
		bool error;
		wxFile& data = *fs_files.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return 0;
		}

		return data.Write(buf, size);
	}

	wxFileOffset Seek(const u64 id, const wxFileOffset offset, const u32 whence)
	{
		bool error;
		wxFile& data = *fs_files.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return 0;
		}
		return data.Seek(offset);
	}

	void Close(const u64 id)
	{
		bool error;
		wxFile& data = *fs_files.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return;
		}
		data.Close();
		fs_files.RemoveById(id);
	}
};

FSFiles fs_files;

class FSDirs : private SysCallBase
{
	SysCallsArraysList<wxDir> fs_dirs;

public:
	FSDirs() : SysCallBase("FSDirs")
	{
	}

	u64 Open(const wxString& patch)
	{
		const u64 id = fs_dirs.Add();

		if(wxDirExists(patch))
		{
			ConLog.Error("%s error: %s not found!", module_name, patch);
			return 0;
		}
		
		fs_dirs.GetDataById(id)->Open(patch);
		return id;
	}

	size_t Read(const u64 id, Lv2FsDirent* fs_dirent)
	{
		wxArrayString files;

		bool error;
		const wxDir& data = *fs_dirs.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			fs_dirent = NULL;
			return 0;
		}

		const size_t ret = wxDir::GetAllFiles(data.GetName(), &files);

		fs_dirent = new Lv2FsDirent[files.GetCount()];

		for(uint i=0; i<files.GetCount(); ++i)
		{
			fs_dirent[i].d_namlen = files[i].Length() < 256 ? files[i].Length() : 255;

			for(uint a=0; a<fs_dirent[i].d_namlen; ++a)
			{
				fs_dirent[i].d_name[a] = files[i][a];
			}

			fs_dirent[i].d_type = 1;//???
		}

		return ret;
	}

	void Close(const u64 id)
	{
		if(!fs_dirs.RemoveById(id))
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return;
		}
	}
};

FSDirs fs_dirs;

int lv2FsOpen()
{
	const wxString& patch = Memory.MemFlags.SearchV(CPU.GPR[3]);
	const s32 oflags = CPU.GPR[4];
	s64& fs_file = CPU.GPR[5];

	const s32 mode = CPU.GPR[6];
	const void* arg = (void*)&CPU.GPR[7];//???
	const s32 argcount = CPU.GPR[8];

	fs_file = fs_files.Open(FixPatch(patch), oflags, mode);
	return 0;
}

int lv2FsRead()
{
	const u32 fs_file = CPU.GPR[3];
	void* buf = (void*)&CPU.GPR[4];
	const u64 size = CPU.GPR[5];
	s64& read = CPU.GPR[6];

	read = fs_files.Read(fs_file, buf, size);
	return 0;
}

int lv2FsWrite()
{
	const u32 fs_file = CPU.GPR[3];
	const void* buf = (void*)&CPU.GPR[4];
	const u64 size = CPU.GPR[5];
	s64& written = CPU.GPR[6];

	written = fs_files.Write(fs_file, buf, size);
	return 0;
}

int lv2FsClose()
{
	const u32 fs_file = CPU.GPR[3];

	fs_files.Close(fs_file);
	return 0;
}

int lv2FsOpenDir()
{
	const wxString& patch = (char*)&CPU.GPR[3];
	s64& fs_file = CPU.GPR[4];

	fs_file = fs_dirs.Open(FixPatch(patch));
	return 0;
}

int lv2FsReadDir()
{
	s64 fs_file = CPU.GPR[3];
	Lv2FsDirent* fs_dirent = (Lv2FsDirent*)&CPU.GPR[4];
	s64& read = CPU.GPR[5];

	read = fs_dirs.Read(fs_file, fs_dirent);
	return 0;
}

int lv2FsCloseDir()
{
	s64 fs_file = CPU.GPR[3];

	fs_dirs.Close(fs_file);
	return 0;
}

int lv2FsMkdir()
{
	const wxString& patch = FixPatch((char*)&CPU.GPR[3]);
	const u32 mode = CPU.GPR[4];//???

	if(wxDirExists(patch)) return -1;

	wxMkdir(patch);
	return 0;
}

int lv2FsRename()
{
	const wxString& patch = (char*)&CPU.GPR[3];
	const wxString& newpatch = (char*)&CPU.GPR[4];

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsRename error: %s not found", patch);
		return -1;
	}

	wxRenameFile(FixPatch(patch), FixPatch(newpatch));
	return 0;
}

int lv2FsLSeek64()
{
	const u32 fs_file = CPU.GPR[3];
	const s64 offset = CPU.GPR[4];
	const s32 whence = CPU.GPR[5];
	s64& position = CPU.GPR[6];

	position = fs_files.Seek(fs_file, offset, whence);
	return 0;
}

int lv2FsRmdir()
{
	const wxString& patch = FixPatch((char*)&CPU.GPR[3]);

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsRmdir error: %s not found", patch);
		return -1;
	}

	wxFileName::Rmdir(patch);
	return 0;
}

int lv2FsUtime()
{
	const wxString& patch = FixPatch((char*)&CPU.GPR[3]);
	Lv2FsUtimbuf& times = *(Lv2FsUtimbuf*)&CPU.GPR[4];

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsUtime error: %s not found", patch);
		return -1;
	}

	wxDateTime actime, modtime;
    wxFileName(patch).GetTimes(&actime, &modtime, NULL);

	times.modtime = modtime.GetTicks();
	times.actime = actime.GetTicks();
	return 0;
}