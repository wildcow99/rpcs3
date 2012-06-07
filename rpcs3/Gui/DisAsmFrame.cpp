#include "stdafx.h"
#include "DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPCThread.h"
#include "Emu/System.h"
#include "Gui/DisAsmFrame.h"
#include "Emu/Cell/PPUDecoder.h"
#include "Emu/Cell/PPUDisAsm.h"
#include "Emu/Cell/SPUDecoder.h"
#include "Emu/Cell/SPUDisAsm.h"

DisAsmFrame::DisAsmFrame(PPCThread& cpu)
	: wxFrame(NULL, wxID_ANY, "DisAsm")
	, CPU(cpu)
{
	exit = false;
	count = 0;
	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );
	wxBoxSizer& s_b_panel( *new wxBoxSizer(wxHORIZONTAL) );

	m_disasm_list = new wxListView(this);

	wxButton& b_fprev	= *new wxButton(this, wxID_ANY, L"<<");
	wxButton& b_prev	= *new wxButton(this, wxID_ANY, L"<");
	wxButton& b_next	= *new wxButton(this, wxID_ANY, L">");
	wxButton& b_fnext	= *new wxButton(this, wxID_ANY, L">>");

	wxButton& b_dump	= *new wxButton(this, wxID_ANY, L"Dump code");

	wxButton& b_setpc	= *new wxButton(this, wxID_ANY, L"Set PC");

	s_b_panel.Add(&b_fprev);
	s_b_panel.Add(&b_prev);
	s_b_panel.AddSpacer(5);
	s_b_panel.Add(&b_next);
	s_b_panel.Add(&b_fnext);
	s_b_panel.AddSpacer(8);
	s_b_panel.Add(&b_dump);

	s_panel.Add(&s_b_panel);
	s_panel.Add(&b_setpc);
	s_panel.Add(m_disasm_list);

	m_disasm_list->InsertColumn(0, "PC");
	m_disasm_list->InsertColumn(1, "ASM");

	m_disasm_list->SetColumnWidth( 0, 50 );

	for(uint i=0; i<LINES_OPCODES; ++i) m_disasm_list->InsertItem(i, -1);

	SetSizerAndFit( &s_panel );

	SetSize(50, 660);

	Connect( wxEVT_SIZE, wxSizeEventHandler(DisAsmFrame::OnResize) );

	wxGetApp().Connect(m_disasm_list->GetId(), wxEVT_MOUSEWHEEL, wxMouseEventHandler(DisAsmFrame::MouseWheel), (wxObject*)0, this);

	Connect(b_prev.GetId(),  wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Prev));
	Connect(b_next.GetId(),  wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Next));
	Connect(b_fprev.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fPrev));
	Connect(b_fnext.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fNext));
	Connect(b_setpc.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::SetPc));

	Connect(b_dump.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Dump));
}

void DisAsmFrame::OnResize(wxSizeEvent& event)
{
	const wxSize size(GetClientSize());
	m_disasm_list->SetSize( size.GetWidth(), size.GetHeight() - 25 );
	m_disasm_list->SetColumnWidth( 1, size.GetWidth() - (m_disasm_list->GetColumnWidth(0) + 8) );
}

void DisAsmFrame::AddLine(const wxString line)
{
	static bool finished = false;

	if(finished && Emu.IsRunned())
	{
		count = 0;
		finished = false;
	}
	else if(count >= LINES_OPCODES || !Emu.IsRunned())
	{
		if(Emu.IsRunned()) Emu.Pause();
		finished = true;
		CPU.PrevPc();
		return;
	}

	m_disasm_list->SetItem(count, 0, wxString::Format("%llx", CPU.PC));
	m_disasm_list->SetItem(count, 1, line);

	++count;
}

void DisAsmFrame::Resume()
{
	Emu.Resume();
}

