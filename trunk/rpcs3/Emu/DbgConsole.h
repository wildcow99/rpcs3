#pragma once

struct DbgPacket
{
	int m_ch;
	wxString m_text;

	DbgPacket(int ch, const wxString& text)
		: m_ch(ch)
		, m_text(text)
	{
	}
};

class DbgConsole
	: public FrameBase
	, public StepThread
{
	wxTextCtrl* m_console;
	wxTextAttr* m_color_white;
	wxTextAttr* m_color_red;
	Array<DbgPacket> m_packets;

public:
	bool hided;

	DbgConsole();
	~DbgConsole();
	void Write(int ch, const wxString& text);
	void Clear();
	virtual void Step();
	bool Close(bool force = false);
};