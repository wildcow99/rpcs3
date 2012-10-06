#pragma once
#include "Array.h"

class ThreadBase : public wxThread
{
protected:
	wxString m_name;

protected:
	ThreadBase(bool detached = true, const wxString& name = "Unknown ThreadBase")
		: wxThread(detached ? wxTHREAD_DETACHED : wxTHREAD_JOINABLE)
	{
	}

	virtual ~ThreadBase() throw()
	{
		if(IsAlive()) Delete();
	}

	ExitCode Entry()
	{
		Task();
		return (ExitCode)0;
	}

	virtual void Task()=0;

public:
	void Start()
	{
		if(IsAlive()) return;

		if(IsPaused())
		{
			Resume();
		}
		else
		{
			Create();
			Run();
		}
	}
};

template<typename T> class MTPacketBuffer
{
protected:
	volatile bool m_busy;
	volatile u32 m_put, m_get;
	Array<u8> m_buffer;
	u32 m_max_buffer_size;

	void CheckBusy()
	{
		m_busy = m_put >= m_max_buffer_size;
	}

public:
	MTPacketBuffer(u32 max_buffer_size)
		: m_max_buffer_size(max_buffer_size)
	{
		Flush();
	}

	~MTPacketBuffer()
	{
		Flush();
	}

	void Flush()
	{
		m_put = m_get = 0;
		m_buffer.Clear();
		m_busy = false;
	}

	virtual void Push(const T& v) = 0;
	virtual T Pop() = 0;

	bool HasNewPacket() const { return m_put != m_get; }
	bool IsBusy() const { return m_busy; }
};

/*
class StepThread : public ThreadBase
{
	wxSemaphore m_main_sem;
	wxSemaphore m_destroy_sem;
	volatile bool m_exit;

protected:
	StepThread(const wxString& name = "Unknown StepThread")
		: ThreadBase(true, name)
		, m_exit(false)
	{
	}

	virtual ~StepThread() throw()
	{
	}

private:
	virtual void Task()
	{
		m_exit = false;

		while(!TestDestroy())
		{
			m_main_sem.Wait();

			if(TestDestroy() || m_exit) break;

			Step();
		}

		while(!TestDestroy()) Sleep(0);
		if(m_destroy_sem.TryWait() != wxSEMA_NO_ERROR) m_destroy_sem.Post();
	}

	virtual void Step()=0;

public:
	void DoStep()
	{
		if(IsRunning()) m_main_sem.Post();
	}

	void WaitForExit()
	{
		if(TestDestroy()) m_destroy_sem.Wait();
	}

	void WaitForNextStep()
	{
		if(!IsRunning()) return;

		while(m_main_sem.TryWait() != wxSEMA_NO_ERROR) Sleep(0);
	}

	void Exit(bool wait = false)
	{
		if(!IsAlive()) return;

		if(m_main_sem.TryWait() != wxSEMA_NO_ERROR)
		{
			m_exit = true;
			m_main_sem.Post();
		}

		Delete();

		if(wait) WaitForExit();
	}
};
*/