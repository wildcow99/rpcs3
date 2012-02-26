#include "stdafx.h"
#include "ELF32.h"

ELF32Loader::ELF32Loader(wxFile& f)
	: elf32_f(f)
	, LoaderBase()
{
}

ELF32Loader::ELF32Loader(const wxString& path)
	: elf32_f(*new wxFile(path))
	, LoaderBase()
{
}

bool ELF32Loader::Load()
{
	if(!elf32_f.IsOpened()) return false;

	if(!LoadEhdr()) return false;
	if(!LoadPhdr()) return false;
	if(!LoadShdr()) return false;

	return true;
}

bool ELF32Loader::Close()
{
	return elf32_f.Close();
}

bool ELF32Loader::LoadEhdr()
{
	elf32_f.Seek(0);
	ehdr.Load(elf32_f);

	if(!ehdr.CheckMagic()) return false;

	switch(ehdr.e_machine)
	{
	case MACHINE_PPC64:
	case MACHINE_SPU:
		machine = (Elf_Machine)ehdr.e_machine;
	break;

	default:
		machine = MACHINE_Unknown;
		ConLog.Error("Unknown elf32 type: 0x%x", ehdr.e_machine);
		return false;
	}

	entry = ehdr.GetEntry();
	if(entry == 0)
	{
		ConLog.Error("elf32 error: entry is null!");
		return false;
	}

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	return true;
}

bool ELF32Loader::LoadPhdr()
{
	if(ehdr.e_phoff == 0 && ehdr.e_phnum)
	{
		ConLog.Error("LoadPhdr32 error: Program header offset is null!");
		return false;
	}

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		elf32_f.Seek(ehdr.e_phoff + (ehdr.e_phentsize * i));
		Elf32_Phdr phdr;
		phdr.Load(elf32_f);
		phdr.Show();

		if(phdr.p_type == 0x00000001) //LOAD
		{
			if (phdr.p_vaddr != phdr.p_paddr)
			{
				ConLog.Warning
				( 
					"LoadPhdr32 different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x", 
					phdr.p_paddr, phdr.p_vaddr
				);
			}

			elf32_f.Seek(phdr.p_offset);
			elf32_f.Read(Memory.GetMemFromAddr(phdr.p_paddr), phdr.p_filesz);
		}

		ConLog.SkipLn();

		phdr_arr.Add(new Elf32_Phdr(phdr));
	}

	return true;
}

bool ELF32Loader::LoadShdr()
{
	shdr_arr.Clear();
	shdr_name_arr.Clear();
	if(ehdr.e_shoff == 0 && ehdr.e_shnum)
	{
		ConLog.Error("LoadShdr32 error: Section header offset is null!");
		return false;
	}

	Memory.MemFlags.Clear();

	Elf32_Shdr* strtab	= NULL;

	for(uint i=0; i<ehdr.e_shnum; ++i)
	{
		Elf32_Shdr& shdr = *new Elf32_Shdr();
		elf32_f.Seek(ehdr.e_shoff + (ehdr.e_shentsize * i));
		shdr.Load(elf32_f);

		shdr_arr.Add(shdr);

		if(ehdr.e_shstrndx == i && shdr.sh_type == SHT_STRTAB)
		{
			if(!strtab) strtab = &shdr;
		}
	}

	for(uint i=0; i<shdr_arr.GetCount(); ++i)
	{
		Elf32_Shdr& shdr = shdr_arr[i];
		if(strtab)
		{
			elf32_f.Seek(strtab->sh_offset + shdr.sh_name);
			wxString name = wxEmptyString;
			while(!elf32_f.Eof())
			{
				char c;
				elf32_f.Read(&c, 1);
				if(c == 0) break;
				name += c;
			}

			shdr_name_arr.Add(name);
			ConLog.Write("Name: %s", name);
		}

		shdr.Show();
		ConLog.SkipLn();

		if((shdr.sh_flags & SHF_ALLOC) != SHF_ALLOC) continue;

		const s64 addr = shdr.sh_addr;
		const s64 size = shdr.sh_size;
		MemoryBlock* mem = NULL;

		switch(shdr.sh_type)
		{
		case SHT_NOBITS:
			if(size == 0) continue;
			mem = &Memory.GetMemByAddr(addr);

			memset(&((u8*)mem->GetMem())[addr - mem->GetStartAddr()], 0, size);

		case SHT_PROGBITS:
		break;
		}
	}

	//TODO
	return true;
}