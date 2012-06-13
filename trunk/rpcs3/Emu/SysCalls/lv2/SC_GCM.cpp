#include "stdafx.h"
#include "Emu/SysCalls/SysCalls.h"
#include "Emu/GS/GCM.h"

SysCallBase sc_gcm("cellGcm");

CellGcmConfig current_config;
CellGcmContextData current_context;
gcmInfo gcm_info;

int cellGcmInit(const u32 context_addr, const u32 cmdSize, const u32 ioSize, const u32 ioAddress)
{
	sc_gcm.Log("cellGcmInit(context_addr=0x%x,cmdSize=0x%x,ioSize=0x%x,ioAddress=0x%x)", context_addr, cmdSize, ioSize, ioAddress);

	const u32 local_addr = Memory.Alloc(cmdSize, 0x100);

	current_config.ioSize = re32(ioSize);
	current_config.ioAddress = re32(ioAddress);
	current_config.localSize = re32(cmdSize);
	current_config.localAddress = re32(local_addr);
	current_config.memoryFrequency = re32(650000000);
	current_config.memoryFrequency = re32(500000000);

	gcm_info.context_addr = Memory.VideoMem.GetStartAddr() + 4;

	current_context.begin = re(ioAddress);
	current_context.end = re(ioAddress + ioSize);
	current_context.current = current_context.begin;

	/*
	if(wxFile::Access("gcm.dump", wxFile::read))
	{
		wxFile f("gcm.dump");
		mem32_t ctx_list(ioAddress);
		for(u32 i=0, buf; i<f.Length(); i+=4)
		{
			f.Read(&buf, sizeof(u32));
			ctx_list += buf;
		}

		current_context.begin = re32(ioAddress + 0x1000);
		current_context.end = re32(ioAddress + 0x1000 + 0x6ffc);
		current_context.current = re32(ioAddress + 0x1000 + 0x3a0);
	}
	else
	{
		current_context.begin = addr;
		current_context.end = addr + re(ioSize);
		current_context.current = current_context.begin;
	}
	*/
	current_context.callback = re32(gcm_info.context_addr + sizeof(current_context));
	Memory.WriteData(gcm_info.context_addr, current_context);

	Memory.Write32(re(current_context.callback), Memory.VideoMem.GetStartAddr());
	Memory.Write32(context_addr, gcm_info.context_addr);

	gcm_info.control_addr = re32(current_context.callback) + sizeof(current_context.callback);

	CellGcmControl& ctrl = *(CellGcmControl*)Memory.GetMemFromAddr(gcm_info.control_addr);
	ctrl.put = 0;
	ctrl.get = 0;
	ctrl.ref = -1;

	Emu.GetGSManager().GetRender().Init(ioAddress, ioSize, gcm_info.control_addr, local_addr);

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
	Memory.Write32(offset_addr, address - re(current_config.ioAddress));
	return CELL_OK;
}

int cellGcmSetDisplayBuffer(const u8 id, const u32 offset, const u32 pitch, const u32 width, const u32 height)
{
	sc_gcm.Log("cellGcmSetDisplayBuffer(id=0x%x,offset=0x%x,pitch=%d,width=%d,height=%d)",
		id, offset, width ? pitch/width : pitch, width, height);
	if(id > 1) return CELL_EINVAL;

	gcmBuffers[id].offset = offset;
	gcmBuffers[id].pitch = pitch;
	gcmBuffers[id].width = width;
	gcmBuffers[id].height = height;
	gcmBuffers[id].update = true;

	return CELL_OK;
}

int cellGcmGetControlRegister()
{
	sc_gcm.Log("cellGcmGetControlRegister()");

	return gcm_info.control_addr;
}

int cellGcmFlush(const u32 ctx, const u8 id)
{
	sc_gcm.Log("cellGcmFlush(ctx=0x%x, id=0x%x)", ctx, id);
	if(id > 1) return CELL_EINVAL;

	Emu.GetGSManager().GetRender().Draw();

	return 0;
}

int cellGcmSetTile(const u8 index, const u8 location, const u32 offset, const u32 size,
	const u32 pitch, const u8 comp, const u16 base, const u8 bank)
{
	sc_gcm.Log("cellGcmSetTile(index=%d, location=%d, offset=0x%x, size=0x%x, pitch=0x%x, comp=0x%x, base=0x%x, bank=0x%x)",
		index, location, offset, size, pitch, comp, base, bank);

	return 0;
}

int cellGcmGetFlipStatus()
{
	return Emu.GetGSManager().GetRender().m_flip_status;
}

int cellGcmResetFlipStatus()
{
	Emu.GetGSManager().GetRender().m_flip_status = 1;
	return 0;
}