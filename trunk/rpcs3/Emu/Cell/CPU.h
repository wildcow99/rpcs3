#pragma once
#include "Thread.h"

class CPUThread : public StepThread
{
protected:
	enum CPUStatus
	{
		Runned,
		Paused,
		Stoped,
	};

	u32 m_status;
	u32 m_error;
	void* m_dec;
	u16 m_id;
	bool isSPU;
	
public:
	bool isBranch;

	u32 PC;
	u32 nPC;
	u64 cycle;

protected:
	CPUThread(bool _isSPU, const u16 id);
	~CPUThread();

public:
	void NextPc();
	void NextBranchPc();
	void PrevPc();
	void SetBranch(const u32 pc);
	void SetPc(const u32 pc);

	void SetError(const u32 error);

	bool IsSPU()	const { return isSPU; }
	bool IsOk()		const { return m_error == 0; }
	bool IsRunned()	const { return m_status == Runned; }
	bool IsPaused()	const { return m_status == Paused; }
	bool IsStoped()	const { return m_status == Stoped; }

	u32  GetError() const { return m_error; }

	void Reset();
	void Close();
	void Run();
	void Pause();
	void Resume();
	void Stop();
	void SysResume();
	
protected:
	virtual void DoReset()=0;
	virtual void DoRun()=0;
	virtual void DoPause()=0;
	virtual void DoResume()=0;
	virtual void DoStop()=0;
	virtual void DoSysResume()=0;
	
private:
	virtual void Step();
	virtual void DoCode(const s32 code)=0;
};