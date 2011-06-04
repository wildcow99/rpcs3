#include "stdafx.h"
#include "ElfLoader.h"
#include "rpcs3.h"

#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

template<typename T>
static wxString from32toString(const T from)
{
	const wxString f0 = wxString::Format("%c", (char)( from >> 24));
	const wxString f1 = wxString::Format("%c", (char)((from >> 16) & 0x00ffffff));
	const wxString f2 = wxString::Format("%c", (char)((from >>  8) & 0x0000ffff));
	const wxString f3 = wxString::Format("%c", (char)( from		   & 0x000000ff));

	return f3 + f2 + f1 + f0;
}

template<typename T>
static wxString from32toString(const T* from, const uint count)
{
	wxString ret = wxEmptyString;

	for(uint i=0; i<count; ++i)
	{
		ret += from32toString(from[i]);
	}

	return ret;
}

static wxString from32toString_Name(const u32* from, const uint count)
{
	wxString ret = wxEmptyString;

	for(uint i=0; i<count; ++i)
	{
		char c[4];

		c[0] = (char)( from[i] >> 24);
		c[1] = (char)((from[i] >> 16) & 0x00ffffff);
		c[2] = (char)((from[i] >>  8) & 0x0000ffff);
		c[3] = (char)( from[i]        & 0x000000ff);

		for(uint a=0; a<4; ++a)
		{
			switch(c[a])
			{
				case 0xFFFFFFE2: case 0xFFFFFFA2: case 0xFFFFFF84:
					c[a] = 0;	
				break;
			};
		}

		for(int a=3; a>=0; --a)
		{
			ret += wxString::Format("%c", c[a]);
		}
	}

	return ret;
}

template<typename T>
static wxString from64toString(const T from)
{
	const wxString f0 = wxString::Format("%c", (char)(from >> 56));
	const wxString f1 = wxString::Format("%c", (char)((from >> 48) & 0x00ffffffffffffff));
	const wxString f2 = wxString::Format("%c", (char)((from >> 40) & 0x0000ffffffffffff));
	const wxString f3 = wxString::Format("%c", (char)((from >> 32) & 0x000000ffffffffff));
	const wxString f4 = wxString::Format("%c", (char)((from >> 24) & 0x00000000ffffffff));
	const wxString f5 = wxString::Format("%c", (char)((from >> 16) & 0x0000000000ffffff));
	const wxString f6 = wxString::Format("%c", (char)((from >>  8) & 0x000000000000ffff));
	const wxString f7 = wxString::Format("%c", (char)( from		   & 0x00000000000000ff));

	return f7 + f6 + f5 + f4 + f3 + f2 + f1 + f0;
}

template<typename T>
static wxString from64toString(const T* from, const uint count)
{
	wxString ret = wxEmptyString;

	for(uint i=0; i<count; ++i)
	{
		ret += from64toString(from[i]);
	}

	return ret;
}

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

	return
		((((u16)c0) & 0xffff) << 8) |
		((((u16)c1) & 0x00ff) << 0);

	//return (u16)c0 | ((u16)c1)
}

u32 Read32(wxFile& f)
{
	const u32 c0 = Read8(f);
	const u32 c1 = Read8(f);
	const u32 c2 = Read8(f);
	const u32 c3 = Read8(f);

	return
		((((u32)c0) & 0xffffffff) << 24) |
		((((u32)c1) & 0x00ffffff) << 16) |
		((((u32)c2) & 0x0000ffff) <<  8) |
		((((u32)c3) & 0x000000ff) <<  0);
}

u32 Read64(wxFile& f)
{
	const u32 c0 = Read8(f);
	const u32 c1 = Read8(f);
	const u32 c2 = Read8(f);
	const u32 c3 = Read8(f);
	const u32 c4 = Read8(f);
	const u32 c5 = Read8(f);
	const u32 c6 = Read8(f);
	const u32 c7 = Read8(f);

	return
		((((u64)c0) & 0xffffffffffffffff) << 56) |
		((((u64)c1) & 0x00ffffffffffffff) << 48) |
		((((u64)c2) & 0x0000ffffffffffff) << 40) |
		((((u64)c3) & 0x000000ffffffffff) << 32) |
		((((u64)c4) & 0x00000000ffffffff) << 24) |
		((((u64)c5) & 0x0000000000ffffff) << 16) |
		((((u64)c6) & 0x000000000000ffff) <<  8) |
		((((u64)c7) & 0x00000000000000ff) <<  0);
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
		ConLog.Error("Error loading %s!", fpatch.c_str());
		return;
	}
	else
	{
		ConLog.Write("Loading %s!", fpatch.c_str());
	}

	wxFile psf(fpatch);

	u32 magic;
	psf.Seek(0);
	const u32 length = psf.Length();
	psf.Read(&magic, sizeof(u32));

	if(magic != 0x46535000)
	{
		ConLog.Error("%s is not PSF!", fpatch.c_str());
		return;
	}

	psf.Seek((length-sizeof(PsfHeader)) + 4);

	PsfHeader header;
	psf.Read(&header, sizeof(PsfHeader));
	psf.Close();

	CurGameInfo.name = from32toString_Name(header.name, 31);
	const wxString serial = from64toString(header.reg, 2);

	//ConLog.Write("magic: %s", from32toString(header.magic).c_str());
	ConLog.Write("name: %s", CurGameInfo.name.c_str());
	ConLog.Write("reg: %s", serial.c_str());
	const wxString comm_id = from32toString(header.comm_id, 4).c_str();
	if(!comm_id.IsEmpty()) ConLog.Write("comm id: %s", comm_id.c_str());
	//ConLog.Write("app ver: %s", from32toString(header.app_ver, 3).c_str());
	ConLog.Write("ver: %s", from32toString(header.ver, 3).c_str());

	if(serial.Length() == 9)
	{
		CurGameInfo.serial = serial(0, 4) + "-" + serial(4, 5);
	}
	else
	{
		CurGameInfo.serial = "Unknown";
	}
}

