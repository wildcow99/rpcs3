#include "stdafx.h"
#include "DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/System.h"
#include "Emu/ElfLoader.h"

DisAsmFrame::DisAsmFrame() : wxFrame(NULL, wxID_ANY, "DisAsm")
{
	exit = false;
	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );
	wxBoxSizer& s_b_panel( *new wxBoxSizer(wxHORIZONTAL) );

	m_disasm_list = new wxListView(this);

	wxButton& b_fprev	= *new wxButton(this, wxID_ANY, L"<<");
	wxButton& b_prev	= *new wxButton(this, wxID_ANY, L"<");
	wxButton& b_next	= *new wxButton(this, wxID_ANY, L">");
	wxButton& b_fnext	= *new wxButton(this, wxID_ANY, L">>");

	wxButton& b_dump	= *new wxButton(this, wxID_ANY, L"Dump code");

	s_b_panel.Add(&b_fprev);
	s_b_panel.Add(&b_prev);
	s_b_panel.AddSpacer(5);
	s_b_panel.Add(&b_next);
	s_b_panel.Add(&b_fnext);
	s_b_panel.AddSpacer(8);
	s_b_panel.Add(&b_dump);

	s_panel.Add(&s_b_panel);
	s_panel.Add(m_disasm_list);

	m_disasm_list->InsertColumn(0, "ASM");

	SetSizerAndFit( &s_panel );

	SetSize(50, 650);

	Connect( wxEVT_SIZE, wxSizeEventHandler(DisAsmFrame::OnResize) );
	wxTheApp->Connect(m_disasm_list->GetId(), wxEVT_MOUSEWHEEL, wxMouseEventHandler(DisAsmFrame::MouseWheel));

	Connect(b_prev.GetId(),  wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Prev));
	Connect(b_next.GetId(),  wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Next));
	Connect(b_fprev.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fPrev));
	Connect(b_fnext.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::fNext));

	Connect(b_dump.GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Dump));
}

void DisAsmFrame::OnResize(wxSizeEvent& event)
{
	const wxSize size(GetClientSize());
	m_disasm_list->SetSize( size.GetWidth(), size.GetHeight() - 25 );
	m_disasm_list->SetColumnWidth( 0, size.GetWidth() - 4 );
}

void DisAsmFrame::AddLine(const wxString line)
{
	static bool finished = false;
	if(finished && System.IsRunned())
	{
		m_disasm_list->DeleteAllItems();
		finished = false;
	}
	else if(m_disasm_list->GetItemCount() >= LINES_OPCODES || !System.IsRunned())
	{
		if(System.IsRunned()) System.Pause();
		finished = true;
		CPU.PrevPc();
		return;
	}

	m_disasm_list->InsertItem(m_disasm_list->GetItemCount(), line);
}

void DisAsmFrame::Resume()
{
	m_disasm_list->DeleteAllItems();
	System.Resume();
}

#include "Thread.h"

class MTProgressDialog : public wxDialog
{
	wxGauge** m_gauge;
	wxStaticText** m_msg;

	wxArrayInt m_maximum;
	uint m_cores;

	static const uint layout = 16;
	static const uint maxdial = 65536;
	wxArrayInt m_lastupdate;

public:
	MTProgressDialog(wxWindow* parent, wxSize size, wxString title, wxString msg, wxArrayInt maximum, const uint cores)
		: wxDialog(parent, wxID_ANY, title, wxDefaultPosition)
		, m_maximum(maximum)
		, m_cores(cores)
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		m_gauge = new wxGauge*[m_cores];
		m_msg = new wxStaticText*[m_cores];

		m_lastupdate.SetCount(cores);

		for(uint i=0; i<m_cores; ++i)
		{
			m_lastupdate[i] = -1;

			m_msg[i] = new wxStaticText(this, wxID_ANY, msg);
			sizer->Add(m_msg[i], 0, wxLEFT | wxTOP, layout);

			m_gauge[i] = new wxGauge(this, wxID_ANY, maxdial,
									  wxDefaultPosition, wxDefaultSize,
									  wxGA_HORIZONTAL );

			sizer->Add(m_gauge[i], 0, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, layout);
			m_gauge[i]->SetValue(0);

			sizer->AddSpacer(5);
		}

		SetSizerAndFit(sizer);
		if(size != wxDefaultSize)
		{
			SetSize(size);
		}
		else
		{
			wxSize ws;
			ws.x = 400;
			ws.y = GetSize().y + 8;
			SetSize(ws);
		}

		Show();
	}

	__forceinline void Update(const uint thread_id, const uint value, const wxString msg)
	{
		if(thread_id > m_cores) return;

		const int curupdate = (int)(((double)value/(double)m_maximum[thread_id])*1000);
		if(curupdate == m_lastupdate[thread_id]) return;
		m_lastupdate[thread_id] = curupdate;

		m_msg[thread_id]->SetLabel(msg);

		if(value >= (uint)m_maximum[thread_id]) return;
		m_gauge[thread_id]->SetValue(((double)value / (double)m_maximum[thread_id]) * maxdial);
	}
};

class DumperThread : public StepThread
{
	volatile uint id;
	DisAsmOpcodes* disasm;
	Decoder* decoder;
	volatile u64 pc;
	volatile bool* done;
	volatile u8 cores;
	volatile u64 length;
	MTProgressDialog* prog_dial;
	wxArrayString* arr;

public:
	DumperThread() : StepThread()
	{
	}

