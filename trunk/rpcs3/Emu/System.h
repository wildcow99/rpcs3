#pragma once

#include "Thread.h"
#include "Emu/Decoder/Decoder.h"
#include "Emu/Opcodes/DisAsm.h"
#include "Emu/Opcodes/Interpreter.h"

#include "Gui/MemoryViewer.h"

class SysThread : public StepThread
{
	enum
	{
		RUN,
		PAUSE,
		STOP,
	};

	uint m_cur_state;

	enum
	{
		DisAsm,
		Interpreter,
	};

	Decoder* decoder;
	uint m_mode;

	MemoryViewerPanel* m_memory_viewer;

public:
	bool IsSlef;

	SysThread();

	~SysThread()
	{
	}

	virtual void SetSelf(wxString self_patch);
	virtual void SetElf(wxString elf_patch);

	virtual void Run();
	virtual void Pause();
	virtual void Resume();
	virtual void Stop(bool CloseFrames = true);

	virtual bool IsRunned() const { return m_cur_state == RUN; }
	virtual bool IsPaused() const { return m_cur_state == PAUSE; }
	virtual bool IsStoped() const { return m_cur_state == STOP; }
	virtual void Step();

	static void CleanupInThread(void* arg);
};

extern SysThread System;