#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"

SysCallBase sc_gcm("cellGcm");

struct CellGcmConfig
{
	u32 localAddress;
	u32 ioAddress;
	u32 localSize;
	u32 ioSize;
	u32 memoryFrequency;
	u32 coreFrequency;
} current_config;

int cellGcmInit(const u32 cmdSize, const u32 ioSize, const u32 ioAddress)
{
	sc_gcm.Log("cellGcmInit(cmdSize=0x%x,ioSize=0x%x,ioAddress=0x%x)", cmdSize, ioSize, ioAddress);
	current_config.ioSize = re(ioSize);
	current_config.ioAddress = re(ioAddress);
	current_config.localSize = current_config.ioSize;
	current_config.localAddress = current_config.ioAddress;
	return 0;
}

int cellGcmGetConfiguration(const u32 config_addr)
{
	sc_gcm.Log("cellGcmGetConfiguration(config_addr=0x%x)", config_addr);
	Memory.WriteData(config_addr, current_config);
	return 0;
}

int cellGcmAddressToOffset(const u32 address, const u32 offset_addr)
{
	sc_gcm.Log("cellGcmAddressToOffset(address=0x%x,offset_addr=0x%x)", address, offset_addr);
	Memory.Write32(offset_addr, address);
	return CELL_OK;
}

int cellGcmSetDisplayBuffer(const u8 id, const u32 offset, const u32 pitch, const u32 width, const u32 height)
{
	sc_gcm.Log("cellGcmSetDisplayBuffer(id=0x%x,offset=0x%x,pitch=0x%x,width=0x%x,height=0x%x)", id, offset, pitch, width, height);
	if(id > 1) return CELL_EINVAL;

	return CELL_OK;
}