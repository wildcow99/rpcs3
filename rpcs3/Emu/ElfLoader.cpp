#include "stdafx.h"
#include "ElfLoader.h"
#include "rpcs3.h"
#include "Emu/Memory/Memory.h"

#include "Gui/MemoryViewer.h"

void ElfLoader::SetElf(wxString elf_full_patch)
{
	m_elf_fpatch = elf_full_patch;
}

uint ElfLoader::LoadElf32(Elf32_Ehdr& header, wxFile& elf)
{
	uint ret = 0;
	ConLog.Warning("Elf 32!!!");
	//ConLog.Write("magic = %08X", header.e_magic & 0xFFFFFFFFL);
	ConLog.Write("machine = %08X", header.e_machine & 0xFFFF);
	ConLog.Write("version = %08X", header.e_version & 0xFFFFFFFFL);
	ConLog.Write("entry = %08X", header.e_entry & 0xFFFFFFFFL);
	ConLog.Write("version = %08X", header.e_phoff & 0xFFFFFFFFL);
	ConLog.Write("shoff = %08X", header.e_shoff & 0xFFFFFFFFL);
	ConLog.Write("flags = %08X", header.e_flags & 0xFFFFFFFFL);
	ConLog.Write("ehsize = %08X", header.e_ehsize & 0xFFFF);
	ConLog.Write("phnum = %08X", header.e_phnum & 0xFFFF);
	ConLog.Write("phoff = %08X", header.e_phoff & 0xFFFF);

	return ret;
}

uint ElfLoader::LoadElf64(Elf64_Ehdr& header, wxFile& elf)
{
	uint ret = 0;

	//ConLog.Write("magic = %08X", header.e_magic & 0xFFFFFFFFL);
	ConLog.Write("machine = %08X", header.e_machine & 0xFFFF);
	ConLog.Write("version = %08X", header.e_version & 0xFFFFFFFFL);
	ConLog.Write("entry = %08X", header.e_entry & 0xFFFFFFFFL);
	ConLog.Write("version = %08X", header.e_phoff & 0xFFFFFFFFL);
	ConLog.Write("shoff = %08X", header.e_shoff & 0xFFFFFFFFL);
	ConLog.Write("flags = %08X", header.e_flags & 0xFFFFFFFFL);
	ConLog.Write("ehsize = %08X", header.e_ehsize & 0xFFFF);
	ConLog.Write("phnum = %08X", header.e_phnum & 0xFFFF);
	ConLog.Write("phoff = %08X", header.e_phoff & 0xFFFF);

	return ret;

	const uint length = elf.Length();

	if( (header.e_phnum & 0xFFFF) > 0 && (header.e_phoff & 0xFFFF) > 0 )
	{
		Elf64_Phdr* proghead = new Elf64_Phdr[header.e_phnum & 0xFFFF];
		elf.Seek(header.e_phoff);
		elf.Read(proghead, sizeof(proghead));

		for( u16 i = 0; i < (header.e_phnum & 0xFFFF); ++i )
		{
			if(proghead[i].p_type == 0x1)
			{
				if (proghead[i].p_offset < length)
				{
					ConLog.Write("Loading Elf64... (%u)", proghead[i].p_type);

					uint size;
					
					if ((proghead[i].p_filesz + proghead[i].p_offset) > length)
					{
						size = length - proghead[i].p_offset;
					}
					else
						size = proghead[i].p_filesz;

					if (proghead[i].p_vaddr != proghead[i].p_paddr)
						ConLog.Warning( L"ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x",
							proghead[i].p_paddr, proghead[i].p_vaddr);

					ConLog.SkipLn();
					ConLog.Write("Founded opcode!");
					ConLog.Write("opcode: size - %d", size);
					ConLog.Write("opcode: addr - %x", proghead[i].p_vaddr & 0x1ffffff);
					ConLog.Write("opcode: offs - %x", proghead[i].p_offset);
					ConLog.SkipLn();

					elf.Seek(proghead[i].p_offset);
					elf.Read(&Memory.MainRam[proghead[i].p_vaddr & 0x1ffffff], size);

					ret = proghead[i].p_vaddr & 0x1ffffff;
				
					ConLog.Write("Elf64 loaded!");

					//return ret;
				}
				else
				{
					ConLog.Warning( L"Unknown Elf64 type! (%u)", proghead[i].p_type );
				}
			}
		}
	}

	return ret;
}

static const uint data_pc = 0;

