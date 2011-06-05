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

u64 Read64(wxFile& f)
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

static const wxChar* DataToString(const u8 data)
{
	if(data > 1) return wxString::Format("%d's complement, big endian", data);
	if(data == 0) return "Data is not found";

	return wxString::Format("%d's complement, small endian", data);
}

static const wxChar* TypeToString(const u16 type)
{
	switch(type)
	{
	case 0:  return "NULL";
	case 2:  return "EXEC (Executable file)";
	default: ConLog.Error("Unknown type (%d)", type); break;
	};

	return wxString::Format("Unknown (%d)", type);
}

static const wxChar* OS_ABIToString(const u8 os_abi)
{
	switch(os_abi)
	{
	case 0x0 :  return "UNIX System V";
	case 0x66:  return "Cell OS LV-2";
	default: ConLog.Error("Unknown OS/ABI (%x)", os_abi); break;
	};

	return wxString::Format("Unknown (%x)", os_abi);
}

static const wxChar* MachineToString(const u16 machine)
{
	switch(machine)
	{
	case 0x15:  return "PowerPC64";
	case 0x17:  return "SPU";
	default: ConLog.Error("Unknown machine (%x)", machine); break;
	};

	return wxString::Format("Unknown (%x)", machine);
}

static const wxChar* Phdr_FlagsToString(const u32 flags)
{
	switch(flags)
	{
	case 0x4:  return "R";
	case 0x7:  return "RWE";
	default: ConLog.Error("Unknown flags (%x)", flags); break;
	};

	return wxString::Format("Unknown (%x)", flags);
}

static const wxChar* Phdr_TypeToString(const u32 type)
{
	switch(type)
	{
	case 0x00000001: return "LOAD";
	case 0x00000004: return "NOTE";
	case 0x00000007: return "TLS";
	case 0x60000001: return "LOOS+1";
	case 0x60000002: return "LOOS+2";
	default: ConLog.Error("Unknown type (%x)", type); break;
	};

	return wxString::Format("Unknown (%x)", type);
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

void ElfLoader::LoadElf32(Elf32_Ehdr& ehdr, wxFile& f)
{
	CPU.PC = 0;

	ConLog.SkipLn();
	ConLog.Write("Magic: %08x",							  ehdr.e_magic);
	ConLog.Write("Class: %s",							  "ELF32");
	ConLog.Write("Data: %s",							  DataToString(ehdr.e_data));
	ConLog.Write("Current Version: %d",					  ehdr.e_curver);
	ConLog.Write("OS/ABI: %s",							  OS_ABIToString(ehdr.e_os_abi));
	ConLog.Write("Type: %s",							  TypeToString(ehdr.e_type));
	ConLog.Write("Machine: %s",							  MachineToString(ehdr.e_machine));
	ConLog.Write("Version: %d",							  ehdr.e_version);
	ConLog.Write("Entry point address: 0x%x",			  ehdr.e_entry);
	ConLog.Write("Program headers offset: %d",			  ehdr.e_phoff);
	ConLog.Write("Section headers offset: %d",			  ehdr.e_shoff);
	ConLog.Write("Flags: 0x%x",							  ehdr.e_flags);
	ConLog.Write("Size of this header: %d",				  ehdr.e_ehsize);
	ConLog.Write("Size of program headers: %d",			  ehdr.e_phentsize);
	ConLog.Write("Number of program headers: %d",		  ehdr.e_phnum);
	ConLog.Write("Size of section headers: %d",			  ehdr.e_shentsize);
	ConLog.Write("Number of section headers: %d",		  ehdr.e_shnum);
	ConLog.Write("Section header string table index: %d", ehdr.e_shstrndx);

	ConLog.SkipLn();

	int offset = ehdr.e_phoff;

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		f.Seek(offset);

		Elf32_Phdr phdr;
		phdr.p_type		= Read32(f);
		phdr.p_offset	= Read32(f);
		phdr.p_vaddr	= Read32(f);
		phdr.p_paddr	= Read32(f);
		phdr.p_filesz	= Read32(f);
		phdr.p_memsz	= Read32(f);
		phdr.p_flags	= Read32(f);
		phdr.p_align	= Read32(f);

		offset += sizeof(phdr);

		ConLog.Write("Type: %s", Phdr_TypeToString(phdr.p_type));
		ConLog.Write("Offset: 0x%08x", phdr.p_offset);
		ConLog.Write("Virtual address: 0x%08x", phdr.p_vaddr);
		ConLog.Write("Physical address: 0x%08x", phdr.p_paddr);
		ConLog.Write("File size: 0x%08x", phdr.p_filesz);
		ConLog.Write("Memory size: 0x%08x", phdr.p_memsz);
		ConLog.Write("Flags: %s", Phdr_FlagsToString(phdr.p_flags));
		ConLog.Write("Algin: 0x%x", phdr.p_align);

		if (phdr.p_vaddr != phdr.p_paddr)
		{
			ConLog.Warning
			( 
				"ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x", 
				phdr.p_paddr, phdr.p_vaddr
			);
		}

		if(phdr.p_type == 0x00000001) //LOAD
		{
			f.Seek(phdr.p_offset);
			f.Read(&Memory.MainRam[phdr.p_paddr], phdr.p_filesz);
		}

		ConLog.SkipLn();
	}
}

