#include "stdafx.h"
#include "MainFrame.h"

#include "Emu/System.h"
#include "Ini.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_CLOSE(MainFrame::OnQuit)
END_EVENT_TABLE()

enum IDs
{
	id_boot_elf = 0x555,
	id_boot_self,
	id_boot_game,
	id_config_emu,
};

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _PRGNAME_ " " _PRGVER_)
{
	wxMenuBar& menubar(*new wxMenuBar());

	wxMenu& menu_boot(*new wxMenu());
	wxMenu& menu_conf(*new wxMenu());

	menubar.Append(&menu_boot, "Boot");
	menubar.Append(&menu_conf, "Config");

	menu_boot.Append(id_boot_game, "Boot game");
	menu_boot.AppendSeparator();
	menu_boot.Append(id_boot_elf, "Boot Elf");
	menu_boot.Append(id_boot_self, "Boot Self");

	menu_conf.Append(id_config_emu, "Settings");

	SetMenuBar(&menubar);

	SetSize(wxSize(280, 180));

	ConLog.Init();

	Connect( id_boot_game, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootGame) );
	Connect( id_boot_elf,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootElf) );
	Connect( id_boot_self, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootSelf) );

	Connect( id_config_emu, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Config) );
}

void MainFrame::BootGame(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(System.IsRunned())
	{
		System.Pause();
		stoped = true;
	}

	wxDirDialog ctrl( this, L"Select game folder", wxEmptyString);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) System.Resume();
		return;
	}

	ConLog.Write("Game: booting...");

	System.SetElf(ctrl.GetPath() + "\\PS3_GAME\\USRDIR\\BOOT.BIN");
	System.Run();

	ConLog.Write("Game: boot done.");
}

void MainFrame::BootElf(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(System.IsRunned())
	{
		System.Pause();
		stoped = true;
	}

	wxFileDialog ctrl( this, L"Select ELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) System.Resume();
		return;
	}

	ConLog.Write("Elf: booting...");

	System.SetElf(ctrl.GetPath());
	System.Run();

	ConLog.Write("Elf: boot done.");
}

void MainFrame::BootSelf(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(System.IsRunned())
	{
		System.Pause();
		stoped = true;
	}

	wxFileDialog ctrl( this, L"Select SELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) System.Resume();
		return;
	}

	ConLog.Write("SELF: booting...");

	System.SetSelf(ctrl.GetPath());
	System.Run();

	ConLog.Write("SELF: boot done.");
}

void MainFrame::Config(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(System.IsRunned())
	{
		System.Pause();
		stoped = true;
	}

	wxDialog* diag = new wxDialog(this, wxID_ANY, "Settings", wxDefaultPosition);

	wxBoxSizer* s_panel(new wxBoxSizer(wxVERTICAL));
	wxStaticBoxSizer* s_round( new wxStaticBoxSizer( wxVERTICAL, diag, _("Decoder") ) );

	wxComboBox* com_box = new wxComboBox(diag, wxID_ANY);

	com_box->Append("DisAsm");
	com_box->Append("Interpreter");

	com_box->SetSelection(ini.Load("DecoderMode", 1));

	s_round->Add(com_box);

	wxBoxSizer* s_b_panel(new wxBoxSizer(wxHORIZONTAL));

	s_b_panel->Add(new wxButton(diag, wxID_OK));
	s_b_panel->AddSpacer(5);
	s_b_panel->Add(new wxButton(diag, wxID_CANCEL));

	s_panel->Add(s_round);
	s_panel->AddSpacer(8);
	s_panel->Add(s_b_panel, wxRIGHT);

	diag->SetSizerAndFit( s_panel );
	
	if(diag->ShowModal() == wxID_OK)
	{
		ini.Save("DecoderMode", com_box->GetSelection());
	}

	delete diag;

	if(stoped) System.Resume();
}

void MainFrame::OnQuit(wxCloseEvent& event)
{
	System.Stop();

	Close();
}