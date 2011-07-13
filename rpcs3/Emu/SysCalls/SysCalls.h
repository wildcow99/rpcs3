#pragma once
#include <Emu/Cell/CPU.h>

class FreeNumList
{
protected:
	wxArrayInt free_nums_list;
	u32 cur_num;
	u32 max_size;
	wxString module_name;

	FreeNumList()
		: cur_num(0)
		, max_size(100)
		, module_name(wxEmptyString)
	{
	}

	inline const s32 GetFree() const
	{
		if(free_nums_list.GetCount() == 0) return -1;
		return free_nums_list[0];
	}

	inline void AddFree(const u32 num)
	{
		free_nums_list.Add(num);
	}

	inline void DeleteFreeByValue(const u32 value)
	{
		for(uint i=0; i<free_nums_list.GetCount(); ++i)
		{
			if(free_nums_list[i] == value)
			{
				DeleteFreeByNum(i);
				break;
			}
		}
	}

	inline void DeleteFreeByNum(const u32 num)
	{
		free_nums_list.Remove(num);
	}

	inline const u32 GetNumAndDelete()
	{
		s32 ret = GetFree();

		if(ret == -1)
		{
			ret = cur_num++;

			if(ret >= (s32)max_size)
			{
				ConLog.Error("%s error: Not enought free containers!", module_name);
				return 0;
			}
		}
		else
		{
			//DeleteFreeByValue(ret);
			DeleteFreeByNum(0);
		}

		return (u32)ret;
	}

	inline void Reset()
	{
		cur_num = 0;
		free_nums_list.Clear();
	}
};

//memory
int lv2MemContinerCreate();
int lv2MemContinerDestroy();

//filesystem
int lv2FsOpen();
int lv2FsRead();
int lv2FsWrite();
int lv2FsClose();
int lv2FsOpenDir();
int lv2FsReadDir();
int lv2FsCloseDir();
int lv2FsMkdir();
int lv2FsRename();
int lv2FsLSeek64();
int lv2FsRmdir();
int lv2FsUtime();

static int DoSyscall(int code)
{
	switch(code)
	{
		//=== lv2 ===
		//memory
		case 324: return lv2MemContinerCreate();
		case 325: return lv2MemContinerDestroy();
		//file system
		case 801: return lv2FsOpen();
		case 802: return lv2FsRead();
		case 803: return lv2FsWrite();
		case 804: return lv2FsClose();
		case 805: return lv2FsOpenDir();
		case 806: return lv2FsReadDir();
		case 807: return lv2FsCloseDir();
		case 811: return lv2FsMkdir();
		case 812: return lv2FsRename();
		case 813: return lv2FsRmdir();
		case 818: return lv2FsLSeek64();
		case 815: return lv2FsUtime();
	}

	ConLog.Error("Unknown syscall: %d - %08x", code, code);
	return 0;
}