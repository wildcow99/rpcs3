#include "stdafx.h"
#include "GameViewer.h"
#include "Loader/PSF.h"

GameViewer::GameViewer(wxWindow* parent) : wxPanel(parent)
{
	wxBoxSizer& s_panel( *new wxBoxSizer(wxVERTICAL) );

	m_game_list = new wxListView(this);
	s_panel.Add(m_game_list);

	SetSizerAndFit( &s_panel );

	m_game_list->InsertColumn(0, "Name");
	m_game_list->InsertColumn(1, "Serial");
	m_game_list->InsertColumn(2, "FW");

	m_path = wxGetCwd(); //TODO

	Connect(m_game_list->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(GameViewer::DClick));

	Refresh();
}

GameViewer::~GameViewer()
{
	m_game_list->Destroy();
}

void GameViewer::DoResize(wxSize size)
{
	SetSize(size);
	m_game_list->SetSize(size);
}

void GameViewer::LoadGames()
{
	if(!wxDirExists(m_path)) return;

	m_games.Clear();
	wxDir dir(m_path);
	if(!dir.HasSubDirs()) return;

	wxString buf;
	if(!dir.GetFirst(&buf)) return;
	if(wxDirExists(buf)) m_games.Add(buf);

	for(;;)
	{
		if(!dir.GetNext(&buf)) break;
		if(wxDirExists(buf)) m_games.Add(buf);
	}

	//ConLog.Write("path: %s", m_path);
	//ConLog.Write("folders count: %d", m_games.GetCount());
}

void GameViewer::LoadPSF()
{
	m_game_data.Clear();
	for(uint i=0; i<m_games.GetCount(); ++i)
	{
		const wxString& path = m_games[i] + "\\" + "PARAM.SFO";
		if(!wxFileExists(path)) continue;
		PSFLoader psf(path);
		if(!psf.Load(false)) continue;
		psf.m_info.root = m_games[i];
		m_game_data.Add(new GameInfo(psf.m_info));
	}
}

void GameViewer::ShowData()
{
	for(uint i=0; i<m_game_data.GetCount(); ++i)
	{
		GameInfo& game = m_game_data[i];
		const u32 index = m_game_list->GetItemCount();
		m_game_list->InsertItem(index, game.name);
		m_game_list->SetItem(index, 1, game.serial);
		m_game_list->SetItem(index, 2, game.fw);
	}
}

void GameViewer::Refresh()
{
	LoadGames();
	LoadPSF();
	ShowData();
}

void GameViewer::DClick(wxListEvent& event)
{
	long i = m_game_list->GetFirstSelected();
	if(i < 0) return;

	const wxString& path = m_game_data[i].root + "\\" + "USRDIR" + "\\" + "BOOT.BIN";
	if(!wxFileExists(path))
	{
		ConLog.Error("Boot error: elf not found! [%s]", path);
		return;
	}

	Emu.Stop();
	Emu.SetElf(path);
	Emu.Run();
}