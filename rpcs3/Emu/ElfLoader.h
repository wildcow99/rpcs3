#pragma once

#include "Emu/GameInfo.h"

u8  Read8 (wxFile& f);
u16 Read16(wxFile& f);
u32 Read32(wxFile& f);
u64 Read64(wxFile& f);

enum SectionHeaderType
{
	SHT_NULL = 0, 
	SHT_PROGBITS,
	SHT_SYMTAB,
	SHT_STRTAB,
	SHT_RELA,
	SHT_HASH,
	SHT_DYNAMIC,
	SHT_NOTE,
	SHT_NOBITS,
	SHT_REL,
	SHT_SHLIB,
	SHT_DYNSYM,
};

enum SectionHeaderFlag
{
	SHF_WRITE		= 0x1,
	SHF_ALLOC		= 0x2,
	SHF_EXECINSTR	= 0x4,
	SHF_MASKPROC	= 0xf0000000,
};

class ElfLoader //TODO
{
public:
	wxArrayPtrVoid m_sh_ptr;

	static wxString FixForName(wxString& name)
	{
		wxString ret = wxEmptyString;

		for(uint i=0; i<name.Length(); ++i)
		{
			switch(name[i])
			{
				case 0xFFFFFFE2: case 0xFFFFFFA2: case 0xFFFFFF84:
				continue;

				default: ret += name[i]; break;
			};
		}

		return ret;
	}

	static const wxString DataToString(const u8 data)
	{
		if(data > 1) return wxString::Format("%d's complement, big endian", data);
		if(data == 0) return "Data is not found";

		return wxString::Format("%d's complement, small endian", data);
	}

	static const wxString TypeToString(const u16 type)
	{
		switch(type)
		{
		case 0:  return "NULL";
		case 2:  return "EXEC (Executable file)";
		default: break;
		};

		return wxString::Format("Unknown (%d)", type);
	}

	static const wxString OS_ABIToString(const u8 os_abi)
	{
		switch(os_abi)
		{
		case 0x0 :  return "UNIX System V";
		case 0x66:  return "Cell OS LV-2";
		default: break;
		};

		return wxString::Format("Unknown (%x)", os_abi);
	}

	static const wxString MachineToString(const u16 machine)
	{
		switch(machine)
		{
		case 0x15:  return "PowerPC64";
		case 0x17:  return "SPU";
		default: break;
		};

		return wxString::Format("Unknown (%x)", machine);
	}

	static const wxString Phdr_FlagsToString(const u32 flags)
	{
		switch(flags)
		{
		case 0x4:  return "R";
		case 0x5:  return "R E";
		case 0x7:  return "RWE";
		default: break;
		};

		return wxString::Format("Unknown (%x)", flags);
	}

	static const wxString Phdr_TypeToString(const u32 type)
	{
		switch(type)
		{
		case 0x00000001: return "LOAD";
		case 0x00000004: return "NOTE";
		case 0x00000007: return "TLS";
		case 0x60000001: return "LOOS+1";
		case 0x60000002: return "LOOS+2";
		default: break;
		};

		return wxString::Format("Unknown (%x)", type);
	}

	struct PsfHeader
	{
		u32 ps_magic;
		u32 ps_unk_0;
		u32 ps_listoff;
		u32 ps_soffset;
	};

	struct SelfHeader
	{
		u32 se_magic;
		u32 se_hver;
		u16 se_flags;
		u16 se_type;
		u32 se_meta;
		u64 se_hsize;
		u64 se_esize;
		u64 se_unknown;
		u64 se_appinfo;
		u64 se_elf;
		u64 se_phdr;
		u64 se_shdr;
		u64 se_phdroff;
		u64 se_scever;
		u64 se_digest;
		u64 se_digestsize;

