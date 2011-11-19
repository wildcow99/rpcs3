#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

typedef u64 GPRtype;
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

enum Lv2FsSeek
{
	LV2_SEEK_SET,
	LV2_SEEK_CUR,
	LV2_SEEK_END,
};

enum Lv2FsLength
{
	LV2_MAX_FS_PATH_LENGTH = 1024,
	LV2_MAX_FS_FILE_NAME_LENGTH = 255,
	LV2_MAX_FS_MP_LENGTH = 31,
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
	char d_name[LV2_MAX_FS_FILE_NAME_LENGTH + 1];
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

		s32 _oflags = oflags;
		if(oflags & LV2_O_RDONLY)
		{
			_oflags &= ~LV2_O_RDONLY;
			o_mode = wxFile::read;
		}
		else if(oflags & LV2_O_RDWR)
		{
			_oflags &= ~LV2_O_RDWR;
			o_mode = wxFile::read_write;
		}
		else if(oflags & LV2_O_WRONLY)
		{
			_oflags &= ~LV2_O_WRONLY;

			if(oflags & LV2_O_EXCL)
			{
				_oflags &= ~LV2_O_EXCL;
				o_mode = wxFile::write_excl;
			}
			else if(oflags & LV2_O_APPEND)
			{
				_oflags &= ~LV2_O_APPEND;
				o_mode = wxFile::write_append;
			}
			else
			{
				o_mode = wxFile::write;
			}
		}
		else
		{
			ConLog.Error("%s[%d] error: '%s' has unknown flags! flags: 0x%08x", module_name, id, patch, oflags);
			return -1;
		}

		if(_oflags != 0)
		{
			ConLog.Error("%s[%d] error: '%s' has unknown flags! flags: 0x%08x", module_name, id, patch, oflags);
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

		static wxSeekMode seek_mode = wxFromStart;
		switch(whence)
		{
			case LV2_SEEK_SET: seek_mode = wxFromStart; break;
			case LV2_SEEK_CUR: seek_mode = wxFromCurrent; break;
			case LV2_SEEK_END: seek_mode = wxFromEnd; break;
			default:
				ConLog.Error("%s[%d] error: Unknown seek whence! (%d)", module_name, id, whence);
				return 0;
		};
		return data.Seek(offset, seek_mode);
	}

	bool Close(const u64 id)
	{
		bool error;
		wxFile& data = *fs_files.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return false;
		}
		data.Close();
		return fs_files.RemoveById(id);
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

	bool Close(const u64 id)
	{
		if(!fs_dirs.RemoveById(id))
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return false;
		}

		return true;
	}
};

FSDirs fs_dirs;

wxString ReadStringInMem(const u32 addr)
{
	return FixPatch(Memory.ReadString(addr));
}

int SysCalls::lv2FsOpen()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);
	const s32 oflags = CPU.GPR[4];
	GPRtype& fs_file = CPU.GPR[5];

	const s32 mode = CPU.GPR[6];
	const void* arg = Memory.GetMemFromAddr(CPU.GPR[7]);//???
	const s32 argcount = CPU.GPR[8];

	ConLog.Write("lv2FsOpen: patch: %s, flags: 0x%llx, mode: %lld, arg addr: 0x%llx, argcount: %d",
		patch, oflags, mode, CPU.GPR[7], argcount);

	fs_file = fs_files.Open(patch, oflags, mode);
	return fs_file == -1 ? -1 : 0;
}

int SysCalls::lv2FsRead()
{
	const u32 fs_file = CPU.GPR[3];
	void* buf = Memory.GetMemFromAddr(CPU.GPR[4]);
	const u64 size = CPU.GPR[5];
	GPRtype& read = CPU.GPR[6];

	ConLog.Write("lv2FsRead: id: %d, buf addr: 0x%llx, size: %lld",
		fs_file, CPU.GPR[4], size);

	read = fs_files.Read(fs_file, buf, size);
	return read != size ? -1 : 0;
}

int SysCalls::lv2FsWrite()
{
	const u32 fs_file = CPU.GPR[3];
	const void* buf = Memory.GetMemFromAddr(CPU.GPR[4]);
	const u64 size = CPU.GPR[5];
	GPRtype& written = CPU.GPR[6];

	ConLog.Write("lv2FsWrite: id: %d, buf addr: 0x%llx, size: %lld",
		fs_file, CPU.GPR[4], size);

	written = fs_files.Write(fs_file, buf, size);
	return written != size;
}

int SysCalls::lv2FsClose()
{
	const u32 fs_file = CPU.GPR[3];

	ConLog.Write("lv2FsClose: id: %d", fs_file);

	return fs_files.Close(fs_file) ? 0 : -1;
}

int SysCalls::lv2FsOpenDir()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);
	GPRtype& fs_file = CPU.GPR[4];

	fs_file = fs_dirs.Open(patch);
	return fs_file == -1 ? -1 : 0;
}

int SysCalls::lv2FsReadDir()
{
	s64 fs_file = CPU.GPR[3];
	Lv2FsDirent* fs_dirent = (Lv2FsDirent*)&CPU.GPR[4];
	GPRtype& read = CPU.GPR[5];

	read = fs_dirs.Read(fs_file, fs_dirent);
	return 0;
}

int SysCalls::lv2FsCloseDir()
{
	s64 fs_file = CPU.GPR[3];

	return fs_dirs.Close(fs_file) ? 0 : -1;
}

int SysCalls::lv2FsMkdir()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);
	const u32 mode = CPU.GPR[4];//???

	if(wxDirExists(patch)) return -1;

	return wxMkdir(patch) ? 0 : -1;
}

int SysCalls::lv2FsRename()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);
	const wxString& newpatch = ReadStringInMem(CPU.GPR[4]);

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsRename error: %s not found", patch);
		return -1;
	}

	return wxRenameFile(patch, newpatch) ? 0 : -1;
}

int SysCalls::lv2FsLSeek64()
{
	const u32 fs_file = CPU.GPR[3];
	const s64 offset = CPU.GPR[4];
	const s32 whence = CPU.GPR[5];
	GPRtype& position = CPU.GPR[6];

	position = fs_files.Seek(fs_file, offset, whence);
	return position == offset ? 0 : -1;
}

int SysCalls::lv2FsRmdir()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsRmdir error: %s not found", patch);
		return -1;
	}

	return wxFileName::Rmdir(patch) ? 0 : -1;
}

int SysCalls::lv2FsUtime()
{
	const wxString& patch = ReadStringInMem(CPU.GPR[3]);
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