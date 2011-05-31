#pragma once

#include "Thread.h"
#include "Emu/Decoder/Decoder.h"
#include "Emu/Opcodes/DisAsm.h"
#include "Emu/Opcodes/Interpreter.h"

class SysThread : public ThreadBase
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

	HANDLE m_hThread;
	wxSemaphore m_sem_wait;
	wxSemaphore m_sem_done;
	volatile bool m_done;
	volatile bool m_exit;

public:
	bool IsSlef;

	SysThread();

	~SysThread()
	{
		m_exit = true;
		m_sem_wait.Post();
		ThreadBase::DeleteThread(m_hThread);
	}

	virtual void SetSelf(wxString self_patch);
	virtual void SetElf(wxString elf_patch);

	virtual void Run();
	virtual void Pause();
	virtual void Stop();

	virtual bool IsRunned() const { return m_cur_state == RUN; }
	virtual bool IsPaused() const { return m_cur_state == PAUSE; }
	virtual bool IsStoped() const { return m_cur_state == STOP; }
	virtual void Task();

	virtual void* Entry()
	{
		while(!m_exit)
		{
			m_sem_wait.Wait();

			if(!m_done) WaitForResult();

			m_done = false;

			if(m_exit) break;
			if(m_done) continue;

			Task();

			m_done = true;
			m_sem_done.Post();
		}

		return NULL;
	}

	static DWORD __stdcall ThreadStart(void* thread)
	{
		return (DWORD)((SysThread*)thread)->Entry();
	}

	virtual void WaitForResult()
	{
		if(!m_done) m_sem_done.Wait();
	}
};

extern SysThread System;