	void Set(uint _id, uint _length, u8 _cores, bool* _done, MTProgressDialog& _prog_dial, wxArrayString* _arr)
	{
		id = _id;
		pc = id * 4;
		length = _length;
		cores = _cores;
		done = _done;
		prog_dial = &_prog_dial;
		arr = _arr;

		*done = false;

		disasm = new DisAsmOpcodes(true);
		decoder = new Decoder(*disasm);
	}

	virtual void Step()
	{
		ConLog.Write("Start dump in thread %d!", (int)id);

		while(pc < length)
		{
			//ConLog.Write("%d thread: %d of %d", (int)id, (int)pc, (int)length);
			prog_dial->Update((int)id, (int)pc, wxString::Format("%d thread: %d of %d", (int)id + 1, (int)pc, (int)length));

			disasm->dump_pc = pc;
			decoder->DoCode(Memory.Read32(pc));
			
			arr[id].Add(disasm->last_opcode);

			pc += (cores - id) * 4; //goto next global block
			pc += id * 4; //goto my block
		}

		ConLog.Write("Finish dump in thread %d!", (int)id);

		*done = true;

		Cleanup();

		StepThread::Exit();
		ThreadAdv::Exit();
		delete this;
	}

	void Cleanup()
	{
		ConLog.Write("CleanUp dump thread (%d)!", (int)id);
		if(decoder)
		{
			safe_delete(decoder);
		}
	}
};

struct WaitDumperThread : public ThreadBase
{
	volatile bool* done;
	volatile u8 cores;
	volatile uint length;
	wxString patch;
	MTProgressDialog& prog_dial;
	wxArrayString* arr;

	WaitDumperThread(bool* _done, u8 _cores, uint _length, wxString _patch, MTProgressDialog& _prog_dial, wxArrayString* _arr) 
		: ThreadBase()
		, done(_done)
		, cores(_cores)
		, length(_length)
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
		prog_dial.Close();

		wxArrayInt max;
		max.Add(length);
		MTProgressDialog& prog_dial2 = 
			*new MTProgressDialog(NULL, wxDefaultSize, "Saving...", "Loading", max, 1);

		wxFile fd;
		fd.Open(patch, wxFile::write);

		for(uint i=0, c=0, v=0; i<length; i++, c++)
		{
			if(c >= cores)
			{
				c = 0;
				v++;
			}

			prog_dial2.Update(0, i, wxString::Format("Saving data to file: %d of %d", i, length));

			if(v >= arr[c].GetCount()) continue;

			fd.Write(arr[c][v]);
		}
		
		ConLog.Write("CleanUp dump saving!");

		for(uint i=0; i<cores; ++i) arr[i].Empty();

		prog_dial2.Close();
		fd.Close();
		Memory.Close();
		CPU.Reset();

		wxMessageBox("rpcs3 message", "Dumping done.");

		ThreadBase::Exit();
		delete this;
	}
};

void DisAsmFrame::Dump(wxCommandEvent& WXUNUSED(event)) 
{
	wxFileDialog ctrl( NULL, L"Select output file...",
		wxEmptyString, "DumpOpcodes.txt", "*.txt", wxFD_SAVE);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	if(!System.IsStoped()) System.Stop();

	Memory.Init();

	if(System.IsSlef)
	{
		elf_loader.LoadSelf(true);
	}
	else
	{
		elf_loader.LoadElf(true);
	}

	const uint dump_size = elf_loader.elf_size;
	if(dump_size == 0) return;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	const uint cores_count =
		(si.dwNumberOfProcessors < 1 || si.dwNumberOfProcessors > 8 ? 2 : si.dwNumberOfProcessors); 

	wxArrayInt max;
	if(cores_count > 1)
	{
		for(uint i=0; i<cores_count - 1; ++i)
		{
			max.Add(dump_size/cores_count);
		}

		const double d = (double)dump_size/(double)cores_count;
		const double i = dump_size/cores_count;

		if(d != i)
		{
			ConLog.Warning("Dump warning: %.3d != %.3d", d, i);
			max.Add((dump_size/cores_count) + (d - i) + 1);
		}
		else
		{
			max.Add(dump_size/cores_count);
		}
	}
	else
	{
		max.Add(dump_size);
	}

	MTProgressDialog& prog_dial = *new MTProgressDialog(this, wxDefaultSize, "Dumping...", "Loading", max, cores_count);

	DumperThread* dump = new DumperThread[cores_count];
	wxArrayString* arr = new wxArrayString[cores_count];

	bool* threads_done = new bool[cores_count];

	for(uint i=0; i<cores_count; ++i)
	{
		dump[i].Set(i, dump_size/cores_count, cores_count, &threads_done[i], prog_dial, arr);
		dump[i].Start(true);
	}

	for(uint i=0; i<cores_count; ++i) dump[i].DoStep();

	WaitDumperThread& wait_dump = 
		*new WaitDumperThread(threads_done, cores_count, (dump_size/cores_count)/4, ctrl.GetPath(), prog_dial, arr);
	wait_dump.Start(true);
}

void DisAsmFrame::Prev (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES+1)); Resume(); } }
void DisAsmFrame::Next (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES-1)); Resume(); } }
void DisAsmFrame::fPrev(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - (4*LINES_OPCODES)*2); Resume(); } }
void DisAsmFrame::fNext(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { Resume(); } }

void DisAsmFrame::MouseWheel(wxMouseEvent& event)
{
	if(!System.IsPaused())
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
		CPU.SetPc( CPU.PC - 4*(LINES_OPCODES + (value/* * event.m_linesPerAction*/)) );
	}

	System.Resume();

	event.Skip();
}