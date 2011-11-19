#include "stdafx.h"
#include "ElfLoader.h"
#include "rpcs3.h"

#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

u8 Read8(wxFile& f)
{
	u8 ret;
	f.Read(&ret, sizeof(u8));
	return ret;
}

u16 Read16(wxFile& f)
{
	const u8 c0 = Read8(f);
	const u8 c1 = Read8(f);

	return ((u16)c0 << 8) | (u16)c1;
}

u32 Read32(wxFile& f)
{
	const u16 c0 = Read16(f);
	const u16 c1 = Read16(f);

	return ((u32)c0 << 16) | (u32)c1;
}

u64 Read64(wxFile& f)
{
	const u32 c0 = Read32(f);
	const u32 c1 = Read32(f);

	return ((u64)c0 << 32) | (u64)c1;
}

void ElfLoader::SetElf(wxString elf_full_patch)
{
	m_elf_fpatch = elf_full_patch;
	CurGameInfo.root = wxFileName(wxFileName(elf_full_patch).GetPath()).GetPath();
}

bool ElfLoader::LoadPsf()
{
	const wxString dir = wxFileName(wxFileName(m_elf_fpatch).GetPath()).GetPath();
	const wxString fpatch = dir + "\\" + "PARAM.SFO";

	if(!wxFile::Access(fpatch, wxFile::read))
	{
		ConLog.Error("Error loading %s: not found!", fpatch.c_str());
		return false;
	}
	else
	{
		ConLog.Write("Loading %s...", fpatch.c_str());
	}

	wxFile psf(fpatch);

	PsfHeader pshdr;
	psf.Read(&pshdr, sizeof(PsfHeader));

	if(pshdr.ps_magic != 0x46535000)
	{
		ConLog.Error("%s is not PSF!", fpatch);
		psf.Close();
		return false;
	}

	psf.Seek(pshdr.ps_listoff);
	wxArrayString list;
	list.Add(wxEmptyString);

	while(!psf.Eof())
	{
		char c;
		psf.Read(&c, 1);
		if(c == 0)
		{
			psf.Read(&c, 1);
			if(c == 0) break;

			list.Add(wxEmptyString);
		}
		list[list.GetCount() - 1].Append(c);
	}

	psf.Seek(pshdr.ps_soffset);

	wxString sb = wxEmptyString;
	wxString serial = wxEmptyString;

	struct PsfHelper
	{
		static wxString ReadString(wxFile& f, const u32 size)
		{
			wxString ret = wxEmptyString;

			for(uint i=0; i<size && !f.Eof(); ++i)
			{
				ret += ReadChar(f);
			}

			return ret;
		}

		static wxString ReadString(wxFile& f)
		{
			wxString ret = wxEmptyString;

			while(!f.Eof())
			{
				const char c = ReadChar(f);
				if(c == 0) break;
				ret += c;
			}

			return ret;
		}

		static char ReadChar(wxFile& f)
		{
			char c;
			f.Read(&c, 1);
			return c;
		}

		static char ReadCharNN(wxFile& f)
		{
			char c;
			while(!f.Eof())
			{
				f.Read(&c, 1);
				if(c != 0) break;
			}
			
			return c;
		}

		static void GoToNN(wxFile& f)
		{
			while(!f.Eof())
			{
				char c;
				f.Read(&c, 1);
				if(c != 0)
				{
					f.Seek(f.Tell() - 1);
					break;
				}
			}
		}

		static wxString ToData(const wxString& str)
		{
			return wxString::Format(": %x", str);
		}
	};

	for(uint i=0;i<list.GetCount(); i++)
	{
		u32 buf;

		if(!list[i].Cmp("TITLE_ID"))
		{
			serial = PsfHelper::ReadString(psf);
			list[i].Append(wxString::Format(": %s", serial));
			PsfHelper::GoToNN(psf);
		}
		else if(!list[i](0, 5).Cmp("TITLE"))
		{
			CurGameInfo.name = FixForName(PsfHelper::ReadString(psf));
			list[i].Append(wxString::Format(": %s", CurGameInfo.name));
			PsfHelper::GoToNN(psf);
		}
		else if(!list[i].Cmp("APP_VER"))
		{
			list[i].Append(wxString::Format(": %s", PsfHelper::ReadString(psf, sizeof(u64))));
		}
		else if(!list[i].Cmp("ATTRIBUTE"))
		{
			psf.Read(&buf, sizeof(buf));
			list[i].Append(wxString::Format(": 0x%x", buf));
		}
		else if(!list[i].Cmp("CATEGORY"))
		{
			list[i].Append(wxString::Format(": %s", PsfHelper::ReadString(psf, sizeof(u32))));
		}
		else if(!list[i].Cmp("BOOTABLE"))
		{			
			psf.Read(&buf, sizeof(buf));
			list[i].Append(wxString::Format(": %s", buf ? "true" : "false"));
		}
		else if(!list[i].Cmp("LICENSE"))
		{
			list[i].Append(wxString::Format(": %s", PsfHelper::ReadString(psf)));
			psf.Seek(psf.Tell() + (sizeof(u64) * 7 * 2) - 1);
		}
		else if(!list[i](0, 14).Cmp("PARENTAL_LEVEL"))
		{
			psf.Read(&buf, sizeof(buf));
			list[i].Append(wxString::Format(": %d", buf));
		}
		else if(!list[i].Cmp("PS3_SYSTEM_VER"))
		{
			list[i].Append(wxString::Format(": %s", PsfHelper::ReadString(psf, sizeof(u64))));
		}
		else if(!list[i].Cmp("SOUND_FORMAT"))
		{
			list[i].Append(wxString::Format(": 0x%x", Read32(psf)));
		}
		else if(!list[i].Cmp("RESOLUTION"))
		{
			list[i].Append(wxString::Format(": 0x%x", Read32(psf)));
		}
		else
		{
			list[i].Append(wxString::Format(": %s", PsfHelper::ReadString(psf)));
			PsfHelper::GoToNN(psf);
		}
	}

	psf.Close();

	ConLog.SkipLn();
	for(uint i=0; i<list.GetCount(); ++i)
	{
		ConLog.Write("%s", list[i]);
	}
	ConLog.SkipLn();

	if(serial.Length() == 9)
	{
		CurGameInfo.serial = serial(0, 4) + "-" + serial(4, 5);
	}
	else
	{
		CurGameInfo.serial = "Unknown";
	}

	return true;
}

