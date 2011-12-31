#pragma once

class DbgConsole : public FrameBase
{
	wxTextCtrl* m_console;
	wxTextAttr* m_color_white;
	wxTextAttr* m_color_red;

public:
	DbgConsole();
	void Write(int ch, const wxString& text);
	void Clear();
};