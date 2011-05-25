#pragma once

//TODO

class ElfLoader
{
	struct ElfHeader
	{
		u32	e_magic;
		u32 e_unknown[10];
	};

	wxString m_elf_fpatch;

public:
	void SetElf(wxString elf_full_patch);
	void Load();
};

extern ElfLoader elf_loader;