#pragma once

#include "Emu/Memory/Memory.h"
#include "Emu/Cell/PPCThread.h"
#include "Ini.h"
#include "wx/glcanvas.h"
#include "GL/gl.h"
#include "GL/glut.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/glext.h"
#pragma comment(lib, "glut32.lib")
#include "Utilites/Timer.h"

class GSFrame_GL : public wxFrame
{
public:
	GSFrame_GL(wxWindow* parent);
};