		virtual void Load(wxFile& f)
		{
			se_magic		= Read32(f);
			se_hver			= Read32(f);
			se_flags		= Read16(f);
			se_type			= Read16(f);
			se_meta			= Read32(f);
			se_hsize		= Read64(f);
			se_esize		= Read64(f);
			se_unknown		= Read64(f);
			se_appinfo		= Read64(f);
			se_elf			= Read64(f);
			se_phdr			= Read64(f);
			se_shdr			= Read64(f);
			se_phdroff		= Read64(f);
			se_scever		= Read64(f);
			se_digest		= Read64(f);
			se_digestsize	= Read64(f);
		}

		virtual void Show()
		{
			ConLog.Write("Magic: %08x",			se_magic);
			ConLog.Write("Class: %s",			"SELF");
			ConLog.Write("hver: 0x%08x",		se_hver);
			ConLog.Write("flags: 0x%04x",		se_flags);
			ConLog.Write("type: 0x%04x",		se_type);
			ConLog.Write("meta: 0x%08x",		se_meta);
			ConLog.Write("hsize: 0x%x",			se_hsize);
			ConLog.Write("esize: 0x%x",			se_esize);
			ConLog.Write("unknown: 0x%x",		se_unknown);
			ConLog.Write("appinfo: 0x%x",		se_appinfo);
			ConLog.Write("elf: 0x%x",			se_elf);
			ConLog.Write("phdr: 0x%x",			se_phdr);
			ConLog.Write("shdr: 0x%x",			se_shdr);
			ConLog.Write("phdroff: 0x%x",		se_phdroff);
			ConLog.Write("scever: 0x%x",		se_scever);
			ConLog.Write("digest: 0x%x",		se_digest);
			ConLog.Write("digestsize: 0x%x",	se_digestsize);
		}

		virtual bool IsCorrect() const
		{
			return se_magic == 0x53434500;
		}
	};

	struct Elf32_Ehdr
	{
		u32 e_magic;
		u8  e_class;
		u8  e_data;
		u8  e_curver;
		u8  e_os_abi;
		u64 e_abi_ver;
		u16 e_type;
		u16 e_machine;
		u32 e_version;
		u16 e_entry;
		u32 e_phoff;
		u32 e_shoff;
		u32 e_flags;
		u16 e_ehsize;
		u16 e_phentsize;
		u16 e_phnum;
		u16 e_shentsize;
		u16 e_shnum;
		u16 e_shstrndx;

		virtual void Show()
		{
			ConLog.Write("Magic: %08x",								e_magic);
			ConLog.Write("Class: %s",								"ELF32");
			ConLog.Write("Data: %s",								DataToString(e_data));
			ConLog.Write("Current Version: %d",						e_curver);
			ConLog.Write("OS/ABI: %s",								OS_ABIToString(e_os_abi));
			ConLog.Write("ABI version: %d",							e_abi_ver);
			ConLog.Write("Type: %s",								TypeToString(e_type));
			ConLog.Write("Machine: %s",								MachineToString(e_machine));
			ConLog.Write("Version: %d",								e_version);
			ConLog.Write("Entry point address: 0x%x",				e_entry);
			ConLog.Write("Program headers offset: 0x%08x",			e_phoff);
			ConLog.Write("Section headers offset: 0x%08x",			e_shoff);
			ConLog.Write("Flags: 0x%x",								e_flags);
			ConLog.Write("Size of this header: %d",					e_ehsize);
			ConLog.Write("Size of program headers: %d",				e_phentsize);
			ConLog.Write("Number of program headers: %d",			e_phnum);
			ConLog.Write("Size of section headers: %d",				e_shentsize);
			ConLog.Write("Number of section headers: %d",			e_shnum);
			ConLog.Write("Section header string table index: %d",	e_shstrndx);
		}

