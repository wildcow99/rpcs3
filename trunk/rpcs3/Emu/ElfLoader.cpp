#include "stdafx.h"
#include "ElfLoader.h"
#include "rpcs3.h"
#include "Emu/Memory/Memory.h"

#include "Gui/MemoryViewer.h"

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

const u8 Read_u8(wxFile& file)
{
	u8 ret;
	file.Read(&ret, sizeof(u8));

	return ret;
}

const u16 Read_u16(wxFile& file)
{
	return ((u16)Read_u8(file)) | ((u16)Read_u8(file) << 8);
}

const s32 Read_s32(wxFile& file)
{
	return (((u32)Read_u8(file)) | ((u32)Read_u8(file) << 8)) |
		(((u32)Read_u8(file) << 16) | ((u32)Read_u8(file) << 24));
}

const u32 Read_u32(wxFile& file)
{
	return ((((u32)Read_u8(file)) | ((u32)Read_u8(file) << 8)) |
		(((u32)Read_u8(file) << 16) | ((u32)Read_u8(file) << 24))) & 0xFFFFFFFFL;
}

void ElfLoader::SetElf(wxString elf_full_patch)
{
	m_elf_fpatch = elf_full_patch;
}

void ElfLoader::LoadPsf()
{
	const wxString dir = wxFileName(wxFileName(m_elf_fpatch).GetPath()).GetPath();
	const wxString fpatch = dir + "\\" + "PARAM.SFO";
	wxFile psf(fpatch);

	if(!psf.IsOpened())
	{
		ConLog.Error("Error loading %s!", fpatch.c_str());
		return;
	}
	else
	{
		ConLog.Write("Loading %s!", fpatch.c_str());
	}

	u32 magic;
	psf.Seek(0);
	const u32 length = psf.Length();
	psf.Read(&magic, sizeof(u32));

	if(magic != 0x46535000)
	{
		ConLog.Error("%s is not PSF!", fpatch.c_str());
		return;
	}

	psf.Seek((length-sizeof(PsfHeader)) + 4);

	PsfHeader header;
	psf.Read(&header, sizeof(PsfHeader));
	psf.Close();

	//ConLog.Write("magic: %s", from32toString(header.magic).c_str());

	ConLog.Write("name: %s", from32toString_Name(header.name, 31).c_str());
	ConLog.Write("reg: %s", from64toString(header.reg, 2).c_str());
	const wxString comm_id = from32toString(header.comm_id, 4).c_str();
	if(!comm_id.IsEmpty()) ConLog.Write("comm id: %s", comm_id.c_str());
	//ConLog.Write("app ver: %s", from32toString(header.app_ver, 3).c_str());
	ConLog.Write("ver: %s", from32toString(header.ver, 3).c_str());
}

uint ElfLoader::LoadElf32(Elf32_Ehdr& header, wxFile& elf)
{
	uint ret = 0;
	ConLog.Warning("Elf 32!!!");
	//ConLog.Write("magic = %08X", header.e_magic & 0xFFFFFFFFL);
	ConLog.Write("machine = %08X", header.e_machine & 0xFFFF);
	ConLog.Write("version = %08X", header.e_version & 0xFFFFFFFFL);
	ConLog.Write("entry = %08X", header.e_entry & 0xFFFFFFFFL);
	ConLog.Write("version = %08X", header.e_phoff & 0xFFFFFFFFL);
	ConLog.Write("shoff = %08X", header.e_shoff & 0xFFFFFFFFL);
	ConLog.Write("flags = %08X", header.e_flags & 0xFFFFFFFFL);
	ConLog.Write("ehsize = %08X", header.e_ehsize & 0xFFFF);
	ConLog.Write("phnum = %08X", header.e_phnum & 0xFFFF);
	ConLog.Write("phoff = %08X", header.e_phoff & 0xFFFF);

	return ret;
}