bool ElfLoader::LoadElf32(wxFile& f)
{
	Elf32_Ehdr ehdr;
	ehdr.Load(f);

	isSPU = ehdr.e_machine == 0x17;
	entry = ehdr.e_entry;

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	return LoadPhdr32(f, ehdr);
}

bool ElfLoader::LoadElf64(wxFile& f)
{
	Elf64_Ehdr ehdr;
	ehdr.Load(f);

	isSPU = ehdr.e_machine == 0x17;
	entry = ehdr.e_entry;

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	if(!LoadPhdr64(f, ehdr)) return false;
	if(!LoadShdr64(f, ehdr)) return false;
	return true;
}

bool ElfLoader::LoadPhdr32(wxFile& f, Elf32_Ehdr& ehdr, const uint offset)
{
	if(ehdr.e_phoff == 0)
	{
		ConLog.Error("LoadPhdr32 error: Program header offset is null!");
		return false;
	}

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		Elf32_Phdr phdr;
		f.Seek(offset + ehdr.e_phoff + (ehdr.e_phentsize * i));
		phdr.Load(f);
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

			f.Seek(offset + phdr.p_offset);

			MemoryBlock& mem = Memory.GetMemByAddr(phdr.p_paddr);
			f.Read(mem.GetMemFromAddr(phdr.p_paddr - mem.GetStartAddr()), phdr.p_filesz);
		}

		ConLog.SkipLn();
	}

	return true;
}

bool ElfLoader::LoadPhdr64(wxFile& f, Elf64_Ehdr& ehdr, const uint offset)
{
	if(ehdr.e_phoff == 0)
	{
		ConLog.Error("LoadPhdr64 error: Program header offset is null!");
		return false;
	}

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		Elf64_Phdr phdr;
		f.Seek(offset + ehdr.e_phoff + (ehdr.e_phentsize * i));
		phdr.Load(f);
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

			f.Seek(offset + phdr.p_offset);

			MemoryBlock& mem = Memory.GetMemByAddr(phdr.p_paddr);
			f.Read(mem.GetMemFromAddr(phdr.p_paddr - mem.GetStartAddr()), phdr.p_filesz);
		}

		ConLog.SkipLn();
	}

	return true;
}

