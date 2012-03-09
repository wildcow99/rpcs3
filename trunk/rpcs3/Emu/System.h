#pragma once

#include "Gui/MemoryViewer.h"
#include "Emu/Cell/PPCThreadManager.h"
#include "Utilites/Array.h"
#include "Emu/Io/Pad.h"
#include "Emu/DbgConsole.h"
#include "Emu/GS/GSManager.h"

enum Status
{
	Runned,
	Paused,
	Stoped,
};

class Emulator
{
	enum Mode
	{
		DisAsm,
		InterpreterDisAsm,
		Interpreter,
	};

	Status m_status;
	uint m_mode;

	MemoryViewerPanel* m_memory_viewer;
	//ArrayF<CPUThread> m_cpu_threads;

	PPCThreadManager m_thread_manager;
	PadManager m_pad_manager;
	IdManager m_id_manager;
	DbgConsole* m_dbg_console;
	GSManager m_gs_manager;

	u64 tls_addr;
	u64 tls_filesz;
	u64 tls_memsz;
	u32 malloc_page_size;

public:
	wxString m_path;
	bool IsSelf;

	Emulator();

	void Init();
	virtual void SetSelf(const wxString& path);
	virtual void SetElf(const wxString& path);

	PPCThreadManager&	GetCPU()		{ return m_thread_manager; }
	PadManager&			GetPadManager()	{ return m_pad_manager; }
	IdManager&			GetIdManager()	{ return m_id_manager; }
	DbgConsole&			GetDbgCon()		{ return *m_dbg_console; }
	GSManager&			GetGSManager()	{ return m_gs_manager; }

	void SetTLSData(const u64 addr, const u64 filesz, const u64 memsz)
	{
		tls_addr = addr;
		tls_filesz = filesz;
		tls_memsz = memsz;
	}

	void SetMallocPageSize(const u32 size) { malloc_page_size = size; }

	u64 GetTLSAddr() const { return tls_addr; }
	u64 GetTLSFilesz() const { return tls_filesz; }
	u64 GetTLSMemsz() const { return tls_memsz; }

	u32 GetMallocPageSize() const { return malloc_page_size; }

	void CheckStatus();

	virtual void Run();
	virtual void Pause();
	virtual void Resume();
	virtual void Stop();

	virtual bool IsRunned() const { return m_status == Runned; }
	virtual bool IsPaused() const { return m_status == Paused; }
	virtual bool IsStoped() const { return m_status == Stoped; }
};

extern Emulator Emu;