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

	~DbgPacket()
	{
		m_text.Clear();
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
	DbgConsole();
	~DbgConsole();
	void Write(int ch, const wxString& text);
	void Clear();
	virtual void Step();

private:
	void OnQuit(wxCloseEvent& event);
	DECLARE_EVENT_TABLE();
};