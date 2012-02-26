#pragma once

#include "Gui/MemoryViewer.h"
#include "Emu/Cell/PPCThreadManager.h"
#include "Utilites/Array.h"
#include "Emu/Io/Pad.h"
#include "Emu/DbgConsole.h"
#include "Emu/Display/Display.h"

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
#ifdef USE_GS_FRAME
	GSFrame_GL* m_gs_frame;
#endif
	//ArrayF<CPUThread> m_cpu_threads;

	PPCThreadManager m_thread_manager;
	PadManager* m_pad_manager;
	IdManager m_id_manager;
	DbgConsole* m_dbg_console;

	u64 tls_addr;
	u64 tls_filesz;
	u64 tls_memsz;

public:
	wxString m_path;
	bool IsSelf;

	Emulator();

	void Init();
	virtual void SetSelf(const wxString& path);
	virtual void SetElf(const wxString& path);

	PPCThreadManager&	GetCPU()		{ return m_thread_manager; }
	PadManager&			GetPadManager()	{ return *m_pad_manager; }
	IdManager&			GetIdManager()	{ return m_id_manager; }
	DbgConsole&			GetDbgCon()		{ return *m_dbg_console; }

	void SetTLSData(u64 addr, u64 filesz, u64 memsz)
	{
		tls_addr = addr;
		tls_filesz = filesz;
		tls_memsz = memsz;
	}

	u64 GetTLSAddr() const { return tls_addr; }
	u64 GetTLSFilesz() const { return tls_filesz; }
	u64 GetTLSMemsz() const { return tls_memsz; }

#ifdef USE_GS_FRAME
	GSFrame_GL&			GetGSFrame() { return *m_gs_frame; }
#endif

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