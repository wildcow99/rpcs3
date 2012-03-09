#pragma once
#include "Emu/GS/GSRender.h"

struct NullGSFrame : public wxFrame
{
	NullGSFrame() : wxFrame(NULL, wxID_ANY, "GSFrame[Null]")
	{
	}

	virtual void OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		Draw(dc);
	}

	virtual void OnSize(wxSizeEvent& event)
	{
		Draw();
	}

	void Draw()
	{
		wxClientDC dc(this);
		Draw(dc);
	}

	void Draw(wxDC& dc)
	{
		dc.DrawText("Null GS output", 0, 0);
	}

private:
	DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(NullGSFrame, wxFrame)
    EVT_PAINT(NullGSFrame::OnPaint)
	EVT_SIZE(NullGSFrame::OnSize)
END_EVENT_TABLE()

class NullGSRender : public GSRender
{
private:
	NullGSFrame* frame;

public:
	NullGSRender() : frame(NULL)
	{
	}

private:
	virtual void SetData(u8* pixels, const u32 width, const u32 height)
	{
	}

	virtual void Init(const u32 width, const u32 height)
	{
		if(frame) return;
		frame = new NullGSFrame();
		frame->SetSize(width, height);
		frame->Show();
		Draw();
	}

	virtual void Draw()
	{
		if(frame) frame->Draw();
	}

	virtual void Close()
	{
		if(frame) frame->Close();
		frame = NULL;
	}
};