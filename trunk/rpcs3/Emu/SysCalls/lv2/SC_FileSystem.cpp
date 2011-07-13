#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

enum filesystem_oflags
{
	LV2_O_RDONLY = 0x000000,
	LV2_O_WRONLY = 0x000001,
	LV2_O_RDWR = 0x000002,
	LV2_O_ACCMODE = 0x000003,
	LV2_O_CREAT = 0x000100,
	LV2_O_EXCL = 0x000200,
	LV2_O_TRUNC = 0x001000,
	LV2_O_APPEND = 0x002000,
	LV2_O_MSELF = 0x010000,
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

class FSFiles : private FreeNumList
{
	wxFile* fs_file;

public:
	FSFiles()
		: FreeNumList()
		, fs_file(new wxFile[100])
	{
		module_name = "FSFiles";
	}

	u32 Open(wxString patch, s32 oflags)
	{
		const u32 num = GetNumAndDelete();
		wxFile::OpenMode mode;

		if(oflags & LV2_O_RDONLY)
		{
			mode = wxFile::read;
		}
		else if(oflags & LV2_O_RDWR)
		{
			mode = wxFile::read_write;
		}
		else if(oflags & LV2_O_WRONLY)
		{
			if(oflags & LV2_O_EXCL)
			{
				mode = wxFile::write_excl;
			}
			else if(oflags & LV2_O_APPEND)
			{
				mode = wxFile::write_append;
			}
			else
			{
				mode = wxFile::write;
			}
		}

		if(mode == wxFile::read && !wxFile::Access(patch, wxFile::read))
		{
			ConLog.Error("FSFiles error: %s not found! mode: %06x", patch, mode);
			return 0;
		}
		
		fs_file[num].Open(patch, mode);
		return num;
	}

	s64 Read(const u32 num, void* buf, const u64 size)
	{
		return fs_file[num].Read(buf, size);
	}

	s64 Write(const u32 num, const void* buf, const u64 size)
	{
		return fs_file[num].Write(buf, size);
	}

	s64 Seek(const u32 num, const s64 offset, const u32 whence)
	{
		return fs_file[num].Seek(offset);
	}

	void Close(u32 num)
	{
		fs_file[num].Close();
		AddFree(num);
	}
};

FSFiles fs_files;

class FSDirs : private FreeNumList
{
	wxDir* fs_dir;

public:
	FSDirs()
		: FreeNumList()
		, fs_dir(new wxDir[100])
	{
		module_name = "FSDirs";
	}

	u32 Open(wxString patch)
	{
		const u32 num = GetNumAndDelete();

		if(wxDirExists(patch))
		{
			ConLog.Error("FSDirs error: %s not found!", patch);
			return 0;
		}
		
		fs_dir[num].Open(patch);
		return num;
	}

	u32 Read(u32 num, Lv2FsDirent* fs_dirent)
	{
		wxArrayString files;
		const u32 ret = wxDir::GetAllFiles(fs_dir[num].GetName(), &files);

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

	void Close(u32 num)
	{
		AddFree(num);
	}
};

FSDirs fs_dirs;

int lv2FsOpen()
{
	const wxString patch = (char*)CPU.GPR[3];
	const s32 oflags = CPU.GPR[4];
	s64& fs_file = CPU.GPR[5];

	const s32 mode = CPU.GPR[6]; //???
	void* arg = (void*)&CPU.GPR[7];//???
	const s32 argcount = CPU.GPR[8];

	fs_file = fs_files.Open(patch, oflags);
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
	const wxString patch = (char*)CPU.GPR[3];
	s64& fs_file = CPU.GPR[4];

	fs_file = fs_dirs.Open(patch);
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
	const wxString patch = (char*)CPU.GPR[3];
	const u32 mode = CPU.GPR[4];//???

	if(wxDirExists(patch)) return -1;

	wxMkdir(patch);
	return 0;
}

int lv2FsRename()
{
	const wxString patch = (char*)CPU.GPR[3];
	const wxString newpatch = (char*)CPU.GPR[4];

	if(!wxFile::Access(patch, wxFile::read))
	{
		ConLog.Error("lv2FsRename error: %s not found", patch);
		return -1;
	}

	wxRenameFile(patch, newpatch);
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
	const wxString patch = (char*)CPU.GPR[3];

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
	const wxString patch = (char*)CPU.GPR[3];
	Lv2FsUtimbuf& times = *(Lv2FsUtimbuf*)CPU.GPR[4];

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