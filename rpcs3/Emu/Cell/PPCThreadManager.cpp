#include "stdafx.h"
#include "PPCThreadManager.h"
#include "PPUThread.h"
#include "SPUThread.h"

PPCThreadManager::PPCThreadManager()
{
}

PPCThreadManager::~PPCThreadManager()
{
	Close();
}

void PPCThreadManager::Close()
{
	Exit();
	while(m_threads.GetCount())
	{
		m_threads[0].Close();
		RemoveThread(m_threads[0].GetId());
	}
	/*
	ID thread;
	u32 pos = 0;
	while(m_threads_id.GetNext(pos, thread))
	{
		(*(PPCThread*)thread.m_data).Close();
	}

	m_threads_id.Clear();
	*/
}

PPCThread& PPCThreadManager::AddThread(bool isPPU)
{
	PPCThread* new_thread = isPPU ? (PPCThread*)new PPUThread() : (PPCThread*)new SPUThread();

	const u32 id =
		Emu.GetIdManager().GetNewID(wxString::Format("%s Thread", isPPU ? "PPU" : "SPU"), new_thread, 0);
	new_thread->SetId(id);

	m_threads.Add(new_thread);

	return *new_thread;
}

void PPCThreadManager::RemoveThread(const u32 id)
{
	if(!Emu.GetIdManager().CheckID(id)) return;
	for(u32 i=0; i<m_threads.GetCount(); ++i)
	{
		if(m_threads[i].GetId() != id) continue;
		m_threads[i].Stop();
		m_threads.RemoveAt(i);
		break;
	}
	Emu.GetIdManager().RemoveID(id, false);
	Emu.CheckStatus();
}

void PPCThreadManager::Exec()
{
	DoStep();
}

void PPCThreadManager::Step()
{
	while(!TestDestroy())
	{
		//emulation
		for(u32 i=0; i<m_threads.GetCount() && Emu.IsRunned(); ++i)
		{
			m_threads[i].Exec();
		}

		//check status
		//Emu.CheckStatus();
		if(!Emu.IsRunned())
		{
			//ConLog.Warning("BREAK LOOP! [%d:%d]", Emu.IsPaused(), Emu.IsStoped());
			break;
		}
	}
}