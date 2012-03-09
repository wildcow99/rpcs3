#include "stdafx.h"
#include "ELF64.h"

ELF64Loader::ELF64Loader(wxFile& f)
	: elf64_f(f)
	, LoaderBase()
{
}

ELF64Loader::ELF64Loader(const wxString& path)
	: elf64_f(*new wxFile(path))
	, LoaderBase()
{
}

bool ELF64Loader::LoadInfo()
{
	if(!elf64_f.IsOpened()) return false;

	if(!LoadEhdrInfo()) return false;
	if(!LoadPhdrInfo()) return false;
	if(!LoadShdrInfo()) return false;

	return true;
}

bool ELF64Loader::LoadData()
{
	if(!elf64_f.IsOpened()) return false;

	if(!LoadEhdrData()) return false;
	if(!LoadPhdrData()) return false;
	if(!LoadShdrData()) return false;

	return true;
}

bool ELF64Loader::Close()
{
	return elf64_f.Close();
}

bool ELF64Loader::LoadEhdrInfo()
{
	elf64_f.Seek(0);
	ehdr.Load(elf64_f);

	if(!ehdr.CheckMagic()) return false;

	if(ehdr.e_phentsize != sizeof(Elf64_Phdr))
	{
		ConLog.Error("elf64 error:  e_phentsize[0x%x] != sizeof(Elf64_Phdr)[0x%x]", ehdr.e_phentsize, sizeof(Elf64_Phdr));
		return false;
	}

	if(ehdr.e_shentsize != sizeof(Elf64_Shdr))
	{
		ConLog.Error("elf64 error: e_shentsize[0x%x] != sizeof(Elf64_Shdr)[0x%x]", ehdr.e_shentsize, sizeof(Elf64_Shdr));
		return false;
	}

	switch(ehdr.e_machine)
	{
	case MACHINE_PPC64:
	case MACHINE_SPU:
		machine = (Elf_Machine)ehdr.e_machine;
	break;

	default:
		machine = MACHINE_Unknown;
		ConLog.Error("Unknown elf64 type: 0x%x", ehdr.e_machine);
		return false;
	}

	entry = ehdr.GetEntry();
	if(entry == 0)
	{
		ConLog.Error("elf64 error: entry is null!");
		return false;
	}

	return true;
}

bool ELF64Loader::LoadPhdrInfo()
{
	phdr_arr.Clear();

	if(ehdr.e_phoff == 0 && ehdr.e_phnum)
	{
		ConLog.Error("LoadPhdr64 error: Program header offset is null!");
		return false;
	}

	elf64_f.Seek(ehdr.e_phoff);
	for(u32 i=0; i<ehdr.e_phnum; ++i)
	{
		Elf64_Phdr phdr;
		phdr.Load(elf64_f);
		phdr_arr.AddCpy(phdr);
	}

	return true;
}

bool ELF64Loader::LoadShdrInfo()
{
	shdr_arr.Clear();
	shdr_name_arr.Clear();
	if(ehdr.e_shoff == 0 && ehdr.e_shnum)
	{
		ConLog.Error("LoadShdr64 error: Section header offset is null!");
		return false;
	}

	elf64_f.Seek(ehdr.e_shoff);
	for(u32 i=0; i<ehdr.e_shnum; ++i)
	{
		Elf64_Shdr shdr;
		shdr.Load(elf64_f);
		shdr_arr.AddCpy(shdr);
	}

	if(ehdr.e_shstrndx >= shdr_arr.GetCount())
	{
		ConLog.Error("LoadShdr64 error: shstrndx too big!");
		return false;
	}

	for(u32 i=0; i<shdr_arr.GetCount(); ++i)
	{
		elf64_f.Seek(shdr_arr[ehdr.e_shstrndx].sh_offset + shdr_arr[i].sh_name);
		wxString name = wxEmptyString;
		while(!elf64_f.Eof())
		{
			char c;
			elf64_f.Read(&c, 1);
			if(c == 0) break;
			name += c;
		}

		shdr_name_arr.Add(name);
	}

	return true;
}

bool ELF64Loader::LoadEhdrData()
{
	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	return true;
}

