#include "stdafx.h"
#include "DisAsmFrame.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Emu/System.h"
#include "Emu/ElfLoader.h"

DisAsmFrame::DisAsmFrame() : wxFrame(NULL, wxID_ANY, "DisAsm")
{
	m_PC = 0;

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

	Connect(b_prev.GetId(),	 wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DisAsmFrame::Prev));
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

void DisAsmFrame::AddLine(wxString line)
{
	static int cur_item_add = 0;
	if(++cur_item_add >= LINES_OPCODES)
	{
		System.Pause();
		cur_item_add = 0;
	}

	int item_count = m_disasm_list->GetItemCount();
	if(item_count >= LINES_OPCODES)
	{
		for(uint i=0; i<item_count-LINES_OPCODES; ++i) m_disasm_list->DeleteItem(0);
		item_count = m_disasm_list->GetItemCount();
	}

	m_disasm_list->InsertItem(item_count, line);
}


#ifdef MTHREAD_DUMP

#include "Thread.h"

class DumpThread : public ThreadBase
{
	void* m_hThread;
	bool m_exit;
	wxMutex m_mtx;
	wxSemaphore m_sem;
	Decoder* decoder;

public:
	DumpThread()
	{
	}

	void Create(wxString patch)
	{
		decoder = new Decoder(*new DisAsmOpcodes(true, patch));

		m_hThread = ::CreateThread
		(
			NULL,
			0,
			DumpThread::ThreadStart,
			(LPVOID)this,
			0,
			NULL
		);
	}

	~DumpThread()
	{
		delete decoder;
		ThreadBase::DeleteThread(m_hThread);
	}

	static DWORD __stdcall ThreadStart(void* thread)
	{
		return (DWORD)((DumpThread*)thread)->Entry();
	}

	virtual void Do()
	{
		m_sem.Post();
	}

	virtual void* Entry()
	{
		while(!m_exit)
		{
			m_sem.Wait();
			if(m_exit) break;
			m_mtx.Lock();

			decoder->DoCode(Memory.Read32(CPU.pc));

			m_mtx.Unlock();
		}

		return NULL;
	}
};

#endif

void DisAsmFrame::Dump(wxCommandEvent& WXUNUSED(event)) 
{
	wxFileDialog ctrl( NULL, L"Select output file...",
		wxEmptyString, "DumpOpcodes.txt", "*.txt", wxFD_SAVE);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	if(!System.IsStoped()) System.Stop();
	const uint dump_size = elf_loader.elf_size;

	wxProgressDialog prog_dial
	(
		"Dumping...",
		"Loading...",
		dump_size,
		NULL,
		wxPD_APP_MODAL		|
		wxPD_ELAPSED_TIME	|
		wxPD_ESTIMATED_TIME |
		wxPD_REMAINING_TIME |
		wxPD_CAN_ABORT		|
		wxPD_SMOOTH
	);

	Memory.Init();

	if(System.IsSlef)
	{
		elf_loader.LoadSelf(true);
	}
	else
	{
		elf_loader.LoadElf(true);
	}

#ifdef MTHREAD_DUMP
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	if(si.dwNumberOfProcessors < 1) si.dwNumberOfProcessors = 1;

	DumpThread* dump = new DumpThread[si.dwNumberOfProcessors];

	for(uint i=0; i<si.dwNumberOfProcessors; ++i)
	{
		dump[i].Create(ctrl.GetPath() + wxString::Format("%d", i));
	}
	uint a=0;
#else
	Decoder* decoder = new Decoder(*new DisAsmOpcodes(true, ctrl.GetPath()));
#endif

	while(CPU.PC < dump_size && prog_dial.Update(CPU.PC, wxString::Format("%d of %d", CPU.PC/4, dump_size/4)))
	{
#ifdef MTHREAD_DUMP
		dump[a].Do();
		if(++a >= si.dwNumberOfProcessors) a = 0;
#else
		decoder->DoCode(Memory.Read32(CPU.PC));
#endif
		CPU.NextPc();
	}

#ifdef MTHREAD_DUMP
	delete dump;
#else
	delete decoder;
#endif

	Memory.Close();
	CPU.Reset();

	prog_dial.Destroy();
	wxMessageBox("rpcs3 message", "Dumping done.");
}

void DisAsmFrame::Prev (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES+1)); System.Resume(); } }
void DisAsmFrame::Next (wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - 4*(LINES_OPCODES-1)); System.Resume(); } }
void DisAsmFrame::fPrev(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { CPU.SetPc( CPU.PC - (4*LINES_OPCODES)*2); System.Resume(); } }
void DisAsmFrame::fNext(wxCommandEvent& WXUNUSED(event)) { if(System.IsPaused()) { System.Resume(); } }