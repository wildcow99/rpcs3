#pragma once
#include "Utilites/IdManager.h"
#include "Thread.h"
#include "PPCThread.h"

class PPCThreadManager : public StepThread
{
	//IdManager m_threads_id;
	//ArrayF<PPUThread> m_ppu_threads;
	//ArrayF<SPUThread> m_spu_threads;
	ArrayF<PPCThread> m_threads;

public:
	PPCThreadManager();
	~PPCThreadManager();

	void Close();

	PPCThread& AddThread(bool isPPU);
	void RemoveThread(const u32 id);

	ArrayF<PPCThread>& GetThreads() { return m_threads; }
	//IdManager& GetIDs() {return m_threads_id;}

	void Exec();
	virtual void Step();
};