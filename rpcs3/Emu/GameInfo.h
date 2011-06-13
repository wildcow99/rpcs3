#pragma once

struct GameInfo
{
	wxString root;
	wxString name;
	wxString serial;

	GameInfo()
	{
		Reset();
	}

	void Reset()
	{
		root = wxEmptyString;
		name = wxEmptyString;
		serial = wxEmptyString;
	}
};

extern GameInfo CurGameInfo;