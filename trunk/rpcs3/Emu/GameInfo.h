#pragma once

struct GameInfo
{
	wxString root;
	wxString name;
	wxString serial;

	GameInfo()
	{
	}

	void Reset()
	{
		root.Clear();
		name.Clear();
		serial.Clear();
	}
};

extern GameInfo CurGameInfo;