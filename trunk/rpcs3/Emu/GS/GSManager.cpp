#include "stdafx.h"
#include "GSManager.h"
#include "Null/NullGSRender.h"

//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "gl.lib")

GSManager::GSManager() : m_render(NULL)
{
}

void GSManager::Init()
{
	if(m_render) return;
	switch(Ini.GSRenderMode.GetValue())
	{
	default:
	case 0: m_render = new NullGSRender(); break;
	}
	m_render = new NullGSRender();
	m_render->Init(GetInfo().outresolution.width, GetInfo().outresolution.height);
}

void GSManager::Close()
{
	if(m_render) m_render->Close();
	m_render = NULL;
}

u8 GSManager::GetState()
{
	return CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED;
}

u8 GSManager::GetColorSpace()
{
	return CELL_VIDEO_OUT_COLOR_SPACE_RGB;
}