bool ElfLoader::LoadShdr64(wxFile& f, Elf64_Ehdr& ehdr, const uint offset)
{
	if(ehdr.e_shoff == 0 && ehdr.e_shnum > 0)
	{
		ConLog.Error("LoadShdr64 error: Section header offset is null!");
		return false;
	}

	m_sh_ptr.Clear();
	Memory.MemFlags.Clear();

	Elf64_Shdr* strtab	= NULL;
	Elf64_Shdr* symtab	= NULL;
	Elf64_Shdr* opd		= NULL;
	Elf64_Shdr* got		= NULL;

	for(uint i=0; i<ehdr.e_shnum; ++i)
	{
		Elf64_Shdr* shdr = new Elf64_Shdr();
		f.Seek(offset + ehdr.e_shoff + (ehdr.e_shentsize * i));
		shdr->Load(f);

		m_sh_ptr.Add(shdr);

		if(ehdr.e_shstrndx == i && shdr->sh_type == SHT_STRTAB)
		{
			if(!strtab) strtab = shdr;
		}
		else if(shdr->sh_type == SHT_SYMTAB)
		{
			if(!symtab) symtab = shdr;
		}
	}

	for(uint i=0; i<m_sh_ptr.GetCount(); ++i)
	{
		Elf64_Shdr& shdr = *(Elf64_Shdr*)m_sh_ptr[i];
		if(strtab)
		{
			f.Seek(strtab->sh_offset + shdr.sh_name);
			wxString name = wxEmptyString;
			while(!f.Eof())
			{
				char c;
				f.Read(&c, 1);
				if(c == 0) break;
				name += c;
			}

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

		const s64 addr = /*offset + */shdr.sh_addr;
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

		f.Seek(offs);
		for(uint i=0; i<size; i += sizeof(s64))
		{
			Memory.MemFlags.Add(Read32(f));
			Memory.MemFlags.Add(Read32(f)); //???
		}
	}

	if(got)
	{
		const u32 offs = got->sh_offset;
		const u32 size = got->sh_size;

		f.Seek(offs);
		for(uint i=0; i<size; i += sizeof(s32))
		{
			const u32 code = Read32(f);
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

bool ElfLoader::LoadElf()
{
	const wxString elf_name = wxFileName(m_elf_fpatch).GetFullName();

	if(!wxFile::Access(m_elf_fpatch, wxFile::read))
	{
		ConLog.Error("Error open %s: not found!", elf_name);
		return false;
	}

	wxFile f(m_elf_fpatch);
	if(!f.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return false;
	}

	elf_size = f.Length();

	if(elf_size == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		f.Close();
		return false;
	}

	f.Seek(0);

	const u32 magic = Read32(f);
	const u8 _class = Read8 (f);

	if(magic != 0x7F454C46)
	{
		ConLog.Error("%s is not correct elf!", elf_name);
		f.Close();
		return false;
	}

	f.Seek(0);

	bool ret = true;
	if(_class == 1)
	{
		if(!LoadElf32(f)) ret = false;
	}
	else if(_class == 2)
	{
		if(!LoadElf64(f)) ret = false;
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", _class);

		f.Read(Memory.MainRam.GetMem(), elf_size);
	}

	f.Close();

	if(ret) LoadPsf();

	return ret;
}

bool ElfLoader::LoadSelf()
{
	wxFile f(m_elf_fpatch);
	const wxString self_name = wxFileName(m_elf_fpatch).GetFullName();
	if(!f.IsOpened())
	{
		ConLog.Error("Error open %s!", self_name);
		return false;
	}

	elf_size = f.Length();

	if(elf_size == 0)
	{
		ConLog.Error("%s is null!", self_name);
		f.Close();
		return false;
	}

	SelfHeader sehdr;
	sehdr.Load(f);

	if(!sehdr.IsCorrect())
	{
		ConLog.Error("%s is not correct self! (0x%08x)", self_name, sehdr.se_magic);
		f.Close();
		return false;
	}

	sehdr.Show();

	f.Seek(sehdr.se_elf);

	const u32 magic = Read32(f);
	const u8 _class = Read8 (f);

	if(magic != 0x7F454C46)
	{
		ConLog.Error("%s has no correct elf!", self_name);
		f.Close();
		return false;
	}

	f.Seek(sehdr.se_elf);

	bool ret = true;
	if(_class == 1)
	{
		Elf32_Ehdr ehdr;
		ehdr.Load(f);

		entry = ehdr.e_entry;

		ConLog.SkipLn();
		ehdr.Show();
		ConLog.SkipLn();

		f.Seek(sehdr.se_phdr);

		if(!LoadPhdr32(f, ehdr, sehdr.se_elf)) ret = false;
	}
	else if(_class == 2)
	{
		Elf64_Ehdr ehdr;
		ehdr.Load(f);

		entry = ehdr.e_entry;

		ConLog.SkipLn();
		ehdr.Show();
		ConLog.SkipLn();

		f.Seek(sehdr.se_phdr);

		if(!LoadPhdr64(f, ehdr, sehdr.se_elf)) ret = false;
	}
	else
	{
		f.Seek(0);
		f.Read(Memory.MainRam.GetMem(), elf_size);

		ConLog.Error("Unknown elf class! (%d)", _class);
	}

	f.Close();

	if(ret) LoadPsf();

	return ret;
}

bool ElfLoader::Load(bool IsSelf)
{
	if(IsSelf) return LoadSelf();
	return LoadElf();
}

ElfLoader Loader;