uint ElfLoader::LoadElf64(Elf64_Ehdr& header, wxFile& elf)
{
	uint ret = 0;

	//ConLog.Write("magic = %08X", header.e_magic & 0xFFFFFFFFL);
	if( (header.e_machine & 0xFFFF) != 8)
	{
		ConLog.Error("machine(%d) != 8!", header.e_machine);
		//return ret;
	}

	ConLog.Write("machine = %08X", header.e_machine);
	ConLog.Write("version = %08X", header.e_version);
	ConLog.Write("entry = %08X", header.e_entry);
	ConLog.Write("phoff = %08X", header.e_phoff);
	ConLog.Write("shoff = %08X", header.e_shoff);
	ConLog.Write("flags = %08X", header.e_flags);
	ConLog.Write("ehsize = %08X", header.e_ehsize);
	ConLog.Write("phnum = %08X", header.e_phnum);
	//ConLog.Write("class = %16X", header.e_class);

	return ret;

	const uint length = elf.Length();

	if( header.e_phnum > 0 /*&& header.e_phoff > 0 */)
	{
		Elf64_Phdr* proghead = new Elf64_Phdr[header.e_phnum];
		elf.Seek(header.e_phoff);
		elf.Read(proghead, sizeof(Elf64_Phdr) * header.e_phnum);

		for( u16 i = 0; i < header.e_phnum; ++i )
		{
			if(proghead[i].p_type == 0x1)
			{
				//if (proghead[i].p_offset < length)
				//{
					ConLog.Write("Loading Elf64... (%u)", proghead[i].p_type);

					uint size;
					
					if ((proghead[i].p_filesz + proghead[i].p_offset) > length)
					{
						size = length - proghead[i].p_offset;
					}
					else
						size = proghead[i].p_filesz;

					if (proghead[i].p_vaddr != proghead[i].p_paddr)
						ConLog.Warning( L"ElfProgram different load addrs: paddr=0x%8.8x, vaddr=0x%8.8x",
							proghead[i].p_paddr, proghead[i].p_vaddr);

					ConLog.SkipLn();
					ConLog.Write("Founded opcode!");
					ConLog.Write("opcode: size - %d", size);
					ConLog.Write("opcode: addr - %x", proghead[i].p_vaddr & 0x1ffffff);
					ConLog.Write("opcode: offs - %x", proghead[i].p_offset);
					ConLog.SkipLn();

					elf.Seek(proghead[i].p_offset);
					elf.Read(&Memory.MainRam[proghead[i].p_vaddr & 0x1ffffff], size);

					ret = proghead[i].p_vaddr & 0x1ffffff;
				
					ConLog.Write("Elf64 loaded!");

					//return ret;
				}
				else
				{
					ConLog.Warning( L"Unknown Elf64 type! (%u)", proghead[i].p_type );
				}
			//}
		}
	}

	return ret;
}

static const uint data_pc = 0;

