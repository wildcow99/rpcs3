#pragma once

class MainFrame : public wxFrame
{
public:
	MainFrame();

	virtual void OnQuit(wxCloseEvent& event);
	virtual void BootElf(wxCommandEvent& event);
	virtual void BootSelf(wxCommandEvent& event);

private:
	DECLARE_EVENT_TABLE()
};