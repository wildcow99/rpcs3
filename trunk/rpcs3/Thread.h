#pragma once

#include "stdafx.h"
#include <pthread.h>
#include <semaphore.h>

class Semaphore
{
	sem_t semaphore;
	bool isWait;

public:
	Semaphore(bool is_create = true)
	{
		if(is_create) Create();
	}

	~Semaphore()
	{
		Destroy();
	}

	virtual void Create()
	{
		sem_init(&semaphore, 0, 0);
		isWait = false;
	}

	virtual void Wait()
	{
		isWait = true;
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
		isWait = false;
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
		isWait = false;
		sem_destroy(&semaphore);
	}

	virtual bool IsWait() const { return isWait; }
};

class Mutex
{
	pthread_mutex_t mutex;
	bool isLocked;

public:
	Mutex(bool is_create = true)
	{
		if(is_create) Create();
	}

	~Mutex()
	{
		Destroy();
	}

	virtual void Create()
	{
		pthread_mutex_init(&mutex, NULL);
		Lock();
	}

	virtual void Lock()
	{
		if(isLocked) return;
		isLocked = true;
		pthread_mutex_lock(&mutex);
	}

	virtual void TryLock()
	{
		pthread_mutex_trylock(&mutex);
	}

	virtual void Unlock()
	{
		if(!isLocked) return;
		isLocked = false;
		pthread_mutex_unlock(&mutex);
	}

	virtual void Destroy()
	{
		isLocked = false;
		pthread_mutex_destroy(&mutex);
	}

	virtual bool IsLocked() const { return isLocked; }
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
	ThreadBase(const wxString& _name = "Unknown thread") : name(_name)
	{
		isStarted = false;
	}

	~ThreadBase()
	{
		if(isStarted) Exit();
	}

    virtual void Task() = 0;
 
    const bool Start(bool is_detach = false, s32 stacksize = -1)
	{
		if(isStarted) return false;

		isStarted = true;
		isDetached = is_detach;

		pthread_attr_t attr;

		pthread_attr_init(&attr);
		if(is_detach) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if(stacksize != -1) pthread_attr_setstacksize(&attr, stacksize);

		const bool ret = pthread_create(&thread, &attr, ThreadBase::StartThread, (void*)this) == 0;

		pthread_attr_destroy(&attr);

		return ret;
	}

	virtual const bool Cancel()
	{
		if(!isStarted) return false;
		isStarted = false;

		return pthread_cancel(thread) == 0;
	}

	virtual const bool Exit()
	{
		if(!isStarted) return false;

		Detach();
		//Cancel();
		return pthread_join(thread, NULL) == 0;
	}

	const bool Detach()
	{
		return pthread_detach(thread) == 0;
	}

	const bool SetCancelState(bool enable, int* prev_state = NULL)
	{
		return pthread_setcancelstate(enable ? PTHREAD_CANCEL_ENABLE : PTHREAD_CANCEL_DISABLE, prev_state) == 0;
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

	static void Exit(void* arg = NULL)
	{
		pthread_exit(arg);
	}

	static void TestCancel()
	{
		pthread_testcancel();
	}
};

class StepThread : public ThreadBase
{
	Semaphore m_main_sem;
	Mutex m_main_mtx;
	bool exit;

protected:
	StepThread(const wxString& name = "Unknown StepThread")
		: ThreadBase(name)
		, exit(false)
	{
	}

	virtual const bool Exit()
	{
		const bool ret = ThreadBase::Exit();
		exit = true;
		if(IsWait()) m_main_sem.Post();
		return ret;
	}

	bool IsWait() const
	{
		return m_main_sem.IsWait();
	}

	bool IsExit() const
	{
		return exit;
	}

private:
	virtual void Task()
	{
		while(!exit)
		{
			m_main_sem.Wait();
			m_main_mtx.Lock();
			if(exit) break;
			ThreadAdv::TestCancel();
			Step();
			ThreadAdv::TestCancel();
			m_main_mtx.Unlock();
		}

		Detach();
		ThreadAdv::Exit();
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