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
		//ConLog.Write("Resetting Game Info...");

		root = wxEmptyString;
		name = wxEmptyString;
		serial = wxEmptyString;
	}
};

extern GameInfo CurGameInfo;