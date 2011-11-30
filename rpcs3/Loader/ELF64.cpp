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

bool ELF64Loader::Load()
{
	if(!elf64_f.IsOpened()) return false;

	if(!LoadEhdr()) return false;
	if(!LoadPhdr()) return false;
	if(!LoadShdr()) return false;

	return true;
}

bool ELF64Loader::Close()
{
	return elf64_f.Close();
}

bool ELF64Loader::LoadEhdr()
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

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	return true;
}

bool ELF64Loader::LoadPhdr()
{
	phdr_arr.Clear();

	if(ehdr.e_phoff == 0 && ehdr.e_phnum)
	{
		ConLog.Error("LoadPhdr64 error: Program header offset is null!");
		return false;
	}

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		elf64_f.Seek(ehdr.e_phoff + (ehdr.e_phentsize * i));
		Elf64_Phdr phdr;
		phdr.Load(elf64_f);
		phdr.Show();

		if(phdr.p_type == 0x00000001) //LOAD
		{
			if (phdr.p_vaddr != phdr.p_paddr)
			{
				ConLog.Warning
				( 
					"ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x", 
					phdr.p_paddr, phdr.p_vaddr
				);
			}

			elf64_f.Seek(phdr.p_offset);

			MemoryBlock& mem = Memory.GetMemByAddr(phdr.p_paddr);
			elf64_f.Read(mem.GetMemFromAddr(mem.FixAddr(phdr.p_paddr)), phdr.p_filesz);
		}

		ConLog.SkipLn();

		phdr_arr.Add(new Elf64_Phdr(phdr));
	}

	return true;
}

bool ELF64Loader::LoadShdr()
{
	shdr_arr.Clear();
	shdr_name_arr.Clear();
	if(ehdr.e_shoff == 0 && ehdr.e_shnum)
	{
		ConLog.Error("LoadShdr64 error: Section header offset is null!");
		return false;
	}

	Memory.MemFlags.Clear();

	Elf64_Shdr* strtab	= NULL;
	Elf64_Shdr* symtab	= NULL;
	Elf64_Shdr* opd		= NULL;
	Elf64_Shdr* got		= NULL;

	for(uint i=0; i<ehdr.e_shnum; ++i)
	{
		Elf64_Shdr& shdr = *new Elf64_Shdr();
		elf64_f.Seek(ehdr.e_shoff + (ehdr.e_shentsize * i));
		shdr.Load(elf64_f);

		shdr_arr.Add(shdr);

		if(ehdr.e_shstrndx == i && shdr.sh_type == SHT_STRTAB)
		{
			if(!strtab) strtab = &shdr;
		}
		else if(shdr.sh_type == SHT_SYMTAB)
		{
			if(!symtab) symtab = &shdr;
		}
	}

	for(uint i=0; i<shdr_arr.GetCount(); ++i)
	{
		Elf64_Shdr& shdr = shdr_arr[i];
		if(strtab)
		{
			elf64_f.Seek(strtab->sh_offset + shdr.sh_name);
			wxString name = wxEmptyString;
			while(!elf64_f.Eof())
			{
				char c;
				elf64_f.Read(&c, 1);
				if(c == 0) break;
				name += c;
			}

			//hack!
			if(name.Length() > 0 && name[0] != '.')
			{
				bool is_done = true;

				if(!opd)
				{
					is_done = entry >= shdr.sh_addr && entry < shdr.sh_addr + shdr.sh_size;
					if(is_done) name = ".opd";
				}

				if(!is_done) //.rodata.sceFNID
				{
					if(shdr.sh_size > 4)
					{
						is_done = true;
						const wxFileOffset pos = elf64_f.Tell();
						elf64_f.Seek(shdr.sh_offset);
						while(elf64_f.Tell() < (s64)(shdr.sh_offset+shdr.sh_size) && is_done && !elf64_f.Eof())
						{
							u32 buf;
							elf64_f.Read(&buf, 4);
							is_done = (buf >> 24) == 0x00;
						}

						elf64_f.Seek(pos);
					}

					if(is_done) name = ".rodata.sceFNID";
				}

				if(!is_done) //.data.sceFStub
				{
					if(shdr.sh_size >= 32)
					{
						const wxFileOffset pos = elf64_f.Tell();

						elf64_f.Seek(shdr.sh_offset);
						u32 buf[8];
						for(u8 i=0; i<8; ++i) elf64_f.Read(&buf[i], 4);
						
						/*
						0: 39 80 00 00 	li      r12,0
						1: 65 8c 00 03 	oris    r12,r12,X
						2: 81 8c 01 9c 	lwz     r12,X(r12)
						3: f8 41 00 28 	std     r2,40(r1)
						4: 80 0c 00 00 	lwz     r0,0(r12)
						5: 80 4c 00 04 	lwz     r2,4(r12)
						6: 7c 09 03 a6 	mtctr   r0
						7: 4e 80 04 20 	bctr
						*/

						is_done =
							buf[0] == 0x39800000 && //li    r12,0
							buf[3] == 0xf8410028 && //std   r2,40(r1)
							buf[4] == 0x800c0000 && //lwz   r0,0(r12)
							buf[5] == 0x804c0004 && //lwz   r2,4(r12)
							buf[6] == 0x7c0903a6 && //mtctr r0
							buf[7] == 0x4e800420;	//bctr

						elf64_f.Seek(pos);

					}

					if(is_done) name = ".data.sceFStub";
				}

				if(!is_done) //.text
				{
					is_done = true;
					for(uint sh=0; sh<shdr_arr.GetCount() && is_done; ++sh)
					{
						if(sh == i) continue;
						Elf64_Shdr& _shdr = shdr_arr[sh];
						is_done = shdr.sh_size > _shdr.sh_size;
					}

					if(is_done) name = ".text";
				}
			}

			shdr_name_arr.Add(name);
			ConLog.Write("Name: %s", name);

			if(!opd && !name.CmpNoCase(".opd"))
			{
				Memory.MemFlags.SetOPDRange(shdr.sh_addr, shdr.sh_size);
				opd = &shdr;
			}
			else if(!got && !name.CmpNoCase(".got"))
			{
				got = &shdr;
			}
			else if(!name.CmpNoCase(".data.sceFStub"))
			{
				Memory.MemFlags.SetFStubRange(shdr.sh_addr, shdr.sh_size);
			}
			else if(!name.CmpNoCase(".rodata.sceFNID"))
			{
				Memory.MemFlags.SetFNIDRange(shdr.sh_addr, shdr.sh_size);
			}
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

	if(opd)
	{
		const u32 offs = opd->sh_offset;
		const u32 size = opd->sh_size;

		elf64_f.Seek(offs);
		for(uint i=0; i<size; i += sizeof(s64))
		{
			Memory.MemFlags.Add(Read32(elf64_f));
			Memory.MemFlags.Add(Read32(elf64_f)); //???
		}
	}

	if(got)
	{
		const u32 offs = got->sh_offset;
		const u32 size = got->sh_size;

		elf64_f.Seek(offs);
		for(uint i=0; i<size; i += sizeof(s32))
		{
			const u32 code = Read32(elf64_f);
			if(code == 0x7f7f7f7f) break;
			Memory.MemFlags.Add(code);
		}
	}

	if(symtab)
	{
		//TODO
	}

	return true;
}