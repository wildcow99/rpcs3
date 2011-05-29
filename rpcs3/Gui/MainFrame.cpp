#include "stdafx.h"
#include "MainFrame.h"

#include "Emu/System.h"


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_CLOSE(MainFrame::OnQuit)
END_EVENT_TABLE()

enum IDs
{
	id_boot_elf = 0x555,
	id_boot_self,
};

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _PRGNAME_ " " _PRGVER_)
{
	wxMenuBar& menubar(*new wxMenuBar());

	wxMenu& menu_boot(*new wxMenu());

	menubar.Append(&menu_boot, "Boot");

	menu_boot.Append(id_boot_elf, "Boot Elf...");
	menu_boot.Append(id_boot_self, "Boot Self...");

	SetMenuBar(&menubar);

	SetSize(wxSize(280, 180));

	ConLog.Init();

	Connect( id_boot_elf, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootElf) );
	Connect( id_boot_self, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootSelf) );
}

void MainFrame::BootElf(wxCommandEvent& event)
{
	wxFileDialog ctrl( this, L"Select ELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	ConLog.Write("Elf: booting...");

	System.SetElf(ctrl.GetPath());
	System.Run();

	ConLog.Write("Elf: boot done.");
}

void MainFrame::BootSelf(wxCommandEvent& event)
{
	wxFileDialog ctrl( this, L"Select SELF", wxEmptyString, wxEmptyString, "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	ConLog.Write("SELF: booting...");

	System.SetSelf(ctrl.GetPath());
	System.Run();

	ConLog.Write("SELF: boot done.");
}


void MainFrame::OnQuit(wxCloseEvent& event)
{
	Close();
}