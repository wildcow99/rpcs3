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
	wxString m_adv_string;
	wxRect m_rect;

	wxPoint adv_pos;
	wxColour adv_colour;
	void* adv_font;

	bool m_is_img;

public:
	void Render();
	void RenderD();
	void RenderImg();

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

	void DrawAdvString(wxPoint pos, wxColour colour, void* font, const wxString& str)
	{
		adv_pos = pos;
		adv_colour = colour;
		adv_font = font;
		m_adv_string = str;
	}

protected:
    DECLARE_EVENT_TABLE()
};
 
BEGIN_EVENT_TABLE(GSCanvas_GL, wxGLCanvas)
    EVT_PAINT(GSCanvas_GL::OnPaint)
	EVT_SIZE(GSCanvas_GL::OnSize)
END_EVENT_TABLE()

class OGLTexture
{
	u32 m_textureId;

public:
	OGLTexture(u32 textureId) : m_textureId(textureId)
	{
	}

	OGLTexture() : m_textureId(0)
	{
	}

	~OGLTexture()
	{
		Destroy();
	}

	bool Init(bool linear)
	{
		if(m_textureId) return false;
		glGenTextures(1, &m_textureId);
		if(!Bind()) return false;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
		UnBind();
		return true;
	}

	bool SetData(void* pixels, u32 type, u32 width, u32 height)
	{
		if(!Bind()) return false;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, type, pixels);
		return true;
	}

	bool Bind()
	{
		if(!m_textureId) return false;
		glBindTexture(GL_TEXTURE_2D, m_textureId);
		return true;
	}

	void UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	bool Destroy()
	{
		if(!m_textureId) return false;
		glDeleteTextures(1, &m_textureId);
		m_textureId = 0;
		return true;
	}
};

class OGLRender
{
	u8* m_pixels;
	u32 m_pitch;

public:
	OGLRender()
	{
	}

	bool Draw(OGLTexture& texture, u32 width, u32 height)
	{
		if(!texture.Bind()) return false;
	}

	void Draw()
	{
	}

	void Rect(float x1, float y1, float x2, float y2)
	{
		glRectf(x1, y1, x2, y2);
	}
};
 
GSCanvas_GL::GSCanvas_GL(wxFrame *parent)
	: wxGLCanvas(parent, wxID_ANY, wxDefaultPosition)
	, m_textureId(0)
	, m_pixels(NULL)
	, m_pixels_count(0)
	, m_pitch(4)
	, m_fps_string("FPS: 0")
	, m_adv_string(wxEmptyString)
	, m_is_img(false)
{
	SetCurrent();

    int argc = 1;
    char* argv[1] = { wxString((wxTheApp->argv)[0]).char_str() };

    glutInit(&argc, argv);
	InitGL();

	glOrtho(0, 500, 500, 0, -1.0, 1.0);

	return;
	
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

	const u32 w = GetSize().GetWidth();
	const u32 h = GetSize().GetHeight();
	glViewport(0, 0, w, h);
	glOrtho(0, w, h, 0, -1.0, 1.0);

	//glGenBuffersARB(2, m_pboId);

	/*
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
	*/
}
 
void GSCanvas_GL::InitGL()
{
	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0, rwidth * 8, height * 8, 0, -1.0, 1.0);

/*
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
*/
}

void GSCanvas_GL::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    Render();
}

void GSCanvas_GL::OnSize(wxSizeEvent& event)
{
/*
	SetCurrent();
	const u32 w = GetSize().GetWidth();
	const u32 h = GetSize().GetHeight();
	glViewport(0, 0, w, h);
	glOrtho(0, w, h, 0, -1.0, 1.0);
	//Render();

    //glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
*/
}

void GSCanvas_GL::Render()
{
	if(m_is_img) RenderImg();
	else RenderD();
}

void GSCanvas_GL::RenderD()
{
	//if(!m_adv_string.Length()) return;
	SetCurrent();
    wxPaintDC(this);

	glViewport(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
	//glClear(GL_COLOR_BUFFER_BIT);
	DrawString(adv_pos, adv_colour, adv_font, m_adv_string);
	//DrawString(wxPoint(3,5), wxColour(1,1,0,1), GLUT_BITMAP_8_BY_13, m_fps_string);

    SwapBuffers();
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
	SetCurrent();

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

/*
	glPushMatrix();
	glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); 
    glLoadIdentity();
    gluOrtho2D(400, 300, 0, 0);

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
*/
}

GSFrame_GL::GSFrame_GL(wxWindow* parent) : wxFrame(parent, wxID_ANY, "GSFrame[GL]")
{
	SetSize(500,500);
	data = new GSCanvas_GL(this);
    Show();
}

void GSFrame_GL::DrawString(u32 x, u32 y, u32 z, u64 string_addr)
{
	ConLog.Warning("Draw string[x: %d, y: %d, z: %d, string: %s]", x, y, z, Memory.ReadString(string_addr));
	GSCanvas_GL& c = (*(GSCanvas_GL*)data);

	c.DrawAdvString
		(wxPoint(x, y), wxColour("White"), GLUT_BITMAP_8_BY_13, Memory.ReadString(string_addr));
}

void GSFrame_GL::Flip()
{
	(*(GSCanvas_GL*)data).SetCurrent();
	glRectd(1, 0, 0, 0);
	//(*(GSCanvas_GL*)data).Render();
}

#endif