#pragma once
#include "rpcs3.h"

//TODO
struct ElfHeader
{
	u32	e_magic;
};

class ElfLoader
{
	wxString m_elf_fpatch;

public:
	void SetElf(wxString elf_full_patch)
	{
		m_elf_fpatch = elf_full_patch;
	}

	void Load()
	{
		wxFile elf(m_elf_fpatch);

		if(!elf.IsOpened())
		{
			ConLog.Error("Error open %s!", wxFileName(m_elf_fpatch).GetFullName().c_str());
			return;
		}

		const u64 length = elf.Length();

		if(length == 0)
		{
			ConLog.Error("%s have null length!", wxFileName(m_elf_fpatch).GetFullName().c_str());
			return;
		}

		const uint size = wxULongLong().GetLo();
		const uint csize = size * sizeof(u8);

		u8* data = (u8*)malloc(csize);

		elf.Read(data, csize);
		elf.Close();

		ElfHeader& header = *(ElfHeader*)data;

		ConLog.Write("magic = %08X", header.e_magic);

		//TODO

		safe_delete(data);
	}
};
