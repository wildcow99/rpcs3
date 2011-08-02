#include "stdafx.h"
#include "MainFrame.h"
#include "CompilerELF.h"

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

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _PRGNAME_ " " _PRGVER_, 
	Ini.Gui.m_MainWindow.GetValue().position, Ini.Gui.m_MainWindow.GetValue().size)
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

	Connect( id_boot_game, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootGame) );
	Connect( id_boot_elf,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootElf) );
	Connect( id_boot_self, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootSelf) );

	Connect( id_config_emu, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Config) );

	(new CompilerELF(this))->Show();
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

	System.Stop();

	const wxString elf0  = ctrl.GetPath() + "\\PS3_GAME\\USRDIR\\BOOT.BIN";
	const wxString elf1  = ctrl.GetPath() + "\\USRDIR\\BOOT.BIN";
	const wxString elf2  = ctrl.GetPath() + "\\BOOT.BIN";
	const wxString self0 = ctrl.GetPath() + "\\PS3_GAME\\USRDIR\\EBOOT.BIN";
	const wxString self1 = ctrl.GetPath() + "\\USRDIR\\EBOOT.BIN";
	const wxString self2 = ctrl.GetPath() + "\\EBOOT.BIN";

	if(wxFile::Access(elf0, wxFile::read))
	{
		System.SetElf(elf0);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(elf1, wxFile::read))
	{
		System.SetElf(elf1);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(elf2, wxFile::read))
	{
		System.SetElf(elf2);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(self0, wxFile::read))
	{
		System.SetSelf(self0);
		ConLog.Warning("Self: booting...");
	}
	else if(wxFile::Access(self1, wxFile::read))
	{
		System.SetSelf(self1);
		ConLog.Warning("Self: booting...");
	}
	else if(wxFile::Access(self2, wxFile::read))
	{
		System.SetSelf(self2);
		ConLog.Warning("Self: booting...");
	}
	else
	{
		ConLog.Error("Not found ps3 game in selected folder! (%s)", ctrl.GetPath());
		return;
	}

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

	System.Stop();

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

	System.Stop();

	System.SetSelf(ctrl.GetPath());
	System.Run();

	ConLog.Write("SELF: boot done.");
}

void MainFrame::Config(wxCommandEvent& WXUNUSED(event))
{
	//TODO

	bool paused = false;

	if(System.IsRunned())
	{
		System.Pause();
		paused = true;
	}

	wxDialog* diag = new wxDialog(this, wxID_ANY, "Settings", wxDefaultPosition);

	wxBoxSizer* s_panel(new wxBoxSizer(wxVERTICAL));

	wxStaticBoxSizer* s_round_decoder( new wxStaticBoxSizer( wxVERTICAL, diag, _("Decoder") ) );

	wxStaticBoxSizer* s_round_video( new wxStaticBoxSizer( wxHORIZONTAL, diag, _("Video") ) );
	wxStaticBoxSizer* s_round_video_render( new wxStaticBoxSizer( wxVERTICAL, diag, _("Render") ) );

	wxComboBox* cbox_decoder = new wxComboBox(diag, wxID_ANY);
	wxComboBox* cbox_video_render = new wxComboBox(diag, wxID_ANY);

	cbox_decoder->Append("DisAsm");
	cbox_decoder->Append("Interpreter");

	cbox_video_render->Append("Software");
	cbox_video_render->Append("OGL");

	cbox_decoder->SetSelection(Ini.Emu.m_DecoderMode.GetValue());
	cbox_video_render->SetSelection(Ini.Emu.m_RenderMode.GetValue());

	s_round_decoder->Add(cbox_decoder);

	s_round_video_render->Add(cbox_video_render);
	s_round_video->Add(s_round_video_render);

	wxBoxSizer* s_b_panel(new wxBoxSizer(wxHORIZONTAL));

	s_b_panel->Add(new wxButton(diag, wxID_OK));
	s_b_panel->AddSpacer(5);
	s_b_panel->Add(new wxButton(diag, wxID_CANCEL));

	s_panel->Add(s_round_decoder);
	s_panel->AddSpacer(5);
	s_panel->Add(s_round_video);
	s_panel->AddSpacer(8);
	s_panel->Add(s_b_panel, wxRIGHT);

	diag->SetSizerAndFit( s_panel );
	
	if(diag->ShowModal() == wxID_OK)
	{
		Ini.Emu.m_DecoderMode.SetValue(cbox_decoder->GetSelection());
		Ini.Emu.m_RenderMode.SetValue(cbox_video_render->GetSelection());
	}

	delete diag;

	if(paused) System.Resume();
}

void MainFrame::OnQuit(wxCloseEvent& event)
{
	Ini.Gui.m_MainWindow.SetValue(WindowInfo(GetSize(), GetPosition()));
	TheApp->Exit();
}