#include "Utilites/Thread.h"
#include <Utilites/MTProgressDialog.h>
#include "Loader/ELF.h"
Array<Elf64_Shdr>* shdr_arr_64 = NULL;
Array<Elf32_Shdr>* shdr_arr_32 = NULL;
ELF64Loader* l_elf64 = NULL;
ELF32Loader* l_elf32 = NULL;
bool ElfType64 = false;

class DumperThread : public StepThread
{
	volatile uint id;
	DisAsm* disasm;
	Decoder* decoder;
	volatile bool* done;
	volatile u8 cores;
	MTProgressDialog* prog_dial;
	wxArrayString** arr;

public:
	DumperThread() : StepThread()
	{
	}

	void Set(uint _id, u8 _cores, bool* _done, MTProgressDialog& _prog_dial, wxArrayString** _arr)
	{
		id = _id;
		cores = _cores;
		done = _done;
		prog_dial = &_prog_dial;
		arr = _arr;

		*done = false;

		if(Emu.GetCPU().GetThreads()[0].IsSPU())
		{
			SPU_DisAsm& dis_asm = *new SPU_DisAsm(*(PPCThread*)NULL, DumpMode);
			decoder = new SPU_Decoder(dis_asm);
			disasm = &dis_asm;
		}
		else
		{
			PPU_DisAsm& dis_asm = *new PPU_DisAsm(*(PPCThread*)NULL, DumpMode);
			decoder = new PPU_Decoder(dis_asm);
			disasm = &dis_asm;
		}
	}

	virtual void Step()
	{
		ConLog.Write("Start dump in thread %d!", (int)id);
		const u32 max_value = prog_dial->GetMaxValue(id);

		//wxFile f(Emu.m_path);

		const u32 shdr_count = ElfType64 ? shdr_arr_64->GetCount() : shdr_arr_32->GetCount();

		for(u32 sh=0, vsize=0; sh<shdr_count; ++sh)
		{
			//const Elf64_Shdr& c_sh = (*shdr_arr)[sh];
			const u64 sh_size = (ElfType64 ? (*shdr_arr_64)[sh].sh_size : (*shdr_arr_32)[sh].sh_size) / 4;
			const u64 sh_addr = (ElfType64 ? (*shdr_arr_64)[sh].sh_addr : (*shdr_arr_32)[sh].sh_addr);

			u64 d_size = sh_size / cores;
			const u64 s_fix = sh_size % cores;
			if(id <= s_fix) d_size++;

			for(u64 off = id * 4, size=0; size<d_size; vsize++, size++)
			{
				prog_dial->Update(id, vsize,
					wxString::Format("%d thread: %d of %d", (int)id + 1, vsize, max_value));

				disasm->dump_pc = sh_addr + off;
				//f.Seek(c_sh.sh_offset + off);
				//u32 buf;
				//f.Read(&buf, sizeof(buf));
				decoder->Decode(Memory.Read32(disasm->dump_pc));

				arr[id][sh].Add(disasm->last_opcode);

				off += (cores - id) * 4;
				off += id * 4;
			}
		}

		ConLog.Write("Finish dump in thread %d!", (int)id);

		//f.Close();
		*done = true;

		Cleanup();

		StepThread::Exit();
		ThreadAdv::Exit();
		delete this;
	}

	void Cleanup()
	{
		ConLog.Write("CleanUp dump thread (%d)!", (int)id);
		safe_delete(decoder);
	}
};

struct WaitDumperThread : public ThreadBase
{
	volatile bool* done;
	volatile u8 cores;
	wxString patch;
	MTProgressDialog& prog_dial;
	wxArrayString** arr;

	WaitDumperThread(bool* _done, u8 _cores, wxString _patch, MTProgressDialog& _prog_dial, wxArrayString** _arr) 
		: ThreadBase()
		, done(_done)
		, cores(_cores)
		, patch(_patch)
		, prog_dial(_prog_dial)
		, arr(_arr)
	{
	}

	~WaitDumperThread()
	{
		free((bool*)done);
		done = NULL;
	}

