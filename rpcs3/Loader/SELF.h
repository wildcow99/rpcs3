#pragma once
#include "Loader.h"

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

	void Load(wxFile& f)
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

	void Show()
	{
		ConLog.Write("Magic: %08x",			se_magic);
		ConLog.Write("Class: %s",			"SELF");
		ConLog.Write("hver: 0x%08x",		se_hver);
		ConLog.Write("flags: 0x%04x",		se_flags);
		ConLog.Write("type: 0x%04x",		se_type);
		ConLog.Write("meta: 0x%08x",		se_meta);
		ConLog.Write("hsize: 0x%llx",		se_hsize);
		ConLog.Write("esize: 0x%llx",		se_esize);
		ConLog.Write("unknown: 0x%llx",		se_unknown);
		ConLog.Write("appinfo: 0x%llx",		se_appinfo);
		ConLog.Write("elf: 0x%llx",			se_elf);
		ConLog.Write("phdr: 0x%llx",		se_phdr);
		ConLog.Write("shdr: 0x%llx",		se_shdr);
		ConLog.Write("phdroff: 0x%llx",		se_phdroff);
		ConLog.Write("scever: 0x%llx",		se_scever);
		ConLog.Write("digest: 0x%llx",		se_digest);
		ConLog.Write("digestsize: 0x%llx",	se_digestsize);
	}

	bool CheckMagic() const { return se_magic == 0x53434500; }
};

class SELFLoader : public LoaderBase
{
	wxFile& self_f;

	SelfHeader hdr;

public:
	SELFLoader(wxFile& f);
	SELFLoader(const wxString& path);
	~SELFLoader() {Close();}

	virtual bool LoadInfo();
	virtual bool LoadData();
};