#pragma once

//#define USE_GS_FRAME
#ifdef USE_GS_FRAME

#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPCThread.h"
#include "Ini.h"
#include "wx/glcanvas.h"
#include "GL/gl.h"
#include "GL/glut.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/glext.h"
#include "Utilites/Timer.h"

#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "gl.lib")

class GSFrame_GL : public wxFrame
{
	void* data;

public:
	GSFrame_GL(wxWindow* parent);
	void DrawString(u32 x, u32 y, u32 z, u64 string_addr);
	void Flip();
};

#endif