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
	id_sys_pause,
	id_sys_stop,
	id_config_emu,
};

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _PRGNAME_ " " _PRGVER_, 
	Ini.Gui.m_MainWindow.GetValue().position, Ini.Gui.m_MainWindow.GetValue().size)
{
	wxMenuBar& menubar(*new wxMenuBar());

	wxMenu& menu_boot(*new wxMenu());
	wxMenu& menu_sys(*new wxMenu());
	wxMenu& menu_conf(*new wxMenu());

	menubar.Append(&menu_boot, "Boot");
	menubar.Append(&menu_sys, "System");
	menubar.Append(&menu_conf, "Config");

	menu_boot.Append(id_boot_game, "Boot game");
	menu_boot.AppendSeparator();
	menu_boot.Append(id_boot_elf, "Boot Elf");
	menu_boot.Append(id_boot_self, "Boot Self");

	menu_sys.Append(id_sys_pause, "Pause");
	menu_sys.Append(id_sys_stop, "Stop");

	menu_conf.Append(id_config_emu, "Settings");

	SetMenuBar(&menubar);

	Connect( id_boot_game, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootGame) );
	Connect( id_boot_elf,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootElf) );
	Connect( id_boot_self, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootSelf) );

	Connect( id_sys_pause, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Pause) );
	Connect( id_sys_stop, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Stop) );

	Connect( id_config_emu, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::Config) );

	UpdateUI();

	(new CompilerELF(this))->Show();
}

void MainFrame::BootGame(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(Emu.IsRunned())
	{
		Emu.Pause();
		stoped = true;
	}

	wxDirDialog ctrl( this, L"Select game folder", wxEmptyString);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) Emu.Resume();
		return;
	}

	Emu.Stop();

	const wxString elf0  = ctrl.GetPath() + "\\PS3_GAME\\USRDIR\\BOOT.BIN";
	const wxString elf1  = ctrl.GetPath() + "\\USRDIR\\BOOT.BIN";
	const wxString elf2  = ctrl.GetPath() + "\\BOOT.BIN";
	const wxString self0 = ctrl.GetPath() + "\\PS3_GAME\\USRDIR\\EBOOT.BIN";
	const wxString self1 = ctrl.GetPath() + "\\USRDIR\\EBOOT.BIN";
	const wxString self2 = ctrl.GetPath() + "\\EBOOT.BIN";

	if(wxFile::Access(elf0, wxFile::read))
	{
		Emu.SetElf(elf0);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(elf1, wxFile::read))
	{
		Emu.SetElf(elf1);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(elf2, wxFile::read))
	{
		Emu.SetElf(elf2);
		ConLog.Write("Elf: booting...");
	}
	else if(wxFile::Access(self0, wxFile::read))
	{
		Emu.SetSelf(self0);
		ConLog.Warning("Self: booting...");
	}
	else if(wxFile::Access(self1, wxFile::read))
	{
		Emu.SetSelf(self1);
		ConLog.Warning("Self: booting...");
	}
	else if(wxFile::Access(self2, wxFile::read))
	{
		Emu.SetSelf(self2);
		ConLog.Warning("Self: booting...");
	}
	else
	{
		ConLog.Error("Not found ps3 game in selected folder! (%s)", ctrl.GetPath());
		return;
	}

	Emu.Run();

	ConLog.Write("Game: boot done.");
}

void MainFrame::BootElf(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(Emu.IsRunned())
	{
		Emu.Pause();
		stoped = true;
	}

	wxFileDialog ctrl( this, L"Select ELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) Emu.Resume();
		return;
	}

	ConLog.Write("Elf: booting...");

	Emu.Stop();

	Emu.SetElf(ctrl.GetPath());
	Emu.Run();

	ConLog.Write("Elf: boot done.");
}

void MainFrame::BootSelf(wxCommandEvent& WXUNUSED(event))
{
	bool stoped = false;

	if(Emu.IsRunned())
	{
		Emu.Pause();
		stoped = true;
	}

	wxFileDialog ctrl( this, L"Select SELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL)
	{
		if(stoped) Emu.Resume();
		return;
	}

	ConLog.Write("SELF: booting...");

	Emu.Stop();

	Emu.SetSelf(ctrl.GetPath());
	Emu.Run();

	ConLog.Write("SELF: boot done.");
}

void MainFrame::Pause(wxCommandEvent& WXUNUSED(event))
{
	if(Emu.IsPaused())
	{
		Emu.Resume();
	}
	else if(Emu.IsRunned())
	{
		Emu.Pause();
	}
}

void MainFrame::Stop(wxCommandEvent& WXUNUSED(event))
{
	Emu.Stop();
}

void MainFrame::Config(wxCommandEvent& WXUNUSED(event))
{
	//TODO

	bool paused = false;

	if(Emu.IsRunned())
	{
		Emu.Pause();
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

	if(paused) Emu.Resume();
}

void MainFrame::OnQuit(wxCloseEvent& event)
{
	Ini.Gui.m_MainWindow.SetValue(WindowInfo(GetSize(), GetPosition()));
	TheApp->Exit();
}

void MainFrame::UpdateUI()
{
	wxMenuBar& menubar( *GetMenuBar() );
	wxMenuItem& pause = *menubar.FindItem( id_sys_pause );
	wxMenuItem& stop  = *menubar.FindItem( id_sys_stop );

	if(Emu.IsRunned())
	{
		pause.SetText(_("Pause"));
		pause.Enable();
		stop.Enable();
	}
	else if(Emu.IsPaused())
	{
		pause.SetText(_("Resume"));
		pause.Enable();
		stop.Enable();
	}
	else
	{
		pause.Enable(false);
		stop.Enable(false);
	}
}