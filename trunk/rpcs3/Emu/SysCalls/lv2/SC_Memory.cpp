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

class MemContiners : private FreeNumList
{
	MemContiner* continers;

public:
	MemContiners()
		: FreeNumList()
		, continers(new MemContiner[100])
	{
		module_name = "MemContainers";
	}

	u32 AddContiner(u32 size)
	{
		const u32 num = GetNumAndDelete();
		continers[num].Create(size);

		return num;
	}

	void DeleteContiner(u32 num)
	{
		continers[num].Delete();
		AddFree(num);
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