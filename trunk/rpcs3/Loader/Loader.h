#pragma once

enum Elf_Machine
{
	MACHINE_Unknown,
	MACHINE_PPC64 = 0x15,
	MACHINE_SPU = 0x17,
};

enum ShdrType
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

enum ShdrFlag
{
	SHF_WRITE		= 0x1,
	SHF_ALLOC		= 0x2,
	SHF_EXECINSTR	= 0x4,
	SHF_MASKPROC	= 0xf0000000,
};

u8  Read8 (wxFile& f);
u16 Read16(wxFile& f);
u32 Read32(wxFile& f);
u64 Read64(wxFile& f);

const wxString Ehdr_DataToString(const u8 data);
const wxString Ehdr_TypeToString(const u16 type);
const wxString Ehdr_OS_ABIToString(const u8 os_abi);
const wxString Ehdr_MachineToString(const u16 machine);
const wxString Phdr_FlagsToString(u32 flags);
const wxString Phdr_TypeToString(const u32 type);

class LoaderBase
{
protected:
	u32 entry;
	Elf_Machine machine;

	LoaderBase()
		: machine(MACHINE_Unknown)
		, entry(0)
	{
	}

public:
	virtual bool LoadInfo(){return false;};
	virtual bool LoadData(){return false;};
	virtual bool Close(){return false;};
	Elf_Machine GetMachine() { return machine; }
	u32 GetEntry() { return entry; }
};

class Loader : public LoaderBase
{
	wxFile& f;
	wxString m_path;

public:
	Loader(const wxString& path);
	~Loader();

	bool Load();

private:
	LoaderBase* SearchLoader();
};