#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

struct MemContiner
{
	u8* continer;
	u32 num;

	void Create(u32 size)
	{
		continer = new u8[size];
	}

	void Delete()
	{
		if(continer != NULL) free(continer);
	}

	~MemContiner()
	{
		Delete();
	}
};

class MemContiners : private SysCallBase
{
	SysCallsArraysList<MemContiner> continers;

public:
	MemContiners() : SysCallBase("MemContainers")
	{
	}

	u64 AddContiner(const u64 size)
	{
		const u64 id = continers.Add();
		bool error;
		MemContiner& data = *continers.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return 0;
		}

		data.Create(size);

		return id;
	}

	void DeleteContiner(const u64 id)
	{
		bool error;
		MemContiner& data = *continers.GetDataById(id, &error);
		if(error)
		{
			ConLog.Error("%s error: id [%d] is not found!", module_name, id);
			return;
		}
		data.Delete();
		continers.RemoveById(id);
	}
};

MemContiners continers;

int lv2MemContinerCreate()
{
	s64& continer = CPU.GPR[3];
	u32 size = CPU.GPR[4];

	continer = continers.AddContiner(size);
	return 0;
}

int lv2MemContinerDestroy()
{
	u32 container = CPU.GPR[3];

	continers.DeleteContiner(container);
	return 0;
}