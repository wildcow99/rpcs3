#pragma once

//TODO
struct ElfHeader
{
	u32	e_magic[16];
};

void ElfLoader()
{
	wxFile bin(m_isoname);
	u8* data;
	const uint size = wxULongLong(bin.Length()).GetLo();
	safe_realloc(data, size);
	bin.Read(data, size);
	bin.Close();
}