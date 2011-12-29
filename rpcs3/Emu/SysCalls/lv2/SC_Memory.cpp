#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"
/*
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
*/

int SysCalls::lv2MemContinerCreate(PPUThread& CPU)
{
	u64& continer = CPU.GPR[3];
	u32 size = CPU.GPR[4];

	ConLog.Warning("lv2MemContinerCreate[size: 0x%x]", size);
	//continer = continers.AddContiner(size);
	return 0;
}

int SysCalls::lv2MemContinerDestroy(PPUThread& CPU)
{
	u32 container = CPU.GPR[3];
	ConLog.Warning("lv2MemContinerDestroy[container: 0x%x]", container);
	//continers.DeleteContiner(container);
	return 0;
}

static const u32 max_user_mem = 100 * (1024 ^ 3); //100mb
u32 free_user_mem = max_user_mem;
static u64 addr_user_mem = 0;

struct MemoryInfo
{
	u32 free_user_mem;
	u32 aviable_user_mem;
};

int SysCalls::lv2MemAllocate(PPUThread& CPU)
{
	//size_t size, uint64_t flags, sys_addr_t * alloc_addr;

	const u64 size = CPU.GPR[3]; //size
	const u64 flags = CPU.GPR[4]; //flags
	u64& alloc_addr = CPU.GPR[5]; //alloc_addr

	ConLog.Write("lv2MemAllocate: size: 0x%llx, flags: 0x%llx, alloc_addr: 0x%llx", size, flags, alloc_addr);

	if(free_user_mem < size)
	{
		ConLog.Error("Not enough free user memory!");
		return -1;
	}
	if(addr_user_mem == 0) addr_user_mem = Memory.MainRam.GetEndAddr(); //FIXME!
	addr_user_mem -= size;
	free_user_mem -= size;
	alloc_addr = addr_user_mem;
	return 0;
}

int SysCalls::lv2MemGetUserMemorySize(PPUThread& CPU)
{
	ConLog.Write("lv2MemGetUserMemorySize: r3=0x%llx, r4=0x%llx", CPU.GPR[3], CPU.GPR[4]);
	MemoryInfo mem_info;
	mem_info.aviable_user_mem = max_user_mem; 
	mem_info.free_user_mem = free_user_mem;
	memcpy(Memory.GetMemFromAddr(CPU.GPR[3]), &mem_info, sizeof(mem_info));

	CPU.GPR[4] = CPU.GPR[3];
	return 0;
}