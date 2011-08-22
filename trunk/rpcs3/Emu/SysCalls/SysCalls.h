#pragma once
#include <Emu/Cell/CPU.h>

class SysCallBase
{
protected:
	wxString module_name;

	SysCallBase(const wxString& name) : module_name(name)
	{
	}

	//TODO
};

static wxString FixPatch(const wxString& patch)
{
	if(!patch(0, 10).CmpNoCase("/dev_hdd0/")) return CurGameInfo.root + patch(10, patch.Length());

	ConLog.Warning("Unknown patch: %s", patch);
	return patch;
}

template<typename T> struct SysCallsData
{
	T* data;
	u64 id;
};

template<typename T> class SysCallsArraysList
{
	u64 m_count;
	SysCallsData<T>* m_array;

public:
	SysCallsArraysList()
		: m_count(0)
		, m_array(NULL)
	{
	}

	__forceinline bool RemoveById(const u64 id)
	{
		const int num = GetNumById(id);
		if(num < 0 || num >= m_count) return false;

		m_count--;
		SysCallsData<T>* new_array = (SysCallsData<T>*)malloc(sizeof(SysCallsData<T>) * m_count);
		memmove(new_array, m_array, num * sizeof(T));
		if(m_count > num) memmove(&new_array[num], &m_array[num + 1], (m_count - num) * sizeof(T));
		safe_delete(m_array);
		m_array = new_array;

		return true;
	}

	__forceinline int GetNumById(const u64 id)
	{
		for(uint i=0; i<m_count; ++i)
		{
			if(m_array[i].id == id) return i;
		}

		return -1;
	}

	__forceinline T* GetDataById(const u64 id, bool* is_error = NULL)
	{
		if(is_error) *is_error = false;

		for(uint i=0; i<m_count; ++i)
		{
			if(m_array[i].id == id) return m_array[i].data;
		}

		if(is_error) *is_error = true;
		return NULL;
	}

	__forceinline u64 GetFreeId()
	{
		for(u64 id=1; id>0; id++)
		{
			bool found = false;
			for(u64 arr=0; arr<m_count; ++arr)
			{
				if(m_array[arr].id == id)
				{
					found = true;
					break;
				}
			}

			if(!found) return id;
		}

		return 0;
	}

	__forceinline u64 Add()
	{
		return Add(new T());
	}

	__forceinline u64 Add(T* data)
	{
		if(!m_array)
		{
			m_array = (SysCallsData<T>*)malloc(sizeof(SysCallsData<T>));
		}
		else
		{
			m_array = (SysCallsData<T>*)realloc(m_array, sizeof(SysCallsData<T>) * (m_count + 1));
		}
		const u64 id = GetFreeId();
		m_array[m_count].data = data;
		m_array[m_count].id = id;
		m_count++;

		return id;
	}

	__forceinline u64 Add(T& data)
	{
		return Add(&data);
	}

	__forceinline void Clear()
	{
		safe_delete(m_array);
	}
};

class SysCalls
{
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

protected:
	CPUThread& CPU;

	SysCalls(CPUThread& cpu) : CPU(cpu)
	{
	}

	int DoSyscall(int code)
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
};