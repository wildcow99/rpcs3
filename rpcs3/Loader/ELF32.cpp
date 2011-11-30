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

			MemoryBlock& mem = Memory.GetMemByAddr(phdr.p_paddr);
			elf32_f.Read(mem.GetMemFromAddr(mem.FixAddr(phdr.p_paddr)), phdr.p_filesz);
		}

		ConLog.SkipLn();

		phdr_arr.Add(new Elf32_Phdr(phdr));
	}

	return true;
}

bool ELF32Loader::LoadShdr()
{
	//TODO
	return true;
}