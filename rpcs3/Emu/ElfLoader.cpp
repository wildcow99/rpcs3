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
	CurGameInfo.root = wxFileName(wxFileName(m_elf_fpatch).GetPath()).GetPath();
}

void ElfLoader::LoadPsf()
{
	const wxString dir = wxFileName(wxFileName(m_elf_fpatch).GetPath()).GetPath();
	const wxString fpatch = dir + "\\" + "PARAM.SFO";

	if(!wxFile::Access(fpatch, wxFile::read))
	{
		ConLog.Error("Error loading %s: not found!", fpatch.c_str());
		return;
	}
	else
	{
		ConLog.Write("Loading %s...", fpatch.c_str());
	}

	wxFile psf(fpatch);

	u32 magic;
	psf.Seek(0);
	const u32 length = psf.Length();
	psf.Read(&magic, sizeof(u32));

	if(magic != 0x46535000)
	{
		ConLog.Error("%s is not PSF!", fpatch.c_str());
		psf.Close();
		return;
	}

	psf.Seek((length-sizeof(PsfHeader)) + 4);

	PsfHeader header;
	psf.Read(&header, sizeof(PsfHeader));
	psf.Close();

	CurGameInfo.name = from32toString_Name(header.name, 31);
	const wxString serial = from64toString(header.reg, 2);

	ConLog.Write("Name: %s", CurGameInfo.name.c_str());
	ConLog.Write("Serial: %s", serial.c_str());
	ConLog.Write("Version: %s", from32toString(header.ver, 3).c_str());

	const wxString comm_id = from32toString(header.comm_id, 4).c_str();
	if(!comm_id.IsEmpty()) ConLog.Write("ID: %s", comm_id.c_str());

	ConLog.SkipLn();

	if(serial.Length() == 9)
	{
		CurGameInfo.serial = serial(0, 4) + "-" + serial(4, 5);
	}
	else
	{
		CurGameInfo.serial = "Unknown";
	}
}

void ElfLoader::LoadElf32(wxFile& f)
{
	CPU.PC = 0;

	Elf32_Ehdr ehdr;
	ehdr.Load(f);

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	f.Seek(ehdr.e_phoff);

	LoadPhdr32(f, ehdr);
}

void ElfLoader::LoadElf64(wxFile& f)
{
	CPU.PC = 0;

	Elf64_Ehdr ehdr;
	ehdr.Load(f);

	ConLog.SkipLn();
	ehdr.Show();
	ConLog.SkipLn();

	f.Seek(ehdr.e_phoff);

	LoadPhdr64(f, ehdr);
}

void ElfLoader::LoadPhdr32(wxFile& f, Elf32_Ehdr& ehdr, const uint offset)
{
	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		Elf32_Phdr phdr;
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

			const wxFileOffset last_pos = f.Tell();

			f.Seek(offset + phdr.p_offset);

			u32 addr = phdr.p_paddr;
			u8* mem = Memory.GetMem(addr);
			f.Read(&mem[addr], phdr.p_filesz);

			f.Seek(last_pos);
			
			if(CPU.PC == 0 || phdr.p_paddr < CPU.PC)
			{
				CPU.PC = phdr.p_paddr;
			}
		}

		ConLog.SkipLn();
	}
}

void ElfLoader::LoadPhdr64(wxFile& f, Elf64_Ehdr& ehdr, const uint offset)
{
	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		Elf64_Phdr phdr;
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

			const wxFileOffset last_pos = f.Tell();

			f.Seek(offset + phdr.p_offset);

			u32 addr = phdr.p_paddr;
			u8* mem = Memory.GetMem(addr);
			f.Read(&mem[addr], phdr.p_filesz);

			f.Seek(last_pos);
			
			if(CPU.PC == 0 || phdr.p_paddr < CPU.PC)
			{
				CPU.PC = phdr.p_paddr;
			}
		}

		ConLog.SkipLn();
	}
}

void ElfLoader::LoadElf() //TODO
{
	const wxString elf_name = wxFileName(m_elf_fpatch).GetFullName();

	if(!wxFile::Access(m_elf_fpatch, wxFile::read))
	{
		ConLog.Error("Error open %s: not found!", elf_name);
		return;
	}

	wxFile f(m_elf_fpatch);
	if(!f.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return;
	}

	elf_size = f.Length();

	if(elf_size == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		f.Close();
		return;
	}

	f.Seek(0);

	const u32 magic = Read32(f);
	const u8 _class = Read8 (f);

	if(magic != 0x7F454C46)
	{
		ConLog.Error("%s is not correct elf!", elf_name);
		f.Close();
		return;
	}

	f.Seek(0);

	if(_class == 1)
	{
		LoadElf32(f);
	}
	else if(_class == 2)
	{
		LoadElf64(f);
	}
	else
	{
		f.Seek(0);
		f.Read(&Memory.MainRam[0], elf_size);

		ConLog.Error("Unknown elf class! (%d)", _class);
	}

	f.Close();

	LoadPsf();
}

void ElfLoader::LoadSelf()
{
	wxFile f(m_elf_fpatch);
	const wxString self_name = wxFileName(m_elf_fpatch).GetFullName();
	if(!f.IsOpened())
	{
		ConLog.Error("Error open %s!", self_name);
		return;
	}

	elf_size = f.Length();

	if(elf_size == 0)
	{
		ConLog.Error("%s is null!", self_name);
		f.Close();
		return;
	}

	SelfHeader sehdr;
	sehdr.Load(f);

	if(!sehdr.IsCorrect())
	{
		ConLog.Error("%s is not correct self! (0x%08x)", self_name, sehdr.se_magic);
		f.Close();
		return;
	}

	sehdr.Show();

	f.Seek(sehdr.se_elf);

	const u32 magic = Read32(f);
	const u8 _class = Read8 (f);

	if(magic != 0x7F454C46)
	{
		ConLog.Error("%s has no correct elf!", self_name);
		f.Close();
		return;
	}

	f.Seek(sehdr.se_elf);

	CPU.PC = 0;

	if(_class == 1)
	{
		Elf32_Ehdr ehdr;
		ehdr.Load(f);

		ConLog.SkipLn();
		ehdr.Show();
		ConLog.SkipLn();

		f.Seek(sehdr.se_phdr);

		LoadPhdr32(f, ehdr, sehdr.se_elf);
	}
	else if(_class == 2)
	{
		Elf64_Ehdr ehdr;
		ehdr.Load(f);

		ConLog.SkipLn();
		ehdr.Show();
		ConLog.SkipLn();

		f.Seek(sehdr.se_phdr);

		LoadPhdr64(f, ehdr, sehdr.se_elf);
	}
	else
	{
		f.Seek(0);
		f.Read(&Memory.MainRam[0], elf_size);

		ConLog.Error("Unknown elf class! (%d)", _class);
	}

	f.Close();

	LoadPsf();
}

ElfLoader elf_loader;