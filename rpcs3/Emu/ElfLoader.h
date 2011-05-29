#pragma once

//TODO

class ElfLoader
{
	struct SelfHeader
	{
		u32 magic;
		u32 version;
		u16 attribute;
		u16 category;
		u32 metadataInfoOffset;
		u64 fileOffset;
		u64 fileSize;
		u64 id;
		u64 programInfoOffset;
		u64 elfHeaderOffset;
		u64 elfProgramHeadersOffset;
		u64 elfSectionHeadersOffset;
		u64 sInfoOffset;
		u64 versionInfoOffset;
		u64 controlInfoOffset;
		u64 controlInfoSize;
		u64 app;
	};


/* Type for a 16-bit quantity.  *//*
typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  *//*
typedef uint32_t Elf32_Word;
typedef	int32_t  Elf32_Sword;
typedef uint32_t Elf64_Word;
typedef	int32_t  Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  *//*
typedef uint64_t Elf32_Xword;
typedef	int64_t  Elf32_Sxword;
typedef uint64_t Elf64_Xword;
typedef	int64_t  Elf64_Sxword;

/* Type of addresses.  *//*
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;

/* Type of file offsets.  *//*
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;

/* Type for section indices, which are 16-bit quantities.  *//*
typedef uint16_t Elf32_Section;
typedef uint16_t Elf64_Section;

/* Type of symbol indices.  *//*
typedef uint32_t Elf32_Symndx;
typedef uint64_t Elf64_Symndx;

/* Type for version symbol information.  *//*
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Versym;
*/

	struct Elf32_Ehdr
	{
		u8 e_magic[16];
		//u64 e_magic;		/* Magic number and other info */
		u16	e_type;			/* Object file type */
		u16	e_machine;		/* Architecture */
		u32	e_version;		/* Object file version */
		u32	e_entry;		/* Entry point virtual address */
		u32	e_phoff;		/* Program header table file offset */
		u32	e_shoff;		/* Section header table file offset */
		u32	e_flags;		/* Processor-specific flags */
		u16	e_ehsize;		/* ELF header size in bytes */
		u16	e_phentsize;	/* Program header table entry size */
		u16	e_phnum;		/* Program header table entry count */
		u16	e_shentsize;	/* Section header table entry size */
		u16	e_shnum;		/* Section header table entry count */
		u16	e_shstrndx;		/* Section header string table index */
	};

	struct Elf64_Ehdr
	{
		u8 e_magic[16];
		//u64 e_magic;		/* Magic number and other info */
		u16 e_type;         /* Object file type */
		u16 e_machine;      /* Architecture */
		u32 e_version;      /* Object file version */
		u64 e_entry;        /* Entry point virtual address */
		u64	e_phoff;        /* Program header table file offset */
		u64 e_shoff;        /* Section header table file offset */
		u32 e_flags;        /* Processor-specific flags */
		u16 e_ehsize;       /* ELF header size in bytes */
		u16 e_phentsize;    /* Program header table entry size */
		u16 e_phnum;        /* Program header table entry count */
		u16 e_shentsize;    /* Section header table entry size */
		u16 e_shnum;        /* Section header table entry count */
		u16 e_shstrndx;     /* Section header string table index */
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

	struct Elf64_Phdr
	{
	  u32      p_type;
	  u32      p_flags;
	  u64	   p_offset;
	  u64      p_vaddr;
	  u64      p_paddr;
	  u64      p_filesz;
	  u64      p_memsz;
	  u64      p_align;
	};

	wxString m_elf_fpatch;

public:
	void SetElf(wxString elf_full_patch);
	void LoadElf();
	void LoadSelf();

	uint LoadElf32(Elf32_Ehdr& header, wxFile& elf);
	uint LoadElf64(Elf64_Ehdr& header, wxFile& elf);
};

extern ElfLoader elf_loader;