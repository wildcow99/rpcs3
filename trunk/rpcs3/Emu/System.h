#pragma once

#include "Gui/MemoryViewer.h"
#include "Emu/Cell/PPCThreadManager.h"
#include "Utilites/Array.h"

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

public:
	wxString m_path;
	bool IsSelf;

	Emulator();

	virtual void SetSelf(const wxString& path);
	virtual void SetElf(const wxString& path);

	//CPUThread& AddThread(bool isSPU);
	//void RemoveThread(const u8 core);

	PPCThreadManager& GetCPU()
	{
		return m_thread_manager;
	}

	void CheckStatus();

	virtual void Run();
	virtual void Pause();
	virtual void Resume();
	virtual void Stop();
	/*
	ArrayF<CPUThread>& GetCPU()
	{
		return m_cpu_threads;
	}

	CPUThread& GetPPU(const u8 core)
	{
		return m_cpu_threads.Get(core);
	}

	CPUThread& GetSPU(const u8 core)
	{
		if(core >= m_cpu_threads.GetCount())
		{
			ConLog.Error("Get SPU error: unknown number! (%d)", core);
			return m_cpu_threads.Get(0);
		}

		return m_cpu_threads.Get(core);
	}
	*/

	virtual bool IsRunned() const { return m_status == Runned; }
	virtual bool IsPaused() const { return m_status == Paused; }
	virtual bool IsStoped() const { return m_status == Stoped; }
};

extern Emulator Emu;