		virtual void Load(wxFile& f)
		{
			e_magic		= Read32(f);
			e_class		= Read8(f);
			e_data		= Read8(f);
			e_curver	= Read8(f);
			e_os_abi	= Read8(f);
			e_abi_ver	= Read64(f);
			e_type		= Read16(f);
			e_machine	= Read16(f);
			e_version	= Read32(f);
			e_entry		= Read32(f);
			e_phoff		= Read32(f);
			e_shoff		= Read32(f);
			e_flags		= Read32(f);
			e_ehsize	= Read16(f);
			e_phentsize = Read16(f);
			e_phnum		= Read16(f);
			e_shentsize = Read16(f);
			e_shnum		= Read16(f);
			e_shstrndx  = Read16(f);
		}
	};

	struct Elf64_Ehdr
	{
		u32 e_magic;
		u8  e_class;
		u8  e_data;
		u8  e_curver;
		u8  e_os_abi;
		u64 e_abi_ver;
		u16 e_type;
		u16 e_machine;
		u32 e_version;
		u64 e_entry;
		u64 e_phoff;
		u64 e_shoff;
		u32 e_flags;
		u16 e_ehsize;
		u16 e_phentsize;
		u16 e_phnum;
		u16 e_shentsize;
		u16 e_shnum;
		u16 e_shstrndx;

		virtual void Load(wxFile& f)
		{
			e_magic		= Read32(f);
			e_class		= Read8(f);
			e_data		= Read8(f);
			e_curver	= Read8(f);
			e_os_abi	= Read8(f);
			e_abi_ver	= Read64(f);
			e_type		= Read16(f);
			e_machine	= Read16(f);
			e_version	= Read32(f);
			e_entry		= Read64(f);
			e_phoff		= Read64(f);
			e_shoff		= Read64(f);
			e_flags		= Read32(f);
			e_ehsize	= Read16(f);
			e_phentsize = Read16(f);
			e_phnum		= Read16(f);
			e_shentsize = Read16(f);
			e_shnum		= Read16(f);
			e_shstrndx  = Read16(f);
		}

		virtual void Show()
		{
			ConLog.Write("Magic: %08x",								e_magic);
			ConLog.Write("Class: %s",								"ELF64");
			ConLog.Write("Data: %s",								DataToString(e_data));
			ConLog.Write("Current Version: %d",						e_curver);
			ConLog.Write("OS/ABI: %s",								OS_ABIToString(e_os_abi));
			ConLog.Write("ABI version: %d",							e_abi_ver);
			ConLog.Write("Type: %s",								TypeToString(e_type));
			ConLog.Write("Machine: %s",								MachineToString(e_machine));
			ConLog.Write("Version: %d",								e_version);
			ConLog.Write("Entry point address: 0x%08x",				e_entry);
			ConLog.Write("Program headers offset: 0x%08x",			e_phoff);
			ConLog.Write("Section headers offset: 0x%08x",			e_shoff);
			ConLog.Write("Flags: 0x%x",								e_flags);
			ConLog.Write("Size of this header: %d",					e_ehsize);
			ConLog.Write("Size of program headers: %d",				e_phentsize);
			ConLog.Write("Number of program headers: %d",			e_phnum);
			ConLog.Write("Size of section headers: %d",				e_shentsize);
			ConLog.Write("Number of section headers: %d",			e_shnum);
			ConLog.Write("Section header string table index: %d",	e_shstrndx);
		}
	};

	struct Elf64_Shdr
	{
		u32 sh_name; 
		u32 sh_type;
		u64 sh_flags;
 		u64 sh_addr;
 		u64 sh_offset;
		u64 sh_size;
 		u32 sh_link;
 		u32 sh_info;
		u64 sh_addralign;
		u64 sh_entsize;

		virtual void Load(wxFile& f)
		{
			sh_name			= Read32(f);
			sh_type			= Read32(f);
			sh_flags		= Read64(f);
			sh_addr			= Read64(f);
			sh_offset		= Read64(f);
			sh_size			= Read64(f);
			sh_link			= Read32(f);
			sh_info			= Read32(f);
			sh_addralign	= Read64(f);
			sh_entsize		= Read64(f);
		}

