#pragma once

class DisAsmFrame : public wxFrame
{
	static const uint LINES_OPCODES = 40;

	wxListView* m_disasm_list;
	uint m_PC;

	virtual void OnResize(wxSizeEvent& event);

	virtual void Prev (wxCommandEvent& event);
	virtual void Next (wxCommandEvent& event);
	virtual void fPrev(wxCommandEvent& event);
	virtual void fNext(wxCommandEvent& event);

public:
	DisAsmFrame();
	virtual void AddLine(wxString line);
};