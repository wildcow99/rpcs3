#include "stdafx.h"
#include "Display.h"

#ifdef USE_GS_FRAME
PFNGLGENBUFFERSARBPROC pglGenBuffersARB = 0;
PFNGLBINDBUFFERARBPROC pglBindBufferARB = 0;
PFNGLBUFFERDATAARBPROC pglBufferDataARB = 0;
PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB = 0;
PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB = 0;
PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB = 0;
PFNGLMAPBUFFERARBPROC pglMapBufferARB = 0;
PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB = 0;

#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB

class GSCanvas_GL: public wxGLCanvas
{
	//GLuint m_vboId;
	//GLuint m_pboId[2];
	GLuint m_pitch;
	GLuint m_textureId;
	u8* m_vertex_buf;
	u8* m_pixels;
	u64 m_pixels_count;
	wxString m_fps_string;
	wxRect m_rect;

	bool m_is_img;

	void Render();
	void RenderD();
	void RenderImg();

public:
    GSCanvas_GL(wxFrame* parent);
    virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSize(wxSizeEvent& event);
	void DrawString(wxPoint pos, wxColour colour, void* font, const wxString& str);
	void Draw();
	void Draw(wxImage& img);
	bool Map(u8** dst, GLenum access = GL_WRITE_ONLY_ARB);
	void Unmap();

	void InitPixels();

	void InitGL();

protected:
    DECLARE_EVENT_TABLE()
};
 
BEGIN_EVENT_TABLE(GSCanvas_GL, wxGLCanvas)
    EVT_PAINT(GSCanvas_GL::OnPaint)
	EVT_SIZE(GSCanvas_GL::OnSize)
END_EVENT_TABLE()
 
GSCanvas_GL::GSCanvas_GL(wxFrame *parent)
	: wxGLCanvas(parent, wxID_ANY, wxDefaultPosition)
	, m_textureId(0)
	, m_pixels(NULL)
	, m_pixels_count(0)
	, m_pitch(4)
	, m_fps_string("FPS: 0")
{
	SetCurrent();

    int argc = 1;
    char* argv[1] = { wxString((wxTheApp->argv)[0]).char_str() };

    glutInit(&argc, argv);
	InitGL();
	
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	if(!(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB &&
		glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB))
	{
		ConLog.Error("ERROR: Video card does NOT support GL_ARB_vertex_buffer_object!");
		Close();
		return;
	}

	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glGenBuffersARB(2, m_pboId);

	wxImage icon0;
	const wxString& icon0_path = CurGameInfo.root + "\\ICON0.PNG";
	ConLog.Error(icon0_path);
	if(!wxFileExists(icon0_path)) return;
	if(!icon0.LoadFile(icon0_path))
	{
		ConLog.Warning("%s load error!", icon0_path);
		return;
	}

	Draw(icon0);
	Render();
}
 
void GSCanvas_GL::InitGL()
{
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
}

void GSCanvas_GL::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    Render();
}

void GSCanvas_GL::OnSize(wxSizeEvent& event)
{
	SetCurrent();
	Render();

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GSCanvas_GL::Render()
{
	if(m_is_img) RenderImg();
	else RenderD();
}

void GSCanvas_GL::RenderD()
{
	//TODO
}

void GSCanvas_GL::RenderImg()
{
	if(m_textureId == 0) return;

    SetCurrent();
    wxPaintDC(this);

	glViewport(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
	gluOrtho2D(0, GetSize().GetWidth(), 0, GetSize().GetHeight());

    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, m_textureId);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	
	const double w_w = GetSize().GetWidth();
	const double w_h = GetSize().GetHeight();
	const double i_w = m_rect.GetWidth();
	const double i_h = m_rect.GetHeight();

	const double q_h = i_h / w_h;
	const double q_w = i_w / w_w;

	double q = q_h > q_w ? q_h : q_w;
	const u64 w = q > 1.0 ? i_w / q : i_w;
	const u64 h = q > 1.0 ? i_h / q : i_h;

	glTranslatef((w_w - w)/2 + m_rect.x, (w_h - h)/2 + m_rect.y, 0);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	{
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0, h);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(w, h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(w, 0);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0, 0);
	}
	glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

	DrawString(wxPoint(315,286), wxColour(1,1,0,1), GLUT_BITMAP_8_BY_13, m_fps_string);

	glPopMatrix();
    glFlush();
    SwapBuffers();
}

void GSCanvas_GL::Draw()
{
	if(!m_pixels || !m_rect.GetWidth() || !m_rect.GetHeight() || !m_pitch || !m_textureId) return;

	SetCurrent();

	static Timer fps_timer;
	static u64 frames = 0;

	const double elapsedTime = fps_timer.GetElapsedTimeInSec();

	if(elapsedTime < 1.0)
	{
		frames++;
	}
	else
	{
		m_fps_string = wxString::Format("FPS: %.2f", frames / elapsedTime);
		frames = 0;
		fps_timer.Start();
	}

	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_rect.GetWidth(), m_rect.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GSCanvas_GL::Draw(wxImage& img)
{
	SetCurrent();

	m_is_img = true;

	m_rect.width = img.GetWidth();
	m_rect.height = img.GetHeight();
	m_rect.x = 0;
	m_rect.y = 0;
	m_pitch = 4;

	InitPixels();

	u8* m;
	if(Map(&m))
	{
		for(int y=0; y<img.GetHeight(); y++)
		{
			for(int x=0; x<img.GetWidth(); x++)
			{
				m[0] = img.GetRed(x, y);
				m[1] = img.GetGreen(x, y);
				m[2] = img.GetBlue(x, y);
				m[3] = img.GetAlpha(x, y);
				m += 4;
			}
		}

		Unmap();
	}

	Draw();
}

void GSCanvas_GL::InitPixels()
{
	const u64 pixels_count = m_rect.GetWidth() * m_rect.GetHeight() * m_pitch;

	if(m_pixels_count != pixels_count)
	{
		/*
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_pboId[0]);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, pixels_count, 0, GL_STREAM_DRAW_ARB);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_pboId[1]);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, pixels_count, 0, GL_STREAM_DRAW_ARB);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		*/
	
		m_pixels_count = pixels_count;
		if(m_pixels) free(m_pixels);
		m_pixels = new u8[pixels_count];
		memset(m_pixels, 0, pixels_count);
	}
}

bool GSCanvas_GL::Map(u8** dst, GLenum access)
{
	*dst = m_pixels;
	//*dst = (u8*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, access);
	return dst != NULL;
}

void GSCanvas_GL::Unmap()
{
	//glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
}

void GSCanvas_GL::DrawString(wxPoint pos, wxColour colour, void* font, const wxString& str)
{
	glPushMatrix();
	glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); 
    glLoadIdentity();
    gluOrtho2D(0, 400, 0, 300);

	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);

	float f_colour[4];
	f_colour[0] = colour.Red();
	f_colour[1] = colour.Green();
	f_colour[2] = colour.Blue();
	f_colour[3] = colour.Alpha();

    glColor4fv(f_colour);
    glRasterPos2i(pos.x, pos.y);

	for(uint i=0; i<str.Length(); ++i)
    {
        glutBitmapCharacter(font, str[i]);
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

GSFrame_GL::GSFrame_GL(wxWindow* parent) : wxFrame(parent, wxID_ANY, "GSFrame[GL]")
{
    new GSCanvas_GL(this);
	SetSize(500,500);
    Show();
}
#endif