		virtual void Show()
		{
			ConLog.Write("Name offset: %x", sh_name);
			ConLog.Write("Type: %d", sh_type);
			ConLog.Write("Addr: %x", sh_addr);
			ConLog.Write("Offset: %x", sh_offset);
			ConLog.Write("Size: %x", sh_size);
			ConLog.Write("EntSize: %d", sh_entsize);
			ConLog.Write("Flags: %x", sh_flags);
			ConLog.Write("Link: %x", sh_link);
			ConLog.Write("Info: %d", sh_info);
			ConLog.Write("Address align: %x", sh_addralign);
		}
	};

	struct Elf32_Phdr
	{
		u32	p_type;
		u32	p_offset;
		u32	p_vaddr;
		u32	p_paddr;
		u32	p_filesz;
		u32	p_memsz;
		u32	p_flags;
		u32	p_align;

		virtual void Load(wxFile& f)
		{
			p_type		= Read32(f);
			p_offset	= Read32(f);
			p_vaddr		= Read32(f);
			p_paddr		= Read32(f);
			p_filesz	= Read32(f);
			p_memsz		= Read32(f);
			p_flags		= Read32(f);
			p_align		= Read32(f);
		}

		virtual void Show()
		{
			ConLog.Write("Type: %s", Phdr_TypeToString(p_type));
			ConLog.Write("Offset: 0x%08x", p_offset);
			ConLog.Write("Virtual address: 0x%08x", p_vaddr);
			ConLog.Write("Physical address: 0x%08x", p_paddr);
			ConLog.Write("File size: 0x%08x", p_filesz);
			ConLog.Write("Memory size: 0x%08x", p_memsz);
			ConLog.Write("Flags: %s", Phdr_FlagsToString(p_flags));
			ConLog.Write("Algin: 0x%x", p_align);
		}
	};

	struct Elf64_Phdr
	{
		u32	p_type;
		u32	p_flags;
		u64	p_offset;
		u64	p_vaddr;
		u64	p_paddr;
		u64	p_filesz;
		u64	p_memsz;
		u64	p_align;

		virtual void Load(wxFile& f)
		{
			p_type		= Read32(f);
			p_flags		= Read32(f);
			p_offset	= Read64(f);
			p_vaddr		= Read64(f);
			p_paddr		= Read64(f);
			p_filesz	= Read64(f);
			p_memsz		= Read64(f);
			p_align		= Read64(f);
		}

		virtual void Show()
		{
			ConLog.Write("Type: %s", Phdr_TypeToString(p_type));
			ConLog.Write("Offset: 0x%08x", p_offset);
			ConLog.Write("Virtual address: 0x%08x", p_vaddr);
			ConLog.Write("Physical address: 0x%08x", p_paddr);
			ConLog.Write("File size: 0x%08x", p_filesz);
			ConLog.Write("Memory size: 0x%08x", p_memsz);
			ConLog.Write("Flags: %s", Phdr_FlagsToString(p_flags));
			ConLog.Write("Algin: 0x%x", p_align);
		}
	};

private:
	wxString m_elf_fpatch;

public:
	uint elf_size;

	ElfLoader()
	{
		elf_size = 0;
	}

	virtual void SetElf(wxString elf_full_patch);
	virtual void LoadElf(bool IsDump = false);
	virtual void LoadSelf(bool IsDump = false);

private:
	virtual void LoadElf32(wxFile& f);
	virtual void LoadElf64(wxFile& f);

	virtual void LoadPhdr32(wxFile& f, Elf32_Ehdr& ehdr, const uint offset = 0);
	virtual void LoadPhdr64(wxFile& f, Elf64_Ehdr& ehdr, const uint offset = 0);

	virtual void LoadShdr64(wxFile& f, Elf64_Ehdr& ehdr, const uint offset = 0);

	virtual void LoadPsf();
};

extern ElfLoader Loader;