void ElfLoader::LoadElf64(Elf64_Ehdr& ehdr, wxFile& f)
{
	CPU.PC = 0;

	ConLog.SkipLn();
	ConLog.Write("Magic: %08x",							  ehdr.e_magic);
	ConLog.Write("Class: %s",							  "ELF64");
	ConLog.Write("Data: %s",							  DataToString(ehdr.e_data));
	ConLog.Write("Current Version: %d",					  ehdr.e_curver);
	ConLog.Write("OS/ABI: %s",							  OS_ABIToString(ehdr.e_os_abi));
	ConLog.Write("Type: %s",							  TypeToString(ehdr.e_type));
	ConLog.Write("Machine: %s",							  MachineToString(ehdr.e_machine));
	ConLog.Write("Version: %d",							  ehdr.e_version);
	ConLog.Write("Entry point address: 0x%08x",			  ehdr.e_entry);
	ConLog.Write("Program headers offset: %d",			  ehdr.e_phoff);
	ConLog.Write("Section headers offset: %d",			  ehdr.e_shoff);
	ConLog.Write("Flags: 0x%x",							  ehdr.e_flags);
	ConLog.Write("Size of this header: %d",				  ehdr.e_ehsize);
	ConLog.Write("Size of program headers: %d",			  ehdr.e_phentsize);
	ConLog.Write("Number of program headers: %d",		  ehdr.e_phnum);
	ConLog.Write("Size of section headers: %d",			  ehdr.e_shentsize);
	ConLog.Write("Number of section headers: %d",		  ehdr.e_shnum);
	ConLog.Write("Section header string table index: %d", ehdr.e_shstrndx);

	if(ehdr.e_unknown != 0)
	{
		ConLog.Warning("Unknown section is not null! (0x08x)", ehdr.e_unknown);
	}

	ConLog.SkipLn();

	int offset = ehdr.e_phoff;

	for(uint i=0; i<ehdr.e_phnum; ++i)
	{
		f.Seek(offset);

		Elf64_Phdr phdr;
		phdr.p_type		= Read32(f);
		phdr.p_flags	= Read32(f);
		phdr.p_offset	= Read64(f);
		phdr.p_vaddr	= Read64(f);
		phdr.p_paddr	= Read64(f);
		phdr.p_filesz	= Read64(f);
		phdr.p_memsz	= Read64(f);
		phdr.p_align	= Read64(f);

		offset += sizeof(phdr);

		ConLog.Write("Type: 0x%x", phdr.p_type);
		ConLog.Write("Offset: 0x%08x", phdr.p_offset);
		ConLog.Write("Virtual address: 0x%08x", phdr.p_vaddr);
		ConLog.Write("Physical address: 0x%08x", phdr.p_paddr);
		ConLog.Write("File size: 0x%08x", phdr.p_filesz);
		ConLog.Write("Memory size: 0x%08x", phdr.p_memsz);
		ConLog.Write("Flags: 0x%x", phdr.p_flags);
		ConLog.Write("Algin: 0x%x", phdr.p_align);

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

			f.Seek(phdr.p_offset);
			f.Read(&Memory.MainRam[phdr.p_paddr], phdr.p_filesz);
		}

		ConLog.SkipLn();
	}
}

