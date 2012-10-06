#pragma once
#include "Utilites/IdManager.h"
#include "Utilites/Thread.h"
#include "PPCThread.h"

class PPCThreadManager : public ThreadBase
{
	//IdManager m_threads_id;
	//ArrayF<PPUThread> m_ppu_threads;
	//ArrayF<SPUThread> m_spu_threads;
	ArrayF<PPCThread> m_threads;
	volatile bool m_exec;

public:
	PPCThreadManager();
	~PPCThreadManager();

	void Close();

	PPCThread& AddThread(bool isPPU);
	void RemoveThread(const u32 id);

	ArrayF<PPCThread>& GetThreads() { return m_threads; }
	s32 GetThreadNumById(bool isPPU, u32 id);
	//IdManager& GetIDs() {return m_threads_id;}

	void Exec();
	virtual void Task();
};