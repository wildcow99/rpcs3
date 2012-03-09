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

#define CELL_FS_TYPE_UNKNOWN   0

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

enum FsDirentType
{
	CELL_FS_TYPE_DIRECTORY	= 1,
	CELL_FS_TYPE_REGULAR	= 2,
	CELL_FS_TYPE_SYMLINK	= 3,
};

SysCallBase sc_log("cellFs");

wxString ReadString(const u64 addr)
{
	return wxGetCwd() + Memory.ReadString(addr);
}

int cellFsOpen(const u64 path_addr, const int flags, const u64 fd_addr, const u64 arg_addr, const u64 size)
{
	const wxString& path = Memory.ReadString(path_addr);
	sc_log.Log("cellFsOpen(path: %s, flags: 0x%x, fd_addr: 0x%x, arg_addr: 0x%llx, size: 0x%llx)",
		path, flags, fd_addr, arg_addr, size);

	const wxString& ppath = wxGetCwd() + (path[0] == '//' ? path : "//" + path);

	wxFile::OpenMode o_mode;

	s32 _oflags = flags;
	if(flags & LV2_O_CREAT)
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

	if(flags & LV2_O_RDONLY)
	{
		_oflags &= ~LV2_O_RDONLY;
		o_mode = wxFile::read;
	}
	else if(flags & LV2_O_RDWR)
	{
		_oflags &= ~LV2_O_RDWR;
		o_mode = wxFile::read_write;
	}
	else if(flags & LV2_O_WRONLY)
	{
		_oflags &= ~LV2_O_WRONLY;

		if(flags & LV2_O_APPEND)
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
		sc_log.Error("'%s' has unknown flags! flags: 0x%08x", path, flags);
		return CELL_EINVAL;
	}

	if(o_mode == wxFile::read && !wxFile::Access(ppath, o_mode)) return CELL_ENOENT;

	Memory.Write32(fd_addr, 
		Emu.GetIdManager().GetNewID(sc_log.GetName(), new wxFile(ppath, o_mode), flags));

	return CELL_OK;
}

int cellFsRead(const u32 fd, const u64 buf_addr, const u64 nbytes, const u64 nread_addr)
{
	sc_log.Log("cellFsRead(fd: %d, buf_addr: 0x%llx, nbytes: 0x%llx, nread_addr: 0x%llx)",
		fd, buf_addr, nbytes, nread_addr);
	if(!Emu.GetIdManager().CheckID(fd)) return CELL_ESRCH;
	wxFile& file = *(wxFile*)Emu.GetIdManager().GetIDData(fd).m_data;
	Memory.Write64(nread_addr, file.Read(Memory.GetMemFromAddr(buf_addr), nbytes));
	return CELL_OK;
}

int cellFsWrite(const u32 fd, const u64 buf_addr, const u64 nbytes, const u64 nwrite_addr)
{
	sc_log.Log("cellFsWrite(fd: %d, buf_addr: 0x%llx, nbytes: 0x%llx, nwrite_addr: 0x%llx)",
		fd, buf_addr, nbytes, nwrite_addr);
	if(!Emu.GetIdManager().CheckID(fd)) return CELL_ESRCH;
	wxFile& file = *(wxFile*)Emu.GetIdManager().GetIDData(fd).m_data;
	Memory.Write64(nwrite_addr, file.Write(Memory.GetMemFromAddr(buf_addr), nbytes));
	return CELL_OK;
}

int cellFsClose(const u32 fd)
{
	sc_log.Log("cellFsClose(fd: %d)", fd);
	if(!Emu.GetIdManager().CheckID(fd)) return CELL_ESRCH;
	wxFile& file = *(wxFile*)Emu.GetIdManager().GetIDData(fd).m_data;
	file.Close();
	Emu.GetIdManager().RemoveID(fd);
	return CELL_OK;
}

int cellFsOpendir(const u64 path_addr, const u64 fd_addr)
{
	const wxString& path = ReadString(path_addr);
	sc_log.Error("cellFsOpendir(path: %s, fd_addr: 0x%llx)", path, fd_addr);
	return CELL_OK;
}

int cellFsReaddir(const u32 fd, const u64 dir_addr, const u64 nread_addr)
{
	sc_log.Error("cellFsReaddir(fd: %d, dir_addr: 0x%llx, nread_addr: 0x%llx)", fd, dir_addr, nread_addr);
	return CELL_OK;
}

int cellFsClosedir(const u32 fd)
{
	sc_log.Error("cellFsClosedir(fd: %d)", fd);
	return CELL_OK;
}

int cellFsStat(const u64 path_addr, const u64 sb_addr)
{
	const wxString& path = ReadString(path_addr);
	sc_log.Error("cellFsFstat(path: %s, sb_addr: 0x%llx)", path, sb_addr);
	return CELL_OK;
}

int cellFsFstat(const u32 fd, const u64 sb_addr)
{
	sc_log.Error("cellFsFstat(fd: %d, sb_addr: 0x%llx)", fd, sb_addr);
	return CELL_OK;
}

int cellFsMkdir(const u64 path_addr, const u32 mode)
{
	const wxString& path = ReadString(path_addr);
	sc_log.Log("cellFsMkdir(path: %s, mode: 0x%x)", path, mode);
	if(wxDirExists(path)) return CELL_EEXIST;
	if(!wxMkdir(path)) return CELL_EBUSY;
	return CELL_OK;
}

int cellFsRename(const u64 from_addr, const u64 to_addr)
{
	const wxString& from = ReadString(from_addr);
	const wxString& to = ReadString(to_addr);
	sc_log.Log("cellFsRename(from: %s, to: %s)", from, to);
	if(!wxFileExists(from)) return CELL_ENOENT;
	if(wxFileExists(to)) return CELL_EEXIST;
	if(!wxRenameFile(from, to)) return CELL_EBUSY;
	return CELL_OK;
}

int cellFsRmdir(const u64 path_addr)
{
	const wxString& path = ReadString(path_addr);
	sc_log.Log("cellFsRmdir(path: %s)", path);
	if(!wxDirExists(path)) return CELL_ENOENT;
	if(!wxRmdir(path)) return CELL_EBUSY;
	return CELL_OK;
}

int cellFsUnlink(const u64 path_addr)
{
	const wxString& path = ReadString(path_addr);
	sc_log.Error("cellFsUnlink(path: %s)", path);
	return CELL_OK;
}

int cellFsLseek(const u32 fd, const s64 offset, const u32 whence, const u64 pos_addr)
{
	wxSeekMode seek_mode;
	switch(whence)
	{
	case LV2_SEEK_SET: seek_mode = wxFromStart; break;
	case LV2_SEEK_CUR: seek_mode = wxFromCurrent; break;
	case LV2_SEEK_END: seek_mode = wxFromEnd; break;
	default:
		sc_log.Error(fd, "Unknown seek whence! (%d)", whence);
	return CELL_EINVAL;
	}
	if(!Emu.GetIdManager().CheckID(fd)) return CELL_ESRCH;
	wxFile& file = *(wxFile*)Emu.GetIdManager().GetIDData(fd).m_data;
	Memory.Write64(pos_addr, file.Seek(offset, seek_mode));
	return CELL_OK;
}