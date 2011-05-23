#pragma once

#include <windows.h>
#include <process.h>
#include <afxmt.h>

//TODO

// -------------------------------------------------------
// Mutex

class AutoMutex
{
	HANDLE m_mutex;

	AutoMutex(const AutoMutex&);
	AutoMutex& operator=(const AutoMutex&);

public:
	AutoMutex()
	{
		m_mutex = CreateMutex(NULL, FALSE, NULL);
	}

	~AutoMutex()
	{
		CloseHandle(m_mutex);
	}

	const HANDLE GetMutex() const { return m_mutex; }
};

class MutexLocker
{
	HANDLE m_mutex;

	MutexLocker(const MutexLocker&);
	MutexLocker& operator=(const MutexLocker&);

public:

	MutexLocker(AutoMutex& mutex) : m_mutex(mutex.GetMutex())
	{
	}

	virtual void Lock()
	{
		WaitForSingleObject(m_mutex, INFINITE);
	}

	virtual void Unlock()
	{
		ReleaseMutex(m_mutex);
	}

	~MutexLocker()
	{
		Unlock();
	}
};

class Mutex
{
	AutoMutex mutex;
	MutexLocker locker;
	bool m_wait;

public:
	Mutex() : locker(mutex)
	{
		m_wait = false;
	}

	virtual void Wait()
	{
		m_wait = true;
		locker.Lock();
	}

	virtual void Post()
	{
		if(!m_wait) return;
		m_wait = false;
		locker.Unlock();
	}
};

// -------------------------------------------------------

// -------------------------------------------------------
// Thread

class Thread
{
	Mutex m_mtx_wait;
	Mutex m_mtx_done;
	bool m_exit;
	bool m_done;
	bool m_started;

public:
	Thread()
	{
		m_done = m_exit = m_started = false;
	}

	virtual void Task()=0;

	virtual void Start()
	{
		done = false;
		m_mtx_wait.Post();
	}

	virtual void StartThread()
	{
		if(m_started) return;
		m_started = true;
		_beginthread( DoTaskInThread, 0, NULL );
	}	

	virtual void WaitForResult()
	{
		if(!m_done) m_mtx_done.Wait();
	}

	virtual void Exit()
	{
		m_exit = true;
		m_mtx_wait.Post();
	}

private:
	virtual void DoTaskInThread()
	{
		while(!m_exit)
		{
			m_mtx_wait.Wait();

			if(m_exit) break;
			if(m_done) continue;

			Task();

			m_done = true;
			m_mtx_done.Post();
		}
		_endthread();
	}
};
// -------------------------------------------------------