#include "stdafx.h"
#include "Loader.h"
#include "ELF.h"

ELFLoader::ELFLoader(wxFile& f)
	: elf_f(f)
	, LoaderBase()
{
}

ELFLoader::ELFLoader(const wxString& path)
	: elf_f(*new wxFile(path))
	, LoaderBase()
{
}

bool ELFLoader::Load()
{
	if(!elf_f.IsOpened()) return false;

	elf_f.Seek(0);
	ehdr.Load(elf_f);
	if(!ehdr.CheckMagic()) return false;

	LoaderBase* l = NULL;
	switch(ehdr.GetClass())
	{
	case CLASS_ELF32: l = new ELF32Loader(elf_f); break;
	case CLASS_ELF64: l = new ELF64Loader(elf_f); break;
	}

	if(l)
	{
		if(!l->Load()) return false;
		entry = l->GetEntry();
		machine = l->GetMachine();
		return true;
	}

	ConLog.Error("Unknown elf type! [0x%x]", ehdr.e_class);
	return false;
}

bool ELFLoader::Close()
{
	return elf_f.Close();
}
