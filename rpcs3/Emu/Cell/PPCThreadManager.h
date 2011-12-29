#pragma once
#include "Utilites/IdManager.h"
#include "Thread.h"

class PPCThreadManager : public StepThread
{
	IdManager m_threads_id;
	//ArrayF<PPUThread> m_ppu_threads;
	//ArrayF<SPUThread> m_spu_threads;

public:
	PPCThreadManager();
	~PPCThreadManager();

	void Close();

	u32 AddThread(bool isPPU);
	void RemoveThread(const u32 id);

	IdManager& GetIDs() {return m_threads_id;}

	void Exec();
	virtual void Step();
};