void ElfLoader::LoadElf() //TODO
{
	wxFile elf(m_elf_fpatch);
	const wxChar* elf_name = wxFileName(m_elf_fpatch).GetFullName().c_str();
	if(!elf.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return;
	}

	const u64 length = elf.Length();

	if(length == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		return;
	}

	const uint size = wxULongLong().GetLo();
	const uint csize = size * sizeof(u8);

	elf.Seek(0);
	u8	ident[16] = {0};
	elf.Read(ident, sizeof(ident));
	elf.Seek(0);

	uint mem_pc = 0;
	if(ident[4] == 1)
	{
		Elf32_Ehdr header;
	
		elf.Read(&header, sizeof(Elf32_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*/

		mem_pc = LoadElf32(header, elf);
	}
	else if(ident[4] == 2)
	{
	
		Elf64_Ehdr header;
	
		elf.Read(&header, sizeof(Elf64_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*/

		mem_pc = LoadElf64(header, elf);
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", ident[4]);
		return;
	}

	if(mem_pc == 0)
	{
		//const uint data_pc = 0x200;
		const uint mem_size = length - data_pc;

		elf.Seek(data_pc);
		elf.Read(&Memory.MainRam[mem_pc], mem_size);
	}

	elf.Close();

	MemoryViewerPanel* memory_viewer = new MemoryViewerPanel(NULL);

	memory_viewer->SetPC(mem_pc);
	memory_viewer->Show();
	memory_viewer->ShowPC();
}

void ElfLoader::LoadSelf()
{
	wxFile elf(m_elf_fpatch);
	const wxChar* elf_name = wxFileName(m_elf_fpatch).GetFullName().c_str();
	if(!elf.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return;
	}

	const u64 length = elf.Length();

	if(length == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		return;
	}

	const uint size = wxULongLong().GetLo();
	const uint csize = size * sizeof(u8);

	SelfHeader header;
	elf.Seek(0);
	elf.Read(&header, sizeof(SelfHeader));

	ConLog.Write("magic = %08X", header.magic);
	ConLog.Write("attribute = %04X", header.attribute);
	ConLog.Write("category = %04X", header.category);
	ConLog.Write("metadataInfoOffset = %08X", header.metadataInfoOffset);
	ConLog.Write("fileOffset = %16X", header.fileOffset);
	ConLog.Write("fileSize = %16X", header.fileSize);
	ConLog.Write("id = %16X", header.id);
	ConLog.Write("programInfoOffset = %16X", header.programInfoOffset);
	ConLog.Write("elfHeaderOffset = %08X", header.elfHeaderOffset);
	ConLog.Write("elfProgramHeadersOffset = %16X", header.elfProgramHeadersOffset);
	ConLog.Write("elfSectionHeadersOffset = %16X", header.elfSectionHeadersOffset);
	ConLog.Write("sInfoOffset = %16X", header.sInfoOffset);
	ConLog.Write("versionInfoOffset = %16X", header.versionInfoOffset);
	ConLog.Write("controlInfoOffset = %16X", header.controlInfoOffset);
	ConLog.Write("controlInfoSize = %16X", header.controlInfoSize);
	ConLog.Write("app = %16X", header.app);

	Elf64_Ehdr elfHeader;

	if(header.elfHeaderOffset > (u64)elf.Length())
	{
		elf.Seek(header.elfHeaderOffset);
		elf.Read(&elfHeader, sizeof( Elf64_Ehdr ));
	}
	else
	{
		ConLog.Error("Elf header too large! (%d)", header.elfHeaderOffset);

		elfHeader = *new Elf64_Ehdr();
	}

	
	elf.Seek(0);
	u8	ident[16] = {0};
	elf.Read(ident, sizeof(ident));
	elf.Seek(0);

	uint mem_pc = 0;
	if(ident[4] == 1)
	{
		Elf32_Ehdr header;
	
		elf.Read(&header, sizeof(Elf32_Ehdr));

		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*/

		mem_pc = LoadElf32(header, elf);
	}
	else if(ident[4] == 2)
	{

		Elf64_Ehdr header;
	
		elf.Read(&header, sizeof(Elf64_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}*/

		mem_pc = LoadElf64(header, elf);
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", ident[4]);
		return;
	}

	/*
	Elf64_Shdr elfSectionHeaders[100];

	elf.Seek(header.elfSectionHeadersOffset);
	elf.Read(elfSectionHeaders, sizeof( Elf64_Shdr ) * elfHeader.e_shnum);

	Elf64_Phdr elfProgramHeaders[100];

	elf.Seek(header.elfProgramHeadersOffset);
	elf.Read(elfProgramHeaders, sizeof( Elf64_Phdr ) * elfHeader.e_phnum);
	*/

	if(mem_pc == 0)
	{
		//const uint data_pc = 0;//0x200;
		const uint mem_size = length - data_pc;
		elf.Seek(data_pc);
		elf.Read(&Memory.MainRam[mem_pc], mem_size);
	}

	elf.Close();

	MemoryViewerPanel* memory_viewer = new MemoryViewerPanel(NULL);

	memory_viewer->SetPC(mem_pc);
	memory_viewer->Show();
	memory_viewer->ShowPC();

	elf.Close();
}

ElfLoader elf_loader;