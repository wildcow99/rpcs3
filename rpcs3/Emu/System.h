#pragma once

#include "Gui/MemoryViewer.h"
#include "Emu/Cell/PPCThreadManager.h"
#include "Utilites/Array.h"
#include "Emu/Io/Pad.h"
#include "Emu/DbgConsole.h"

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
	PadManager* m_pad_manager;
	DbgConsole* m_dbg_console;

public:
	wxString m_path;
	bool IsSelf;

	Emulator();

	void Init();
	virtual void SetSelf(const wxString& path);
	virtual void SetElf(const wxString& path);

	PPCThreadManager&	GetCPU()	{ return m_thread_manager; }
	PadManager&			GetPads()	{ return *m_pad_manager; }
	DbgConsole&			GetDbgCon()	{ return *m_dbg_console; }

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