#pragma once
#include "Thread.h"

class SPUThread : public CPUThread
{
public:
	s32 GPR[128]; //General-Purpose Register
	u64 cycle;

public:
	SPUThread(const u16 id);
	~SPUThread();

protected:
	virtual void DoReset();
	virtual void DoRun();
	virtual void DoPause();
	virtual void DoResume();
	virtual void DoStop();
	virtual void DoSysResume();

private:
	virtual void DoCode(const s32 code);
};