void ElfLoader::LoadElf() //TODO
{
	wxFile elf(m_elf_fpatch);
	const wxChar* elf_name = wxFileName(m_elf_fpatch).GetFullName().c_str();
	if(!elf.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return;
	}

	const u64 length = elf.Length();
	elf_size = length;

	if(length == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		return;
	}

	const uint size = wxULongLong(length).GetLo();
	const uint csize = size * sizeof(u8);

	elf.Seek(0);
	u8	ident[16] = {0};
	elf.Read(ident, sizeof(ident));
	elf.Seek(0);

	uint mem_pc = 0;
	if(ident[4] == 1)
	{
		Elf32_Ehdr header;
	
		elf.Read(&header, sizeof(Elf32_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*/

		mem_pc = LoadElf32(header, elf);
	}
	else if(ident[4] == 2)
	{
	
		Elf64_Ehdr header;

		//Word = 32
		//Half = 16
		//Byte = 8
		/*
		header.e_magic = Read_u32(elf);
		header.e_class = Read_u8(elf);
		header.e_data = Read_u8(elf);
		header.e_idver = Read_u8(elf);
		elf.Read(header.e_pad, sizeof(header.e_pad));
		header.e_type = Read_u16(elf);
		header.e_machine = Read_u16(elf);
		header.e_version = Read_u32(elf);
		header.e_entry = Read_u32(elf);
		header.e_phoff = Read_u32(elf);
		header.e_shoff = Read_u32(elf);
		header.e_flags = Read_u32(elf);
		header.e_ehsize = Read_u32(elf);
		header.e_phentsize = Read_u16(elf);
		header.e_phnum = Read_u16(elf);
		header.e_shentsize = Read_u16(elf);
		header.e_shnum = Read_u16(elf);
		header.e_shstrndx = Read_u16(elf);
		*/

	
		elf.Read(&header, sizeof(Elf64_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*/

		mem_pc = LoadElf64(header, elf);
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", ident[4]);
		return;
	}

	if(mem_pc == 0)
	{
		//const uint data_pc = 0x200;
		const uint mem_size = length - data_pc;
		elf_size = mem_size;

		elf.Seek(data_pc);
		elf.Read(&Memory.MainRam[mem_pc], mem_size);
	}

	elf.Close();

	LoadPsf();

	MemoryViewerPanel* memory_viewer = new MemoryViewerPanel(NULL);

	memory_viewer->SetPC(mem_pc);
	memory_viewer->Show();
	memory_viewer->ShowPC();
}

void ElfLoader::LoadSelf()
{
	wxFile elf(m_elf_fpatch);
	const wxChar* elf_name = wxFileName(m_elf_fpatch).GetFullName().c_str();
	if(!elf.IsOpened())
	{
		ConLog.Error("Error open %s!", elf_name);
		return;
	}

	const u64 length = elf_size = elf.Length();

	if(length == 0)
	{
		ConLog.Error("%s is null!", elf_name);
		return;
	}

	SelfHeader header;
	elf.Seek(0);
	elf.Read(&header, sizeof(SelfHeader));

	ConLog.Write("magic = %08X", header.magic);
	ConLog.Write("attribute = %04X", header.attribute);
	ConLog.Write("category = %04X", header.category);
	ConLog.Write("metadataInfoOffset = %08X", header.metadataInfoOffset);
	ConLog.Write("fileOffset = %16X", header.fileOffset);
	ConLog.Write("fileSize = %16X", header.fileSize);
	ConLog.Write("id = %16X", header.id);
	ConLog.Write("programInfoOffset = %16X", header.programInfoOffset);
	ConLog.Write("elfHeaderOffset = %08X", header.elfHeaderOffset);
	ConLog.Write("elfProgramHeadersOffset = %16X", header.elfProgramHeadersOffset);
	ConLog.Write("elfSectionHeadersOffset = %16X", header.elfSectionHeadersOffset);
	ConLog.Write("sInfoOffset = %16X", header.sInfoOffset);
	ConLog.Write("versionInfoOffset = %16X", header.versionInfoOffset);
	ConLog.Write("controlInfoOffset = %16X", header.controlInfoOffset);
	ConLog.Write("controlInfoSize = %16X", header.controlInfoSize);
	ConLog.Write("app = %16X", header.app);

	Elf64_Ehdr elfHeader;
	const uint size = wxULongLong(elf.Length()).GetLo();
	const uint c_size = size * sizeof(u8);

	if(header.elfHeaderOffset > c_size)
	{
		elf.Seek(header.elfHeaderOffset);
		elf.Read(&elfHeader, sizeof( Elf64_Ehdr ));
	}
	else
	{
		ConLog.Error("Elf header too large! (%d - %d)", header.elfHeaderOffset, c_size);

		elfHeader = *new Elf64_Ehdr();
	}

	uint mem_pc = LoadElf64(elfHeader, elf);

	/*
	elf.Seek(0);
	u8	ident[16] = {0};
	elf.Read(ident, sizeof(ident));
	elf.Seek(0);

	uint mem_pc = 0;
	if(ident[4] == 1)
	{
		Elf32_Ehdr header;
	
		elf.Read(&header, sizeof(Elf32_Ehdr));

		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}
		*//*

		mem_pc = LoadElf32(header, elf);
	}
	else if(ident[4] == 2)
	{

		Elf64_Ehdr header;
	
		elf.Read(&header, sizeof(Elf64_Ehdr));
		/*
		if((header.e_magic & 0xFFFFFFFFL) != 0x464C457F)
		{
			ConLog.Error("%s is not correct elf!", elf_name);
			return;
		}*//*

		mem_pc = LoadElf64(header, elf);
	}
	else
	{
		ConLog.Error("Unknown elf class! (%d)", ident[4]);
		return;
	}
	*/

	/*
	Elf64_Shdr elfSectionHeaders[100];

	elf.Seek(header.elfSectionHeadersOffset);
	elf.Read(elfSectionHeaders, sizeof( Elf64_Shdr ) * elfHeader.e_shnum);

	Elf64_Phdr elfProgramHeaders[100];

	elf.Seek(header.elfProgramHeadersOffset);
	elf.Read(elfProgramHeaders, sizeof( Elf64_Phdr ) * elfHeader.e_phnum);
	*/

	if(mem_pc == 0)
	{
		//const uint data_pc = 0;//0x200;
		const uint mem_size = elf_size = length - data_pc;
		elf.Seek(data_pc);
		elf.Read(&Memory.MainRam[mem_pc], mem_size);
	}

	elf.Close();

	LoadPsf();

	MemoryViewerPanel* memory_viewer = new MemoryViewerPanel(NULL);

	memory_viewer->SetPC(mem_pc);
	memory_viewer->Show();
	memory_viewer->ShowPC();

	elf.Close();
}

ElfLoader elf_loader;