#include "stdafx.h"
#include "Callback.h"

#include "Emu/Cell/PPCThread.h"

Callback::Callback(u32 slot, u64 addr, u64 userdata)
	: m_addr(addr)
	, m_slot(slot)
	, m_status(0)
	, m_param(0)
	, m_userdata(userdata)
	, m_has_data(false)
{
}

void Callback::Handle(u64 status, u64 param)
{
	m_status = status;
	m_param = param;
	m_has_data = true;
}

void Callback::Branch()
{
	PPCThread& new_thread = Emu.GetCPU().AddThread(true);

	new_thread.SetPc(m_addr);
	new_thread.SetArg(m_status);
	new_thread.SetPrio(0x1001);
	new_thread.stack_size = 0x10000;
	new_thread.SetName("Callback");
	new_thread.Run();

	((PPUThread&)new_thread).GPR[3] = 0x212;
	((PPUThread&)new_thread).GPR[4] = 0x399;
	((PPUThread&)new_thread).GPR[5] = 0x21;
	new_thread.Exec();

	while(new_thread.IsAlive()) Sleep(1);
	return;
	PPUThread& thr = GetCurrentPPUThread();

	u64 GPR[32];
	double FPR[32];
	VPR_reg VPR[32];
	u64 LR = thr.LR;
	u64 CTR = thr.CTR;
	memcpy(GPR, thr.GPR, sizeof(u64) * 32);
	memcpy(FPR, thr.FPR, sizeof(double) * 32);
	memcpy(VPR, thr.VPR, sizeof(VPR_reg) * 32);

	thr.GPR[1] -= 112;
	thr.GPR[3] = m_status;
	thr.GPR[4] = m_param;
	thr.GPR[5] = m_userdata;

	u64 lpc = thr.PC;
	thr.LR = thr.PC + 4;
	thr.PC = Memory.Read32(m_addr);
	thr.nPC = thr.PC + 4;

	while(thr.PC != lpc + 4 && Emu.IsRunned())
	{
		thr.DoCode(Memory.Read32(thr.PC));
		thr.NextPc();
	}

	memcpy(thr.GPR, GPR, sizeof(u64) * 32);
	memcpy(thr.FPR, FPR, sizeof(double) * 32);
	memcpy(thr.VPR, VPR, sizeof(VPR_reg) * 32);
	thr.CTR = CTR;
	thr.LR = LR;
	thr.PC = lpc;
	thr.nPC = thr.PC + 4;
	//thr.SetBranch(lpc);
	//thr.NextPc();
	ConLog.Write("EXIT!");
}