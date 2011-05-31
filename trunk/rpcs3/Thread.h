#pragma once

// -------------------------------------------------------
// Mutex (TODO)

class Mutex : public wxMutex
{
	bool m_wait;

public:
	Mutex() 
		: m_wait(false)
		, wxMutex()
	{
	}

	virtual void Wait()
	{
		if(m_wait) return;
		m_wait = true;
		wxMutex::Lock();
	}

	virtual void Post()
	{
		if(!m_wait) return;
		m_wait = false;
		wxMutex::Unlock();
	}
};

// -------------------------------------------------------

// -------------------------------------------------------
// ThreadBase

//static DWORD gs_tlsThisThread;

class ThreadBase
{
public:
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
/*
class Thread : private ThreadBase
{
	volatile bool m_started;

	volatile HANDLE m_hThread;

public:

	wxFile& thread_log;
	
	wxCriticalSection m_critsect;

	Thread(bool start = false)
		: thread_log(*new wxFile("Thread.log", wxFile::write))
		, m_sem_wait(1, 2)
		, m_sem_done(1, 9000)
	{
		m_done = m_started = true;
		m_exit = false;

		m_hThread = CreateThread(start);
		
		if(m_hThread == NULL)
		{
			thread_log.Write("Error create thread\n");
		}
	}

	

	virtual HANDLE CreateThread( int prior = THREAD_PRIORITY_NORMAL, bool start = false )
	{
		/*
			THREAD_PRIORITY_LOWEST
			THREAD_PRIORITY_BELOW_NORMAL
			THREAD_PRIORITY_NORMAL
			THREAD_PRIORITY_ABOVE_NORMAL
			THREAD_PRIORITY_HIGHEST
		*//*

		HANDLE ret = ::CreateThread
		(
			NULL,
			0,
			Thread::ThreadStart,
			(LPVOID)this,
			/*CREATE_SUSPENDED*//*0,
			NULL
		);

		::SetThreadPriority(ret, prior);

		return ret;
	}

	virtual void* Entry(Thread* thread)=0;

	virtual void Start()
	{
		wxCriticalSectionLocker lock(m_critsect);
		m_sem_wait.Post();
	}

	virtual void StartThread()
	{
		if(m_started) return;
		m_started = true;

		if(m_hThread == NULL)
		{
			m_hThread = CreateThread(true);
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
		if(!m_done) m_sem_done.Wait();
	}

	virtual void Exit()
	{
		m_started = false;
		m_exit = true;
		m_sem_wait.Post();
		ThreadBase::DeleteThread(m_hThread);
	}
};
*/
// -------------------------------------------------------
/*
class MThread : private ThreadBase
{
	volatile int m_threadcount;
	volatile HANDLE* m_hThreads;
	volatile uint m_cur_thread;
	wxCriticalSection m_critsect;
	Mutex* m_mtx_done;
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

			m_mtx_done = new Mutex[m_threadcount];
			m_done = new bool[m_threadcount];
		}
		else
		{
			m_threadcount = 0;
			m_mtx_done = new Mutex[1];
			m_done = new bool[1];
		}
	}

	virtual void Task()=0;

	virtual void WaitForResult()
	{
		if(m_threadcount == 0)
		{
			if(!m_done[0]) m_mtx_done[0].Wait();
		}
		else
		{
			const uint i = m_cur_thread == 0 ? (uint)m_threadcount - 1 : m_cur_thread - 1;
			if(!m_done[i]) m_mtx_done[i].Wait();
		}
	}

	virtual void* Entry()
	{
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
			m_mtx_done[0].Post();
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

			m_mtx_done[i].Post();
			
		}
	}
};*/