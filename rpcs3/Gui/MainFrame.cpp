#include "stdafx.h"
#include "MainFrame.h"
#include "Emu/ElfLoader.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_CLOSE(MainFrame::OnQuit)
END_EVENT_TABLE()

enum IDs
{
	id_boot_elf = 0x555,
};

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, _PRGNAME_ " " _PRGVER_)
{
	wxMenuBar& menubar(*new wxMenuBar());

	wxMenu& menu_boot(*new wxMenu());

	menubar.Append(&menu_boot, "Boot");

	menu_boot.Append(id_boot_elf, "Boot Elf...");

	SetMenuBar(&menubar);

	SetSize(wxSize(280, 180));

	ConLog.Init();

	Connect( id_boot_elf, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::BootElf) );
}

void MainFrame::BootElf(wxCommandEvent& event)
{
	wxFileDialog ctrl( this, L"Select boot.bin", wxEmptyString, wxEmptyString, "boot.bin",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(ctrl.ShowModal() == wxID_CANCEL) return;

	ConLog.Write("Elf: booting...");

	elf_loader.SetElf(ctrl.GetPath());
	elf_loader.Load();

	ConLog.Write("Elf: boot done.");
}

void MainFrame::OnQuit(wxCloseEvent& event)
{
	Close();
}