void ElfLoader::LoadElf32(Elf32_Ehdr& header, wxFile& elf)
{
	CPU.PC = 0;

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
}

void ElfLoader::LoadElf64(Elf64_Ehdr& header, wxFile& elf)
{
	CPU.PC = 0;

	ConLog.Write("Class: ELF64");
	ConLog.Write("Entry point address: 0x%08x", header.entry);
	ConLog.Write("Program headers offset: %d", header.phoff);
	ConLog.Write("Section headers offset: %d", header.shoff);
	ConLog.Write("Flags: %x", header.flags);
	ConLog.Write("Size of this header: %d", header.ehsize);
	ConLog.Write("Size of program headers: %d", header.phentsize);
	ConLog.Write("Number of program headers: %d", header.phnum);
	ConLog.Write("Size of section headers: %d", header.shentsize);
	ConLog.Write("Number of section headers: %d", header.shnum);
	ConLog.Write("Section header string table index: %d", header.shstrndx);

	int offset = header.phoff;

	for(uint i=0; i<header.phnum; ++i)
	{
		Elf64_Phdr phdr;

		elf.Seek(offset);
		phdr.p_type = Read32(elf);
		phdr.p_flags = Read32(elf);

		phdr.p_offset = Read64(elf);
		phdr.p_vaddr = Read64(elf);
		phdr.p_paddr = Read64(elf);
		phdr.p_filesz = Read64(elf);
		phdr.p_memsz = Read64(elf);
		phdr.p_align = Read64(elf);

		offset += sizeof(phdr);

		const uint size = phdr.p_filesz;

		switch(phdr.p_type)
		{
		case 0x1:
			ConLog.Write( L"LOAD" );
			//if(phdr.p_offset >= length) continue;

			if (phdr.p_vaddr != phdr.p_paddr)
			{
				ConLog.Warning( "ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x", 
					phdr.p_paddr, phdr.p_vaddr);
			}

			elf.Seek(phdr.p_offset);
			ConLog.Write("addr = %08X", phdr.p_paddr);
			elf.Read(&Memory.MainRam[phdr.p_paddr], size);

			if(CPU.PC == 0) CPU.PC = phdr.p_vaddr;
		break;

		case 0x7: ConLog.Write( L"TLS" );	 break;
		case 0x60000001: ConLog.Write( L"LOOS+1" ); break;
		case 0x60000002: ConLog.Write( L"LOOS+2" ); break;

		default: ConLog.Warning("Unknown phdr type! (%X)", phdr.p_type); break;
		}
	}
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
	elf_size = length;

	if(length == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		return;
	}

	const uint size = wxULongLong(length).GetLo();
	const uint csize = size * sizeof(u8);

	elf.Seek(0);
	u8	ident[16] = {0};
	elf.Read(ident, sizeof(ident));
	elf.Seek(0);

	if(ident[4] == 1)
	{
		ConLog.Error("ELF32");
		/*
		Elf32_Ehdr header;
	
		elf.Read(&header, sizeof(Elf32_Ehdr));

		if
		( 
			header.Magic[0] != 0x7F || header.Magic[1] != 0x45 ||
			header.Magic[2] != 0x4C || header.Magic[3] != 0x46
		)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}

		LoadElf32(header, elf);
		*/
	}
	else if(ident[4] == 2)
	{
	
		Elf64_Ehdr header;

		elf.Read(header.Magic, sizeof(header.Magic));
		elf.Read(header.unknown, sizeof(header.unknown));
		header.entry = Read32(elf);
		header.phoff = Read64(elf);
		header.shoff = Read64(elf);

		header.flags = Read32(elf);
		header.ehsize = Read16(elf);
		header.phentsize = Read16(elf);

		header.phnum = Read16(elf);
		header.shentsize = Read16(elf);
		header.shnum = Read16(elf);

		header.shstrndx = Read16(elf);

		if
		( 
			header.Magic[0] != 0x7F || header.Magic[1] != 0x45 ||
			header.Magic[2] != 0x4C || header.Magic[3] != 0x46
		)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}

		LoadElf64(header, elf);
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", ident[4]);
		return;
	}

	if(CPU.PC == 0)
	{
		//const uint data_pc = 0x200;
		const uint mem_size = length - data_pc;
		elf_size = mem_size;

		elf.Seek(sizeof(Elf64_Ehdr) + 4);
		elf.Read(&Memory.MainRam[0], mem_size);
	}

	elf.Close();

	LoadPsf();
}

void ElfLoader::LoadSelf()
{
}

ElfLoader elf_loader;