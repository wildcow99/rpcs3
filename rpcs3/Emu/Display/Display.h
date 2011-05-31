#pragma once

#include <wx/glcanvas.h>
#include <GL/gl.h>
#include <GL/glu.h>

class GLCanvas: public wxGLCanvas
{
public:
    GLCanvas(wxWindow* parent) : wxGLCanvas(parent, wxID_ANY, 0)
	{
		Connect( wxEVT_PAINT, wxPaintEventHandler(GLCanvas::OnPaint) );
		Connect( wxEVT_SIZE, wxSizeEventHandler(GLCanvas::OnSize) );
	}

    void OnPaint(wxPaintEvent& WXUNUSED(event))
	{
		wxPaintDC dc(this);
		if(!GetContext()) return;

		SetCurrent();

		SwapBuffers();
	}

    void OnSize(wxSizeEvent& event)
	{
		wxGLCanvas::OnSize(event);

		static int w=0, h=0;
		GetClientSize(&w, &h);

		if (GetContext())
		{
			SetCurrent();
			glViewport(0, 0, (GLint)w, (GLint)h);
		}
	}
};

class Display : public wxFrame
{
	GLCanvas* canvas;

public:
	Display() : wxFrame(NULL, wxID_ANY, "Display")
	{
		canvas = new GLCanvas(this);

		canvas->SetBackgroundColour("Black");
		SetSize(640, 480);
	}
};