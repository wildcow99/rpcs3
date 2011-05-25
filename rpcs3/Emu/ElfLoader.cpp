#include "stdafx.h"
#include "ElfLoader.h"
#include "rpcs3.h"
#include "Emu/Memory/Memory.h"

#include "Gui/MemoryViewer.h"

void ElfLoader::SetElf(wxString elf_full_patch)
{
	m_elf_fpatch = elf_full_patch;
}

void ElfLoader::Load() //TODO
{
	wxFile elf(m_elf_fpatch);

	if(!elf.IsOpened())
	{
		ConLog.Error("Error open %s!", wxFileName(m_elf_fpatch).GetFullName().c_str());
		return;
	}

	const u64 length = elf.Length();

	if(length == 0)
	{
		ConLog.Error("%s is null!", wxFileName(m_elf_fpatch).GetFullName().c_str());
		return;
	}

	const uint size = wxULongLong().GetLo();
	const uint csize = size * sizeof(u8);

	ElfHeader header;
	elf.Seek(0);
	elf.Read(&header, sizeof(ElfHeader));

	ConLog.Write("magic = %08X", header.e_magic);

	for(uint i=0; i<10; ++i) ConLog.Write("unknown %d = %08X", i, header.e_unknown[i]);

	const uint data_pc = 0x200;
	const uint mem_pc = 0x00000000;
	const uint mem_size = length - data_pc;

	elf.Seek(data_pc);
	elf.Read(&Memory.MainRam[mem_pc], mem_size);
	elf.Close();

	MemoryViewerPanel* memory_viewer = new MemoryViewerPanel(NULL);

	memory_viewer->SetPC(mem_pc);
	memory_viewer->Show();
	memory_viewer->ShowPC();
}

ElfLoader elf_loader;