	virtual void Task()
	{
		for(uint i=0; i<cores;)
		{
			while(done[i] == false) ThreadAdv::Sleep(1000);
				
			i++;
		}

		ConLog.Write("Saving dump is started!");
		const uint length_for_core = prog_dial.GetMaxValue(0);
		const uint length = length_for_core * cores;
		prog_dial.Close();

		wxArrayLong max;
		max.Add(length);
		MTProgressDialog& prog_dial2 = 
			*new MTProgressDialog(NULL, wxDefaultSize, "Saving", "Loading...", max, 1);

		wxFile fd;
		fd.Open(patch, wxFile::write);

		const u32 shdr_count = ElfType64 ? shdr_arr_64->GetCount() : shdr_arr_32->GetCount();

		for(uint sh=0, counter=0; sh<shdr_count; ++sh)
		{
			const u64 sh_size = ElfType64 ? (*shdr_arr_64)[sh].sh_size : (*shdr_arr_32)[sh].sh_size;

			if(!sh_size) continue;
			const uint c_sh_size = sh_size / 4;

			fd.Write(wxString::Format("Start of section header %d (instructions count: %d)\n", sh, c_sh_size));

			for(uint i=0, c=0, v=0; i<c_sh_size; i++, c++, counter++)
			{
				if(c >= cores)
				{
					c = 0;
					v++;
				}

				prog_dial2.Update(0, counter, wxString::Format("Saving data to file: %d of %d", counter, length));

				if(v >= arr[c][sh].GetCount()) continue;

				fd.Write("	");
				fd.Write(arr[c][sh][v]);
			}

			fd.Write(wxString::Format("End of section header %d\n\n", sh));
		}
		
		ConLog.Write("CleanUp dump saving!");

		for(uint c=0; c<cores; ++c)
		{
			for(uint sh=0; sh<shdr_count; ++sh) arr[c][sh].Empty();
		}

		free(arr);

		//safe_delete(shdr_arr);
		safe_delete(l_elf32);
		safe_delete(l_elf64);

		prog_dial2.Close();
		fd.Close();

		wxMessageBox("Dumping done.", "rpcs3 message");

		Emu.Stop();

		ThreadBase::Exit();
		delete this;
	}
};

void DisAsmFrame::Dump(wxCommandEvent& WXUNUSED(event)) 
{
	wxFileDialog ctrl( this, L"Select output file...",
		wxEmptyString, "DumpOpcodes.txt", "*.txt", wxFD_SAVE);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	wxFile& f_elf = *new wxFile(Emu.m_path);
	ConLog.Write("path: %s", Emu.m_path);
	Elf_Ehdr ehdr;
	ehdr.Load(f_elf);
	f_elf.Close();
	if(!ehdr.CheckMagic())
	{
		ConLog.Error("Corrupted ELF!");
		return;
	}

	switch(ehdr.GetClass())
	{
	case CLASS_ELF64:
		ElfType64 = true;
		l_elf64 = new ELF64Loader(Emu.m_path);
		if(!l_elf64->LoadInfo())
		{
			safe_delete(l_elf64);
			return;
		}
		shdr_arr_64 = &l_elf64->shdr_arr;
		if(l_elf64->shdr_arr.GetCount() <= 0) return;
	break;

	case CLASS_ELF32:
		ElfType64 = false;
		l_elf32 = new ELF32Loader(Emu.m_path);
		if(!l_elf32->LoadInfo())
		{
			safe_delete(l_elf32);
			return;
		}
		shdr_arr_32 = &l_elf32->shdr_arr;
		if(l_elf32->shdr_arr.GetCount() <= 0) return;
	break;

	default: ConLog.Error("Corrupted ELF!"); return;
	}

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	const uint cores_count =
		(si.dwNumberOfProcessors < 1 || si.dwNumberOfProcessors > 8 ? 2 : si.dwNumberOfProcessors); 

	wxArrayLong max;
	max.Clear();

	u64 max_count = 0;

	if(ElfType64)
	{
		for(uint sh=0; sh<l_elf64->shdr_arr.GetCount(); ++sh)
		{
			max_count += l_elf64->shdr_arr[sh].sh_size / 4;
		}
	}
	else
	{
		for(uint sh=0; sh<l_elf32->shdr_arr.GetCount(); ++sh)
		{
			max_count += l_elf32->shdr_arr[sh].sh_size / 4;
		}
	}

	for(uint c=0; c<cores_count; ++c) max.Add(max_count / cores_count);
	for(uint c=0; c<max_count % cores_count; ++c) max[c]++;

	MTProgressDialog& prog_dial = *new MTProgressDialog(this, wxDefaultSize, "Dumping...", "Loading", max, cores_count);

	DumperThread* dump = new DumperThread[cores_count];
	wxArrayString** arr = new wxArrayString*[cores_count];

	bool* threads_done = new bool[cores_count];

	for(uint i=0; i<cores_count; ++i)
	{
		arr[i] = new wxArrayString[ElfType64 ? l_elf64->shdr_arr.GetCount() : l_elf32->shdr_arr.GetCount()];
		dump[i].Set(i, cores_count, &threads_done[i], prog_dial, arr);
		dump[i].Start(true);
	}

	for(uint i=0; i<cores_count; ++i) dump[i].DoStep();

	WaitDumperThread& wait_dump = 
		*new WaitDumperThread(threads_done, cores_count, ctrl.GetPath(), prog_dial, arr);
	wait_dump.Start(true);
}

