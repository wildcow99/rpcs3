#pragma once
#include "Thread.h"
#include "Emu/Memory/MemoryBlock.h"
#include "Emu/System.h"

class PPCThread// : public StepThread
{
protected:
	Status m_status;
	u32 m_error;
	void* m_dec;
	void* DisAsmFrame;
	u32 m_id;
	bool isSPU;
	u64 m_arg;

public:
	MemoryBlock Stack;
	u32 stack_num;
	u32 stack_size;
	u32 stack_addr;

	virtual void _InitStack()=0;

	virtual void InitStack();
	virtual void CloseStack();
	
	virtual u64 GetStackAddr() const { return stack_addr; }
	virtual u64 GetStackSize() const { return stack_size; }
	virtual u64 GetFreeStackSize() const=0;
	void SetArg(const u64 arg) { m_arg = arg; }

	void SetId(const u32 id);
	
public:
	bool isBranch;

	u32 PC;
	u32 nPC;
	u64 cycle;

protected:
	PPCThread(bool _isSPU);
	~PPCThread();

public:
	void NextPc();
	void NextBranchPc();
	void PrevPc();
	void SetBranch(const u32 pc);
	void SetPc(const u32 pc);

	void SetError(const u32 error);

	static wxArrayString ErrorToString(const u32 error);
	wxArrayString ErrorToString() { return ErrorToString(m_error); }

	bool IsSPU()	const { return isSPU; }
	bool IsOk()		const { return m_error == 0; }
	bool IsRunned()	const { return m_status == Runned; }
	bool IsPaused()	const { return m_status == Paused; }
	bool IsStoped()	const { return m_status == Stoped; }

	u32 GetError() const { return m_error; }
	u16 GetId() const { return m_id; }

	void Reset();
	void Close();
	void Run();
	void Pause();
	void Resume();
	void Stop();

	virtual wxString RegsToString() { return wxEmptyString; }

	virtual void Exec();
	
protected:
	virtual void DoReset()=0;
	virtual void DoRun()=0;
	virtual void DoPause()=0;
	virtual void DoResume()=0;
	virtual void DoStop()=0;
	
private:
	virtual void DoCode(const s32 code)=0;
};