#pragma once

#include "stdafx.h"

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

class StepThread : public ThreadBase
{
	wxSemaphore m_main_sem;

protected:
	StepThread(bool detached = true, const wxString& name = "Unknown StepThread")
		: ThreadBase(detached, name)
	{
	}

	virtual ~StepThread() throw()
	{
		Exit();
	}

private:
	virtual void Task()
	{
		while(!TestDestroy())
		{
			m_main_sem.Wait();
			if(TestDestroy()) break;

			Step();
		}
	}

	virtual void Step()=0;

public:
	void DoStep()
	{
		if(IsAlive()) m_main_sem.Post();
	}

	void Exit()
	{
		if(!IsAlive()) return;
		if(m_main_sem.TryWait() != wxSEMA_NO_ERROR) m_main_sem.Post();
		Delete();
	}
};