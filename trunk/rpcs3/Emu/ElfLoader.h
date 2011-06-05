#pragma once

#include "Emu/GameInfo.h"

class ElfLoader //TODO
{
	/*
	struct PsfHeader
	{
		u32		magic; //46535000 (PSF)
		u64		unknown_0[28];
		u32		list[34];
		u32		app_ver[3];
		u32		attribute;
		u32		license[129];
		u32		comm_id[4];
		u32		unknown_1;
		u64		plevel;
		u32		unknown_2[3];
		u32		name[31];
		u64		reg[2];
		u32		ver[3];
	};
	*/

	struct PsfHeader
	{
		u32		comm_id[4];
		u32		unknown_1;
		u64		plevel;
		u32		unknown_2[3];
		u32		name[31];
		u64		reg[2];
		u32		ver[3];
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
		u32 e_unknown;
		u32 e_entry;
		u64 e_phoff;
		u64 e_shoff;
		u32 e_flags;
		u16 e_ehsize;
		u16 e_phentsize;
		u16 e_phnum;
		u16 e_shentsize;
		u16 e_shnum;
		u16 e_shstrndx;
	};

	struct Elf64_Shdr
	{
		u32     sh_name; 
		u32     sh_type;
		u64     sh_flags;
		u64     sh_addr;
		u64     sh_offset;
		u64     sh_size;
		u32     sh_link;
		u32     sh_info;
		u64     sh_addralign;
		u64     sh_entsize;
	};

	struct Elf32_Phdr
	{
		u32		p_type;
		u32		p_offset;
		u32		p_vaddr;
		u32		p_paddr;
		u32		p_filesz;
		u32		p_memsz;
		u32		p_flags;
		u32		p_align;
	};

	struct Elf64_Phdr
	{
		u32		p_type;
		u32		p_flags;
		u64		p_offset;
		u64		p_vaddr;
		u64		p_paddr;
		u64		p_filesz;
		u64		p_memsz;
		u64		p_align;
	};

	wxString m_elf_fpatch;

public:
	uint elf_size;

	ElfLoader()
	{
		elf_size = 0;
	}

	void SetElf(wxString elf_full_patch);
	void LoadElf();
	void LoadSelf();

private:
	void LoadElf32(Elf32_Ehdr& ehdr, wxFile& f);
	void LoadElf64(Elf64_Ehdr& ehdr, wxFile& f);

	void LoadPsf();
};

extern ElfLoader elf_loader;