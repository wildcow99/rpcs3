#pragma once
#include <wx/listctrl.h>
#include <Utilites/Array.h>
#include <Emu/GameInfo.h>

class GameViewer : public wxPanel
{
	wxListView* m_game_list;
	wxString m_path;
	wxArrayString m_games;
	ArrayF<GameInfo> m_game_data;

public:
	GameViewer(wxWindow* parent);
	~GameViewer();

	void DoResize(wxSize size);

	void LoadGames();
	void LoadPSF();
	void ShowData();

	void Refresh();

private:
	virtual void DClick(wxListEvent& event);
};