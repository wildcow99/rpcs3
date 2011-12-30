#include "stdafx.h"
#include "PPCThreadManager.h"
#include "PPUThread.h"
#include "SPUThread.h"

PPCThreadManager::PPCThreadManager()
{
	Start();
}

PPCThreadManager::~PPCThreadManager()
{
	Exit();
	Close();
}

void PPCThreadManager::Close()
{
	ID thread;
	u32 pos = 0;
	while(m_threads_id.GetNext(pos, thread))
	{
		(*(PPCThread*)thread.m_data).Close();
	}

	m_threads_id.Clear();
}

u32 PPCThreadManager::AddThread(bool isPPU)
{
	const u32 id =  m_threads_id.GetNewID(
		isPPU ? "PPU Thread"			: "SPU Thread",
		isPPU ? (void*)new PPUThread()	: (void*)new SPUThread(),
		isPPU ? 1						: 2);

	(*(PPCThread*)m_threads_id.GetIDData(id).m_data).SetId(id);
	return id;
}

void PPCThreadManager::RemoveThread(const u32 id)
{
	if(!m_threads_id.CheckID(id)) return;
	ID& thread = m_threads_id.GetIDData(id);
	(*(PPCThread*)thread.m_data).Stop();
	m_threads_id.RemoveID(id);
	Emu.CheckStatus();
}

void PPCThreadManager::Exec()
{
	DoStep();
}

void PPCThreadManager::Step()
{
	for(;;)
	{
		//emulation
		ID thread;
		u32 pos = 0;
		while(m_threads_id.GetNext(pos, thread))
		{
			(*(PPCThread*)thread.m_data).Exec();
		}

		//check status
		//Emu.CheckStatus();
		if(!Emu.IsRunned())
		{
			ConLog.Warning("BREAK LOOP! [%d:%d]", Emu.IsPaused(), Emu.IsStoped());
			break;
		}
	}
}