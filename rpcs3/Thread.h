#pragma once

// -------------------------------------------------------
// Mutex (TODO)

class Mutex
{
	wxMutex m_mtx;
	bool m_wait;

public:
	Mutex() : m_wait(false) {}

	virtual void Wait()
	{
		if(m_wait) return;
		m_wait = true;
		m_mtx.Lock();
	}

	virtual void Post()
	{
		if(!m_wait) return;
		m_wait = false;
		m_mtx.Unlock();
	}
};

// -------------------------------------------------------

// -------------------------------------------------------
// ThreadBase

class ThreadBase
{
public:
	virtual HANDLE CreateThread( int prior = THREAD_PRIORITY_NORMAL, bool start = false )
	{
		/*
			THREAD_PRIORITY_LOWEST
			THREAD_PRIORITY_BELOW_NORMAL
			THREAD_PRIORITY_NORMAL
			THREAD_PRIORITY_ABOVE_NORMAL
			THREAD_PRIORITY_HIGHEST
		*/

		HANDLE ret = ::CreateThread
		(
			NULL,
			0,
			NULL,
			(LPVOID)this,
			CREATE_SUSPENDED,
			NULL
		);

		::SetThreadPriority(ret, prior);

		if(start) ResumeThread(ret);

		return ret;
	}

	virtual bool KillThread( HANDLE thread )
	{
		return !!::TerminateThread(thread, (DWORD)-1);
	}

	virtual int SuspendThread( HANDLE thread )
	{
		return (int)::SuspendThread(thread);
	}

	virtual int ResumeThread( HANDLE thread )
	{
		return (int)::ResumeThread(thread);
	}

	virtual bool DeleteThread( HANDLE thread )
	{
		bool ret = true;

		ret = !!SuspendThread( thread ) && ret;

		if( thread != NULL )
        {
            ret = !!::CloseHandle(thread) && ret;

            thread = 0;
        }

		return ret;
	}
};

// -------------------------------------------------------

// -------------------------------------------------------
// Thread

class Thread : private ThreadBase
{
	volatile bool m_started;

	volatile HANDLE m_hThread;

	wxFile& thread_log;
	Mutex m_mtx_wait;
	volatile bool m_done;
	wxCriticalSection m_critsect;

public:
	Thread(bool start = false) : thread_log(*new wxFile("Thread.log", wxFile::write))
	{
		m_started = true;
		m_done = true;

		m_hThread = ThreadBase::CreateThread(start);
		
		if(m_hThread == NULL)
		{
			thread_log.Write("Error create thread\n");
		}
	}

	virtual void Task()=0;

	virtual void Start()
	{
		wxCriticalSectionLocker lock(m_critsect);

		m_done = false;
		Task();
		m_done = true;
		m_mtx_wait.Post();
	}

	virtual void StartThread()
	{
		if(m_started) return;
		m_started = true;

		if(m_hThread == NULL)
		{
			m_hThread = ThreadBase::CreateThread(true);
		}

		if(ThreadBase::ResumeThread(m_hThread) == -1)
		{
			thread_log.Write("Error resume thread\n");
		}
	}

	virtual void Suspend()
	{
		ThreadBase::SuspendThread(m_hThread);
	}

	virtual void WaitForResult()
	{
		if(!m_done) m_mtx_wait.Wait();
	}

	virtual void Exit()
	{
		m_started = false;
		ThreadBase::DeleteThread(m_hThread);
	}
};

// -------------------------------------------------------

class MThread : private ThreadBase
{
	volatile int m_threadcount;
	volatile HANDLE* m_hThreads;
	volatile uint m_cur_thread;
	wxCriticalSection m_critsect;
	Mutex* m_mtx_wait;
	volatile bool* m_done;

public:
	MThread(bool setByCPUCount = true)
	{
		m_cur_thread = 0;

		if(setByCPUCount)
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);

			m_threadcount = si.dwNumberOfProcessors;

			if(m_threadcount < 1)
			{
				m_threadcount = 2;
			}

			m_hThreads = new HANDLE[m_threadcount];

			for(int i=0; i<m_threadcount; ++i)
			{
				m_hThreads[i] = ThreadBase::CreateThread();
			}

			m_mtx_wait = new Mutex[m_threadcount];
			m_done = new bool[m_threadcount];
		}
		else
		{
			m_threadcount = 0;
			m_mtx_wait = new Mutex[1];
			m_done = new bool[1];
		}
	}

	virtual void Task()=0;

	virtual void WaitForResult()
	{
		if(m_threadcount == 0)
		{
			if(!m_done[0]) m_mtx_wait[0].Wait();
		}
		else
		{
			const uint i = m_cur_thread == 0 ? (uint)m_threadcount - 1 : m_cur_thread - 1;
			if(!m_done[i]) m_mtx_wait[i].Wait();
		}
	}

	virtual void Start()
	{
		wxCriticalSectionLocker lock(m_critsect);

		if(m_threadcount == 0)
		{
			m_done[0] = false;

			volatile HANDLE hThread = ThreadBase::CreateThread(true);

			Task();

			ThreadBase::DeleteThread(hThread);

			m_done[0] = true;
			m_mtx_wait[0].Post();
		}
		else
		{
			const volatile uint i = m_cur_thread++;
			if(m_cur_thread >= (uint)m_threadcount) m_cur_thread=0;

			m_done[i] = false;

			ThreadBase::ResumeThread(m_hThreads[i]);

			Task();

			ThreadBase::SuspendThread(m_hThreads[i]);

			m_done[i] = true;

			m_mtx_wait[i].Post();
			
		}
	}
};