#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

typedef u64 GPRtype;
enum Lv2FsOflag
{
	LV2_O_RDONLY	= 000000,
	LV2_O_WRONLY	= 000001,
	LV2_O_RDWR		= 000002,
	LV2_O_ACCMODE	= 000003,
	LV2_O_CREAT		= 000100,
	LV2_O_EXCL		= 000200,
	LV2_O_TRUNC		= 001000,
	LV2_O_APPEND	= 002000,
	LV2_O_MSELF		= 010000,
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

SysCallBase sc_f("FSFile");
SysCallBase sc_d("FSDir");
int SysCalls::lv2FsOpen(PPUThread& CPU)
{
	const wxString& path = Memory.ReadString(CPU.GPR[3]);
	const u32 oflags = CPU.GPR[4];
	const u64 fd_addr = CPU.GPR[5];

	const u32 mode = CPU.GPR[6];
	const void* arg = Memory.GetMemFromAddr(CPU.GPR[7]);//???
	const u32 argcount = CPU.GPR[8];

	sc_f.Log("lv2FsOpen[patch: %s, flags: 0x%llx, mode: %lld, arg addr: 0x%llx, argcount: %d]",
		path, oflags, mode, CPU.GPR[7], argcount);

	const wxString& ppath = wxGetCwd() + path;

	wxFile::OpenMode o_mode;

	s32 _oflags = oflags;
	if(oflags & LV2_O_CREAT)
	{
		_oflags &= ~LV2_O_CREAT;
		//create path
		for(uint p=1;p<path.Length();p++)
		{
			for(;p<path.Length(); p++) if(path[p] == '/') break;
			
			if(p == path.Length()) break;
			const wxString& dir = wxGetCwd() + path(0, p);
			if(!wxDirExists(dir))
			{
				ConLog.Write("create dir: %s", dir);
				wxMkdir(dir);
			}
		}

		//create file
		if(!wxFileExists(ppath))
		{	
			wxFile f;
			f.Create(ppath);
			f.Close();
		}
	}

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

	if(_oflags != 0)
	{
		sc_f.Error("'%s' has unknown flags! flags: 0x%08x", path, oflags);
		return CELL_UNKNOWN_ERROR;
	}

	if(o_mode == wxFile::read && !wxFile::Access(ppath, wxFile::read))
	{
		sc_f.Error("%s not found! flags: 0x%08x", ppath, oflags);
		return CELL_UNKNOWN_ERROR;
	}

	const u32 fd = SysCalls_IDs.GetNewID(sc_f.GetName(), new wxFile(ppath, o_mode), oflags);
	if(!SysCalls_IDs.CheckID(fd))
	{
		Memory.Write32(fd_addr, -1);
		return CELL_UNKNOWN_ERROR;
	}
	
	Memory.Write32(fd_addr, fd);
	return CELL_OK;
}

int SysCalls::lv2FsRead(PPUThread& CPU)
{
	const u32 fs_file = CPU.GPR[3];
	void* buf = Memory.GetMemFromAddr(CPU.GPR[4]);
	const u64 size = CPU.GPR[5];
	GPRtype& read = CPU.GPR[6];

	sc_f.Log("lv2FsRead[id: %d, buf addr: 0x%llx, size: %lld]",
		fs_file, CPU.GPR[4], size);

	if(size == 0) return CELL_OK;

	if(!SysCalls_IDs.CheckID(fs_file))
	{
		sc_f.Error("id [%d] is not found!", fs_file);
		read = 0;
		return CELL_EBADF;
	}

	wxFile& file = *(wxFile*)SysCalls_IDs.GetIDData(fs_file).m_data;
	read = file.Read(buf, size);

	return read == size ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsWrite(PPUThread& CPU)
{
	const u32 fs_file = CPU.GPR[3];
	const void* buf = Memory.GetMemFromAddr(CPU.GPR[4]);
	const u64 size = CPU.GPR[5];
	GPRtype& written = CPU.GPR[6];

	sc_f.Log("lv2FsWrite[id: %d, buf addr: 0x%llx, size: %lld]",
		fs_file, CPU.GPR[4], size);

	if(!SysCalls_IDs.CheckID(fs_file))
	{
		sc_f.Error("id [%d] is not found!", fs_file);
		written = 0;
		return CELL_EBADF;
	}

	wxFile& file = *(wxFile*)SysCalls_IDs.GetIDData(fs_file).m_data;
	/*
	if(size == 0)
	{
		for(u32 i=0; ; i++)
		{
			const u8 b = ((u8*)buf)[i];

			if(b == 0)
			{
				written = i;
				return CELL_OK;
			}

			file.Write(&b, 1);
		}
	}
	*/
	written = file.Write(buf, size);
	return written == size ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsClose(PPUThread& CPU)
{
	const u32 fs_file = CPU.GPR[3];

	sc_f.Log("lv2FsClose[id: %d]", fs_file);

	if(!SysCalls_IDs.CheckID(fs_file))
	{
		sc_f.Error("id [%d] is not found!", fs_file);
		return CELL_EBADF;
	}

	wxFile& file = *(wxFile*)SysCalls_IDs.GetIDData(fs_file).m_data;
	file.Close();
	SysCalls_IDs.RemoveID(fs_file);
	
	return CELL_OK;
}

int SysCalls::lv2FsOpenDir(PPUThread& CPU)
{
	const wxString& path = Memory.ReadString(CPU.GPR[3]);
	GPRtype& fs_file = CPU.GPR[4];

	if(!wxDirExists(path))
	{
		sc_d.Error("%s not found!", path);
		return CELL_ENOENT;
	}

	fs_file = SysCalls_IDs.GetNewID(sc_f.GetName(), new wxDir(path));

	return SysCalls_IDs.CheckID(fs_file) ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsReadDir(PPUThread& CPU)
{
	s64 fs_file = CPU.GPR[3];
	Lv2FsDirent* fs_dirent = (Lv2FsDirent*)Memory.GetMemFromAddr(CPU.GPR[4]);
	GPRtype& read = CPU.GPR[5];

	if(!SysCalls_IDs.CheckID(fs_file))
	{
		sc_d.Error("id [%d] is not found!", fs_file);
		fs_dirent = NULL;
		return CELL_EBADF;
	}

	wxArrayString files;

	wxDir& dir = *(wxDir*)SysCalls_IDs.GetIDData(fs_file).m_data;
	read = wxDir::GetAllFiles(dir.GetName(), &files);

	fs_dirent = new Lv2FsDirent[files.GetCount()];

	for(uint i=0; i<files.GetCount(); ++i)
	{
		fs_dirent[i].d_namlen = files[i].Length() <= LV2_MAX_FS_FILE_NAME_LENGTH ? files[i].Length() : LV2_MAX_FS_FILE_NAME_LENGTH;

		for(uint a=0; a<fs_dirent[i].d_namlen; ++a)
		{
			fs_dirent[i].d_name[a] = files[i][a];
		}

		fs_dirent[i].d_type = 1;//???
	}

	files.Clear();

	return CELL_OK;
}

int SysCalls::lv2FsCloseDir(PPUThread& CPU)
{
	s64 fs_file = CPU.GPR[3];

	if(!SysCalls_IDs.CheckID(fs_file))
	{
		sc_d.Error("id [%d] is not found!", fs_file);
		return CELL_EBADF;
	}

	SysCalls_IDs.RemoveID(fs_file);
	return CELL_OK;
}

int SysCalls::lv2FsMkdir(PPUThread& CPU)
{
	const wxString& path = Memory.ReadString(CPU.GPR[3]);
	const u32 mode = CPU.GPR[4];//???
	sc_f.Log("lv2FsMkdir[path: %s, mode: 0x%x]", path, mode);

	if(path.IsEmpty()) return CELL_ENOENT;

	if(wxDirExists(path))
	{
		ConLog.Write("lv2FsMkdir error: path[%s] is exists", path);
		return CELL_EEXIST;
	}

	return wxMkdir(path) ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsRename(PPUThread& CPU)
{
	sc_f.Log("lv2FsRename[r3: 0x%llx, r4: 0x%llx, r5: 0x%llx]", CPU.GPR[3], CPU.GPR[4], CPU.GPR[5]);
	const wxString& path = Memory.ReadString(CPU.GPR[3]);
	const wxString& newpath = Memory.ReadString(CPU.GPR[4]);

	sc_f.Log("lv2FsRename[path: %s, newpath: %s]", path, newpath);

	if(path.IsEmpty() || !wxFile::Access(path, wxFile::read))
	{
		sc_f.Error("%s not found", path);
		return CELL_ENOENT;
	}

	return wxRenameFile(path, newpath) ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsLSeek64(PPUThread& CPU)
{
	const u32 fs_file = CPU.GPR[3];
	const s64 offset = CPU.GPR[4];
	const s32 whence = CPU.GPR[5];
	GPRtype& position = CPU.GPR[6];

	wxSeekMode seek_mode;
	switch(whence)
	{
	case LV2_SEEK_SET: seek_mode = wxFromStart; break;
	case LV2_SEEK_CUR: seek_mode = wxFromCurrent; break;
	case LV2_SEEK_END: seek_mode = wxFromEnd; break;
	default:
		sc_f.Error(fs_file, "Unknown seek whence! (%d)", whence);
	return CELL_UNKNOWN_ERROR;
	}
	wxFile& file = *(wxFile*)SysCalls_IDs.GetIDData(fs_file).m_data;
	position = file.Seek(offset, seek_mode);

	return position == offset ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsRmdir(PPUThread& CPU)
{
	const wxString& path = Memory.ReadString(CPU.GPR[3]);

	sc_f.Log("lv2FsRmdir[path: %s]", path);

	if(!wxDirExists(path))
	{
		sc_f.Error("%s not found", path);
		return CELL_ENOENT;
	}

	return wxFileName::Rmdir(path) ? CELL_OK : CELL_UNKNOWN_ERROR;
}

int SysCalls::lv2FsUtime(PPUThread& CPU)
{
	const wxString& path = Memory.ReadString(CPU.GPR[3]);
	Lv2FsUtimbuf& times = *(Lv2FsUtimbuf*)&CPU.GPR[4];

	if(!wxFileExists(path))
	{
		sc_f.Error("%s not found", path);
		return CELL_ENOENT;
	}

	wxDateTime actime, modtime;
    wxFileName(path).GetTimes(&actime, &modtime, NULL);

	times.modtime = modtime.GetTicks();
	times.actime = actime.GetTicks();
	return CELL_OK;
}