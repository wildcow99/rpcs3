#pragma once

#include "stdafx.h"
#include <pthread.h>
#include <semaphore.h>

class Semaphore
{
	sem_t semaphore;

public:
	Semaphore(bool is_create = true)
	{
		if(is_create) Create();
	}

	virtual void Create()
	{
		sem_init(&semaphore, 0, 0);
	}

	virtual void Wait()
	{
		sem_wait(&semaphore);
	}

	virtual bool Wait( const wxTimeSpan& wtime )
	{
		const wxDateTime stime( wxDateTime::UNow() + wtime );
		const timespec fstime = { stime.GetTicks(), stime.GetMillisecond() * 1000000 };

		return sem_timedwait( &semaphore, &fstime ) == 0;
	}

	virtual void Post(const u32 count = 1)
	{
		if(count > 1)
		{
			sem_post_multiple(&semaphore, count);
		}
		else
		{
			sem_post(&semaphore);
		}
	}

	virtual void Destroy()
	{
		sem_destroy(&semaphore);
	}
};

class ThreadBase
{
public:
    pthread_t thread;

private:
	volatile bool isStarted;
	volatile bool isDetached;
	wxString name;
    static void* StartThread(void* arg)   { ((ThreadBase*)arg)->Task(); return NULL; }
 
public: 
	ThreadBase() : name("Unknown thread")
	{
		isStarted = false;
	}

	~ThreadBase()
	{
		if(isStarted) Exit();
	}

    virtual void Task() = 0;
 
    const bool Start(bool is_detach = false)
	{
		if(isStarted) return false;

		isStarted = true;
		isDetached = is_detach;

		if(isDetached)
		{
			pthread_attr_t attr;

			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			const bool ret = pthread_create(&thread, &attr, ThreadBase::StartThread, (void*)this) == 0;

			pthread_attr_destroy(&attr);

			return ret;
		}

		return pthread_create(&thread, NULL, ThreadBase::StartThread, (void*)this) == 0;
	}

	const bool Exit()
	{
		if(!isStarted) return false;
		isStarted = false;

		return pthread_cancel(thread) == 0;
	}

	const bool SetCancelState(bool enable, int* prev_state = NULL)
	{
		if(enable)
		{
			return pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, prev_state) == 0;
		}

		return pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, prev_state) == 0;
	}

	void SetName(wxString name)
	{
		this->name = name;
	}

	wxString GetName()
	{
		return name;
	}

	const bool IsStarted() const { return isStarted; }
	const bool IsDetached() const { return isDetached; }
};

struct ThreadAdv
{
	static void Sleep(const int ms)
	{
		::Sleep( ms );
	}

	static void SpinWait()
	{
		_asm pause
	}

	static void StoreFence()
	{
		__asm sfence
	}

	static ptw32_cleanup_t* CleanUpPush(void (*func)(void*), void* arg = NULL)
	{
		if(func == NULL) return NULL;

		ptw32_cleanup_t* _cleanup = NULL;
        ptw32_push_cleanup( _cleanup, (ptw32_cleanup_callback_t) (func), arg );

		return _cleanup;
	}

	static ptw32_cleanup_t* CleanUpPop(const int execute = 1)
	{
		return ptw32_pop_cleanup( execute );
	}

	static void ExitAndPostArg(void* arg)
	{
		pthread_exit(arg);
	}

	static void* WaitForExitAndGetPostedArg(ThreadBase& thr)
	{
		if(!thr.IsStarted())
		{
			wxMessageBox("Error wait for exit: Thread is not started!", "Thread " + thr.GetName() + " error");
			return NULL;
		}

		if(thr.IsDetached())
		{
			wxMessageBox("Error wait for exit: Thread is detached!", "Thread " + thr.GetName() + " error");
			return NULL;
		}

		void* arg;
		const int r = pthread_join(thr.thread, &arg);
		if(r != 0)
		{
			wxMessageBox(wxString::Format("Error join to thread: [%d] %s", r, strerror(r)),
				"Thread " + thr.GetName() + " error");
			return NULL;
		}

		return arg;
	}

	static void Exit()
	{
		pthread_exit(NULL);
	}

	static void TestCancel()
	{
		pthread_testcancel();
	}
};

class StepThread : public ThreadBase
{
	Semaphore m_main_sem;
	bool doCleanupEveryStep;

protected:
	StepThread()
	{
	}

private:
	virtual void Task()
	{
		for(;;)
		{
			m_main_sem.Wait();

			//CleanUpPush(_CleanupInThread, this);

			Step();

			//CleanUpPop(doCleanupEveryStep ? 1 : 0);
			//TestCancel();
		}
	}

	static void _CleanupInThread(void* arg) {((StepThread*)arg)->CleanupInThread();}
	virtual void CleanupInThread() {}
	virtual void Step()=0;

public:
	void DoStep()
	{
		m_main_sem.Post();
	}
};