void DisAsmFrame::Prev (wxCommandEvent& WXUNUSED(event)) { if(Emu.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES+1)); Resume(); } }
void DisAsmFrame::Next (wxCommandEvent& WXUNUSED(event)) { if(Emu.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES-1)); Resume(); } }
void DisAsmFrame::fPrev(wxCommandEvent& WXUNUSED(event)) { if(Emu.IsPaused()) { CPU.SetPc( CPU.PC - (4*LINES_OPCODES)*2); Resume(); } }
void DisAsmFrame::fNext(wxCommandEvent& WXUNUSED(event)) { if(Emu.IsPaused()) { Resume(); } }
void DisAsmFrame::SetPc(wxCommandEvent& WXUNUSED(event))
{
	if(!Emu.IsPaused()) return;

	wxDialog* diag = new wxDialog(this, wxID_ANY, "Set PC", wxDefaultPosition);

	wxBoxSizer* s_panel(new wxBoxSizer(wxVERTICAL));
	wxBoxSizer* s_b_panel(new wxBoxSizer(wxHORIZONTAL));
	wxTextCtrl* p_pc(new wxTextCtrl(diag, wxID_ANY));

	s_panel->Add(p_pc);
	s_panel->AddSpacer(8);
	s_panel->Add(s_b_panel);

	s_b_panel->Add(new wxButton(diag, wxID_OK), wxLEFT, 0, 5);
	s_b_panel->AddSpacer(5);
	s_b_panel->Add(new wxButton(diag, wxID_CANCEL), wxRIGHT, 0, 5);

	diag->SetSizerAndFit( s_panel );

	p_pc->SetLabel(wxString::Format("%llx", CPU.PC));

	if(diag->ShowModal() == wxID_OK)
	{
		sscanf(p_pc->GetLabel(), "%llx", &CPU.PC);
		Resume();
	}
}

void DisAsmFrame::MouseWheel(wxMouseEvent& event)
{
	if(!Emu.IsPaused())
	{
		event.Skip();
		return;
	}

	const int value = (event.m_wheelRotation / event.m_wheelDelta);

	if(event.ControlDown())
	{
		CPU.SetPc( CPU.PC - (((4*LINES_OPCODES)*2)*value) );
	}
	else
	{
		CPU.SetPc( CPU.PC - 4*(LINES_OPCODES + (value /** event.m_linesPerAction*/)) );
	}

	Emu.Resume();

	event.Skip();
}