bool ELF64Loader::LoadPhdrData()
{
	for(u32 i=0; i<phdr_arr.GetCount(); ++i)
	{
		phdr_arr[i].Show();

		if(phdr_arr[i].p_vaddr != phdr_arr[i].p_paddr)
		{
			ConLog.Warning
			( 
				"ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x", 
				phdr_arr[i].p_paddr, phdr_arr[i].p_vaddr
			);
		}

		switch(phdr_arr[i].p_type)
		{
			case 0x00000001: //LOAD
			{
				elf64_f.Seek(phdr_arr[i].p_offset);
				elf64_f.Read(Memory.GetMemFromAddr(phdr_arr[i].p_paddr), phdr_arr[i].p_filesz);
			}
			break;

			case 0x00000007: //TLS
				Emu.SetTLSData(phdr_arr[i].p_paddr, phdr_arr[i].p_filesz, phdr_arr[i].p_memsz);
			break;
		}

		ConLog.SkipLn();
	}

	return true;
}

bool ELF64Loader::LoadShdrData()
{
	Memory.MemFlags.Clear();

	for(uint i=0; i<shdr_arr.GetCount(); ++i)
	{
		Elf64_Shdr& shdr = shdr_arr[i];

		if(i < shdr_name_arr.GetCount())
		{
			const wxString& name = shdr_name_arr[i];
			ConLog.Write("Name: %s", shdr_name_arr[i]);

			if(!name.CmpNoCase(".opd"))
			{
				Memory.MemFlags.SetOPDRange(shdr.sh_addr, shdr.sh_size);
				const u32 offs = shdr.sh_offset;
				const u32 size = shdr.sh_size;

				elf64_f.Seek(offs);
				for(uint i=0; i<size; i += sizeof(s64))
				{
					Memory.MemFlags.Add(Read32(elf64_f));
					Memory.MemFlags.Add(Read32(elf64_f));
				}
			}
			else if(!name.CmpNoCase(".got"))
			{
				const u32 offs = shdr.sh_offset;
				const u32 size = shdr.sh_size;

				elf64_f.Seek(offs);
				for(uint i=0; i<size; i += sizeof(s32))
				{
					const u32 code = Read32(elf64_f);
					if(code == 0x7f7f7f7f) break;
					Memory.MemFlags.Add(code);
				}
			}
			else if(!name.CmpNoCase(".data.sceFStub"))
			{
				Memory.MemFlags.SetFStubRange(shdr.sh_addr, shdr.sh_size);
			}
			else if(!name.CmpNoCase(".rodata.sceFNID"))
			{
				Memory.MemFlags.SetFNIDRange(shdr.sh_addr, shdr.sh_size);
			}
			else if(!name.CmpNoCase(".sceStub.text"))
			{
				Memory.MemFlags.SetStubRange(shdr.sh_addr, shdr.sh_size);
			}
			else if(!name.CmpNoCase(".sys_proc_param"))
			{
				ConLog.Warning("section .sys_proc_param founded!");
				struct sys_process_param
				{
					u32 size;
					u32 magic;
					u32 version;
					u32 sdk_version;
					s32 primary_prio;
					u32 primary_stacksize;
					u32 malloc_pagesize;
					u32 ppc_seg;
					u32 crash_dump_param_addr;
				} &proc_param = *(sys_process_param*)Memory.GetMemFromAddr(shdr.sh_addr);

				if(Memory.Reverse32(proc_param.size) != sizeof(sys_process_param))
				{
					ConLog.Warning("Bad proc param size! [0x%x : 0x%x]", proc_param.size, sizeof(sys_process_param));
				}
				if(Memory.Reverse32(proc_param.magic) != 0x13bcc5f6)
				{
					ConLog.Error("Bad magic! [0x%x]", Memory.Reverse32(proc_param.magic));
				}
				else
				{
					ConLog.Write("*** sdk version: 0x%x", re(proc_param.sdk_version));
					ConLog.Write("*** primary prio: %d", re(proc_param.primary_prio));
					ConLog.Write("*** primary stacksize: 0x%x", re(proc_param.primary_stacksize));
					ConLog.Write("*** malloc pagesize: 0x%x", re(proc_param.malloc_pagesize));
					ConLog.Write("*** ppc seg: 0x%x", re(proc_param.ppc_seg));
					ConLog.Write("*** crash dump param addr: 0x%x", re(proc_param.crash_dump_param_addr));

					Emu.SetMallocPageSize(re(proc_param.malloc_pagesize));
				}
			}
			//else if(!name.CmpNoCase(".lib.stub"))
			//{
			//	Memory.MemFlags.SetStubRange(shdr.sh_addr, shdr.sh_size);
			//}
		}

		if(shdr.sh_type == SHT_SYMTAB)
		{
			//TODO
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

	return true;
}