#pragma once

// -------------------------------------------------------
// Thread

class Thread : public wxThread
{
	wxSemaphore m_sem_wait;
	wxSemaphore m_sem_done;
	bool m_exit;
	bool m_done;
	bool m_started;

public:
	Thread() : wxThread(wxTHREAD_JOINABLE)
	{
		m_exit = false;
		m_started = m_done = true;

		wxThread::Create();
		wxThread::Run();
	}

	virtual void Task()=0;

	virtual void Start()
	{
		m_done = false;
		m_sem_wait.Post();
	}

	virtual void StartThread()
	{
		if(m_started) return;
		m_started = true;

		wxThread::Run();
	}	

	virtual void WaitForResult()
	{
		if(!m_done) m_sem_done.Wait();
	}

	virtual void Exit()
	{
		m_exit = true;
		m_sem_wait.Post();

		wxThread::OnExit();
	}

	virtual void* Entry()
	{
		while(!m_exit)
		{
			m_sem_wait.Wait();

			if ( TestDestroy() ) break;

			if(m_exit) break;
			if(m_done) continue;

			Task();

			m_done = true;
			m_sem_done.Post();
		}

		m_started = false;

		return NULL;
	}
};

// -------------------------------------------------------