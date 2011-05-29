#pragma once

#include "Thread.h"
#include "Emu/Decoder/Decoder.h"
#include "Emu/Opcodes/DisAsm.h"


class SysThread : private Thread
{
	enum
	{
		RUN,
		PAUSE,
		STOP,
	};

	uint m_cur_state;

	Mutex m_mtx_wait;
	Mutex m_mtx_pause;

	enum
	{
		DisAsm,
	};

	DisAsmOpcodes* disasm;
	Decoder* decoder;
	uint m_mode;

	bool IsSlef;

public:
	SysThread();

	virtual void SetSelf(wxString self_patch);
	virtual void SetElf(wxString elf_patch);

	virtual void Run();
	virtual void Pause();
	virtual void Stop();

	virtual bool IsRunned() const { return m_cur_state == RUN; }
	virtual bool IsPaused() const { return m_cur_state == PAUSE; }
	virtual bool IsStoped() const { return m_cur_state == STOP; }

private:
	virtual void Task();
};

extern SysThread System;