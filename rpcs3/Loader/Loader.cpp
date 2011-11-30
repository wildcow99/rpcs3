#include "stdafx.h"
#include "Loader.h"
#include "ELF.h"
#include "SELF.h"
#include "PSF.h"

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

	return ((u16)c0 << 8) | (u16)c1;
}

u32 Read32(wxFile& f)
{
	const u16 c0 = Read16(f);
	const u16 c1 = Read16(f);

	return ((u32)c0 << 16) | (u32)c1;
}

u64 Read64(wxFile& f)
{
	const u32 c0 = Read32(f);
	const u32 c1 = Read32(f);

	return ((u64)c0 << 32) | (u64)c1;
}

const wxString Ehdr_DataToString(const u8 data)
{
	if(data > 1) return wxString::Format("%d's complement, big endian", data);
	if(data < 1) return "Data is not found";

	return wxString::Format("%d's complement, small endian", data);
}

const wxString Ehdr_TypeToString(const u16 type)
{
	switch(type)
	{
	case 0: return "NULL";
	case 2: return "EXEC (Executable file)";
	};

	return wxString::Format("Unknown (%d)", type);
}

const wxString Ehdr_OS_ABIToString(const u8 os_abi)
{
	switch(os_abi)
	{
	case 0x0 : return "UNIX System V";
	case 0x66: return "Cell OS LV-2";
	};

	return wxString::Format("Unknown (%x)", os_abi);
}

const wxString Ehdr_MachineToString(const u16 machine)
{
	switch(machine)
	{
	case MACHINE_PPC64: return "PowerPC64";
	case MACHINE_SPU:	return "SPU";
	};

	return wxString::Format("Unknown (%x)", machine);
}

const wxString Phdr_FlagsToString(const u32 flags)
{
	switch(flags)
	{
	case 0x4: return "R";
	case 0x5: return "R E";
	case 0x7: return "RWE";
	};

	return wxString::Format("Unknown (%x)", flags);
}

const wxString Phdr_TypeToString(const u32 type)
{
	switch(type)
	{
	case 0x00000001: return "LOAD";
	case 0x00000004: return "NOTE";
	case 0x00000007: return "TLS";
	case 0x60000001: return "LOOS+1";
	case 0x60000002: return "LOOS+2";
	};

	return wxString::Format("Unknown (%x)", type);
}

Loader::Loader(const wxString& path)
	: f(*new wxFile(path))
	, m_path(path)
{
}

bool Loader::Load()
{
	LoaderBase* l = new ELFLoader(f);
	if(!l->Load())
	{
		free(l);
		l = new SELFLoader(f);
		if(!l->Load())
		{
			free(l);
			l = NULL;
		}
	}

	f.Close();

	if(l)
	{
		machine = l->GetMachine();
		entry = l->GetEntry();
		free(l);

		const wxString& root = wxFileName(wxFileName(m_path).GetPath()).GetPath();
		const wxString& psf_path = root + "\\" + "PARAM.SFO";
		if(wxFileExists(psf_path))
		{
			PSFLoader psf_l(psf_path);
			if(psf_l.Load())
			{
				CurGameInfo = psf_l.m_info;
				CurGameInfo.root = root;
				psf_l.Close();
			}
		}
	}

	return l != NULL;
}