void ElfLoader::LoadElf() //TODO
{
	const wxChar* elf_name = wxFileName(m_elf_fpatch).GetFullName().c_str();

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

	f.Seek(sizeof(u8) * 4);
	u8 _class;
	f.Read(&_class, sizeof(u8));
	f.Seek(0);

	if(_class == 1)
	{
		Elf32_Ehdr ehdr;

		ehdr.e_magic	 = Read32(f);
		ehdr.e_class	 = Read8(f);
		ehdr.e_data		 = Read8(f);
		ehdr.e_curver	 = Read8(f);
		ehdr.e_os_abi	 = Read8(f);
		ehdr.e_abi_ver	 = Read64(f);
		ehdr.e_type		 = Read16(f);
		ehdr.e_machine	 = Read16(f);
		ehdr.e_version	 = Read32(f);
		ehdr.e_entry	 = Read32(f);
		ehdr.e_phoff	 = Read32(f);
		ehdr.e_shoff	 = Read32(f);
		ehdr.e_flags	 = Read32(f);
		ehdr.e_ehsize	 = Read16(f);
		ehdr.e_phentsize = Read16(f);
		ehdr.e_phnum	 = Read16(f);
		ehdr.e_shentsize = Read16(f);
		ehdr.e_shnum	 = Read16(f);
		ehdr.e_shstrndx  = Read16(f);

		if(ehdr.e_magic != 0x7F454C46)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			f.Close();
			return;
		}

		LoadElf32(ehdr, f);
	}
	else if(_class == 2)
	{
		Elf64_Ehdr ehdr;

		ehdr.e_magic	 = Read32(f);
		ehdr.e_class	 = Read8(f);
		ehdr.e_data		 = Read8(f);
		ehdr.e_curver	 = Read8(f);
		ehdr.e_os_abi	 = Read8(f);
		ehdr.e_abi_ver	 = Read64(f);
		ehdr.e_type		 = Read16(f);
		ehdr.e_machine	 = Read16(f);
		ehdr.e_version	 = Read32(f);
		ehdr.e_unknown	 = Read32(f);
		ehdr.e_entry	 = Read32(f);
		ehdr.e_phoff	 = Read64(f);
		ehdr.e_shoff	 = Read64(f);
		ehdr.e_flags	 = Read32(f);
		ehdr.e_ehsize	 = Read16(f);
		ehdr.e_phentsize = Read16(f);
		ehdr.e_phnum	 = Read16(f);
		ehdr.e_shentsize = Read16(f);
		ehdr.e_shnum	 = Read16(f);
		ehdr.e_shstrndx  = Read16(f);

		if(ehdr.e_magic != 0x7F454C46)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			f.Close();
			return;
		}

		LoadElf64(ehdr, f);
	}
	else
	{
		f.Seek(sizeof(Elf64_Ehdr) + 4);
		f.Read(&Memory.MainRam[0], elf_size);

		ConLog.Error("Unknown elf class! (%d)", _class);
		f.Close();
		return;
	}

	f.Close();

	LoadPsf();
}

void ElfLoader::LoadSelf()
{
	wxFile f(m_elf_fpatch);
	const wxChar* self_name = wxFileName(m_elf_fpatch).GetFullName().c_str();
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

	SelfHeader hdr;
	hdr.se_magic		= Read32(f);
	hdr.se_hver			= Read32(f);
	hdr.se_flags		= Read16(f);
	hdr.se_type			= Read16(f);
	hdr.se_meta			= Read32(f);
	hdr.se_hsize		= Read64(f);
	hdr.se_esize		= Read64(f);
	hdr.se_unknown		= Read64(f);
	hdr.se_appinfo		= Read64(f);
	hdr.se_elf			= Read64(f);
	hdr.se_phdr			= Read64(f);
	hdr.se_shdr			= Read64(f);
	hdr.se_phdroff		= Read64(f);
	hdr.se_scever		= Read64(f);
	hdr.se_digest		= Read64(f);
	hdr.se_digestsize	= Read64(f);

	f.Close();
}

ElfLoader elf_loader;