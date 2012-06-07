#pragma once
#include "Emu/GS/GCM.h"

static wxSize AspectRatio(wxSize rs, const wxSize as)
{
	const double aq = (double)as.x / as.y;
	const double rq = (double)rs.x / rs.y;
	const double q = aq / rq;

	if(q > 1.0)
	{
		rs.y /= q;
	}
	else if(q < 1.0)
	{
		rs.x *= q;
	}

	return rs;
}

class GSFrame : public wxFrame
{
protected:
	//wxSize m_size;

	GSFrame(wxWindow* parent, const wxString& title) : wxFrame(parent, wxID_ANY, title)
	{
		SetMinSize(wxSize(720, 576));
		SetSize(720, 576);
		//Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(GSFrame::OnLeftDclick));
		wxGetApp().Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(GSFrame::OnKeyDown), (wxObject*)0, this);
	}

	virtual void SetViewport(wxPoint pos, wxSize size) {}
	virtual void OnPaint(wxPaintEvent& event) { wxPaintDC(this); }
	/*
	virtual void OnSize(wxSizeEvent&)
	{
		const wxSize client = GetClientSize();
		const wxSize viewport = AspectRatio(client, m_size);

		const int x = (client.GetX() - viewport.GetX()) / 2;
		const int y = (client.GetY() - viewport.GetY()) / 2;

		SetViewport(wxPoint(x, y), viewport);
	}
	*/
	void OnLeftDclick(wxMouseEvent&)
	{
		OnFullScreen();
	}

	void OnKeyDown(wxKeyEvent& event)
	{
		switch(event.GetKeyCode())
		{
		case WXK_RETURN: if(event.ControlDown()) { OnFullScreen(); return; } break;
		case WXK_ESCAPE: if(IsFullScreen()) { ShowFullScreen(false); return; } break;
		}
		event.Skip();
	}

	void OnFullScreen()
	{
		ShowFullScreen(!IsFullScreen());
	}

public:
	/*
	void SetSize(int width, int height)
	{
		m_size.SetWidth(width);
		m_size.SetHeight(height);
		//wxFrame::SetSize(width, height);
		OnSize(wxSizeEvent());
	}
	*/

private:
	DECLARE_EVENT_TABLE();
};

struct GSRender
{
	u32 m_ioAddress, m_ctrlAddress;
	CellGcmControl* m_ctrl;
	int m_flip_status;

	GSRender()
		: m_ctrl(NULL)
		, m_flip_status(-1)
	{
	}

	virtual void Init(const u32 ioAddress, const u32 ctrlAddress)=0;
	virtual void Draw()=0;
	virtual void Close()=0;

	u32 GetAddress(u32 offset, u8 location)
	{
		switch(location)
		{
		case CELL_GCM_LOCATION_LOCAL: return m_ioAddress + offset;
		case CELL_GCM_LOCATION_MAIN: return offset;
		}
		return 0;
	}
};