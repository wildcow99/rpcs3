#include "stdafx.h"
#include "PPCThreadManager.h"
#include "PPUThread.h"
#include "SPUThread.h"

PPCThreadManager::PPCThreadManager()
	: ThreadBase(true, "PPCThreadManager")
	, m_exec(false)
{
	//ThreadBase::Start();
}

PPCThreadManager::~PPCThreadManager()
{
	Close();
}

void PPCThreadManager::Close()
{
	if(Emu.IsRunned()) Emu.Pause();

	while(m_exec) Sleep(1);

	while(m_threads.GetCount())
	{
		m_threads[0].Close();
		RemoveThread(m_threads[0].GetId());
	}
}

PPCThread& PPCThreadManager::AddThread(bool isPPU)
{
	PPCThread* new_thread = isPPU ? (PPCThread*)new PPUThread() : (PPCThread*)new SPUThread();

	new_thread->SetId(Emu.GetIdManager().GetNewID(
		wxString::Format("%s Thread", isPPU ? "PPU" : "SPU"), new_thread, 0)
	);

	m_threads.Add(new_thread);

	return *new_thread;
}

void PPCThreadManager::RemoveThread(const u32 id)
{
	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].GetId() != id) continue;

		m_threads[i].Stop();
		m_threads.RemoveAt(i);

		break;
	}

	if(Emu.GetIdManager().CheckID(id))
	{
		Emu.GetIdManager().RemoveID(id, false);
	}

	Emu.CheckStatus();
}

s32 PPCThreadManager::GetThreadNumById(bool isPPU, u32 id)
{
	s32 num = 0;

	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].GetId() == id) return num;
		if(m_threads[i].IsSPU() == !isPPU) num++;
	}

	return -1;
}

void PPCThreadManager::Exec()
{
	m_exec = true;
}

void PPCThreadManager::Task()
{
	u32 thread = 0;

	while(!TestDestroy())
	{
		if(!m_exec)
		{
			Sleep(1);
			continue;
		}

		m_threads[thread].Exec();

		thread = (thread + 1) % m_threads.GetCount();
		if(!Emu.IsRunned()) m_exec = false;
	}
}