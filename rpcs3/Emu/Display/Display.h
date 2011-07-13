#pragma once

#include "Emu/ElfLoader.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"
#include "Ini.h"

#define HAVE_M_PI

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"

static const wxSize display_size = wxSize(640, 480);

class Render
{
public:
	uint m_width, m_height, m_b_width;
	u32 m_topaddr;

	virtual void ReadImg(SDL_Surface* surf)=0;
	virtual void UpdateScreen()=0;
	virtual void UpdatePixels()=0;
};

class RenderSoftware : public Render
{
	SDL_Surface* m_screen;

public:
	RenderSoftware()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0) 
		{
			ConLog.Error("Unable to initialize SDL: %s", SDL_GetError());
			return;
		}

		m_screen = SDL_SetVideoMode(display_size.GetWidth(), display_size.GetHeight(), 32, 
			 SDL_DOUBLEBUF);

		const wxString icon0 = CurGameInfo.root + "\\ICON0.PNG";

		if(!wxFile::Access(icon0, wxFile::read))
		{
			ConLog.Error("Error loading %s: not found!", icon0);
			return;
		}

		SDL_Surface* tmp = IMG_LoadPNG_RW(SDL_RWFromFile(icon0, "rb"));
		if(tmp == NULL)
		{
			ConLog.Error("Error loading %s: %s!", icon0, SDL_GetError());
			return;
		}

		SDL_Surface* image = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);

		SDL_Rect src, dest;
 
		src.x = 0;
		src.y = 0;

		src.w = image->w;
		src.h = image->h;

		dest = UpdateImagePos(image);

		SDL_BlitSurface(image, &src, m_screen, &dest);
		SDL_FreeSurface(image);
	}

	~RenderSoftware()
	{
		SDL_FreeSurface(m_screen);
		SDL_FreeSurface(SDL_GetVideoSurface());

		SDL_Quit();
		IMG_Quit();
	}

	SDL_Rect UpdateImagePos(SDL_Surface* image)
	{
		SDL_Rect dest;

		dest.w = image->w;
		dest.h = image->h;
		dest.x = (m_screen->w - image->w) / 2;
		dest.y = (m_screen->h - image->h) / 2;

		return dest;
	}

	virtual void ReadImg(SDL_Surface* surf)
	{
		SDL_Surface* image = SDL_DisplayFormat(surf);
		SDL_FreeSurface(surf);

		SDL_Rect src, dest;
 
		src.x = 0;
		src.y = 0;

		src.w = image->w;
		src.h = image->h;

		dest = UpdateImagePos(image);

		SDL_BlitSurface(image, &src, m_screen, &dest);
		SDL_FreeSurface(image);
	}

	void DrawPixel(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a)
	{
		u8* p = (u8*)m_screen->pixels;

		const int p_addr = (x + y * m_b_width) * 4;

        p[p_addr + 0] = r;
        p[p_addr + 1] = g;
        p[p_addr + 2] = b;
        p[p_addr + 3] = a;
	}

	virtual void UpdatePixels()
	{
		u32 addr = m_topaddr;

		if ( SDL_MUSTLOCK(m_screen) )
		{
			if ( SDL_LockSurface(m_screen) != 0 ) return;
		}

		for (uint y = 0; y < m_height; y++)
		{
			for(uint x = 0; x < m_width; x++)
			{
				const u8 r = Memory.Read8(addr + 0);
				const u8 g = Memory.Read8(addr + 1);
				const u8 b = Memory.Read8(addr + 2);
				const u8 a = Memory.Read8(addr + 3);

				DrawPixel(x, y, r, g, b, a);

				addr += 4;
			}
		}

		if ( SDL_MUSTLOCK(m_screen) ) SDL_UnlockSurface(m_screen);
	}

	virtual void UpdateScreen()
	{
		if(m_screen == NULL) return;

		SDL_Flip(m_screen);
	}
};

struct GLTexture
{
	GLuint tex;

	uint w, h;
	SDL_Surface* output_surf;
	uint bpp;
	uint mode;
	uint type;
	void* pixels;
	uint pixel_count;

	GLTexture(SDL_Surface* _output_surf)
		: tex(NULL)
		, w(0), h(0)
		, output_surf(_output_surf)
		, bpp(4)
		, mode(GL_RGBA)
		, pixels(NULL)
		, pixel_count(0)
	{
	}

	~GLTexture()
	{
		Delete();
	}

	void Create()
	{
		if(tex != NULL) return;

		glGenTextures( 1, &tex );
	}

	void Delete()
	{
		if(tex == NULL) return;

		glDeleteTextures( 1, &tex );
		tex = NULL;
	}

	void Present()
	{
		if( w == 0 || h == 0 || pixels == NULL ||
			bpp == 0 || mode == 0 || type == 0 || pixel_count == 0 ) return;

		Delete();
		Create();

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, bpp, w, h, 0, mode, type, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	void Draw()
	{
		if(tex == NULL) return;

		glClear( GL_COLOR_BUFFER_BIT );

		glBindTexture( GL_TEXTURE_2D, tex );

		glBegin(GL_QUADS);
		{
			glColor3f(1.0f, 1.0f, 1.0f);

			glTexCoord2f(0, w / pixel_count);
			glVertex2f(0, 0);

			glTexCoord2f(h / pixel_count, w / pixel_count);
			glVertex2f(output_surf->w, 0);

			glTexCoord2f(h / pixel_count, 0);
			glVertex2f(output_surf->w, output_surf->h);

			glTexCoord2f(0, 0);
			glVertex2f(0, output_surf->h);
		}
		glEnd();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();

		SDL_GL_SwapBuffers();
	}
};

struct GLImage
{
	GLuint tex;

	uint w, h;
	SDL_Surface* output_surf;
	uint bpp;
	uint mode;
	void* pixels;

	GLImage(SDL_Surface* _output_surf)
		: tex(NULL)
		, w(0), h(0)
		, output_surf(_output_surf)
		, bpp(4)
		, mode(GL_RGBA)
		, pixels(NULL)
	{
	}

	~GLImage()
	{
		Delete();
	}

	void Create()
	{
		if(tex != NULL) return;

		glGenTextures( 1, &tex );
	}

	void Delete()
	{
		if(tex == NULL) return;

		glDeleteTextures( 1, &tex );
		tex = NULL;
	}

	void Present()
	{
		if(w == 0 || h == 0 || pixels == NULL || bpp == 0 || mode == 0) return;

		Delete();
		Create();

		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, bpp, w, h, 0, mode, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTranslatef( (output_surf->w - w) / 2, output_surf->h - h, 0 );
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	}

	void Load(SDL_Surface* surf)
	{
		if(surf == NULL) return;

		switch(surf->format->BytesPerPixel)
		{
			case 3:
				if (surf->format->Rmask & 0x000000ff)
				{
					mode = GL_RGB;
				}
				else
				{
					mode = GL_BGR;
				}
			break;

			case 4:
				if (surf->format->Rmask & 0x000000ff)
				{
					mode = GL_RGBA;
				}
				else
				{
					mode = GL_BGRA;
				}
			break;

			default:
				ConLog.Error("The image is not truecolor!");
			return;
		}

		pixels = surf->pixels;
		w = surf->w;
		h = surf->h;
		bpp = surf->format->BytesPerPixel;
	}

	void Draw()
	{
		if(tex == NULL) return;

		glClear( GL_COLOR_BUFFER_BIT );

		glBindTexture( GL_TEXTURE_2D, tex );

		glBegin(GL_QUADS);
		{
			glColor3f(1.0f, 1.0f, 1.0f);

			glTexCoord2f(0, 1);
			glVertex2f(0, 0);

			glTexCoord2f(1, 1);
			glVertex2f(w, 0);

			glTexCoord2f(1, 0);
			glVertex2f(w, h);

			glTexCoord2f(0, 0);
			glVertex2f(0, h);
		}
		glEnd();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();

		SDL_GL_SwapBuffers();
	}
};

class RenderOGL : public Render
{
	SDL_Surface* m_screen;
	GLTexture m_tex;

	uint m_width, m_height, m_b_width;
	u32 m_topaddr;
	u32 m_framecount;

	SDL_TimerID m_utimer;
	SDL_TimerID m_fpstimer;

public:
	RenderOGL() : m_tex(m_screen)
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0) 
		{
			ConLog.Error("Unable to initialize SDL: %s", SDL_GetError());
			return;
		}

		m_screen = SDL_SetVideoMode(display_size.GetWidth(), display_size.GetHeight(), 32, SDL_OPENGL);

		glClearColor( 0, 0, 0, 0 );

		glEnable( GL_TEXTURE_2D );

		glViewport( 0, 0, display_size.GetWidth(), display_size.GetHeight() );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		glOrtho( 0, display_size.GetWidth(), display_size.GetHeight(), 0, -1, 1 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	}

	~RenderOGL()
	{
		SDL_FreeSurface(m_screen);

		SDL_RemoveTimer(m_utimer);
		SDL_RemoveTimer(m_fpstimer);

		SDL_Quit();
		IMG_Quit();
	}

	virtual void ReadImg(SDL_Surface* surf)
	{
		GLImage gl_img(m_screen);
		{
			gl_img.Load(surf);
			gl_img.Present();
			gl_img.Draw();
		}
		gl_img.Delete();

		SDL_FreeSurface(surf);
	}

	virtual void UpdatePixels()
	{
		if(!Memory.Video_FrameBuffer.GetMemFromAddr(m_topaddr, m_tex.pixels))
		{
			ConLog.Error("Update Image ERROR: Unknown topaddr! (%d)", m_topaddr);
			return;
		}

		m_tex.Present();
		m_tex.Draw();
	}

	virtual void UpdateScreen()
	{
		if(m_screen == NULL) return;

		SDL_GL_SwapBuffers();
	}
};

class Display
{
	u32 m_framecount;

	SDL_TimerID m_utimer;
	SDL_TimerID m_fpstimer;
	Render* renderer;

public:
	Display()
	{
		switch(Ini.Emu.m_RenderMode.GetValue())
		{
		case 0: renderer = new RenderSoftware(); break;
		
		default:
		case 1: renderer = new RenderOGL(); break;
		}
		
		renderer->m_topaddr = Memory.Video_FrameBuffer.GetStartAddr();
		renderer->m_width = renderer->m_height = renderer->m_b_width = 0;
		m_framecount = 0;

		m_utimer = SDL_AddTimer(1000/30, UTimer, this);
		if(m_utimer == NULL) ConLog.Error("Error create utimer! (%s)", SDL_GetError());
		m_fpstimer = SDL_AddTimer(1000,  FPSTimer, this);
		if(m_fpstimer == NULL) ConLog.Error("Error create fpstimer! (%s)", SDL_GetError());

		const wxString icon0 = CurGameInfo.root + "\\ICON0.PNG";

		if(!wxFile::Access(icon0, wxFile::read))
		{
			ConLog.Error("Error loading %s: not found!", icon0);
			return;
		}

		SDL_Surface* tmp = IMG_LoadPNG_RW(SDL_RWFromFile(icon0, "rb"));
		if(tmp == NULL)
		{
			ConLog.Error("Error loading %s: %s!", icon0, SDL_GetError());
			return;
		}

		renderer->ReadImg(tmp);
	}

	static u32 FPSTimer(u32 interval, void* param)
	{
		if(param) ((Display*)param)->UpdateFPS();
		return interval;
	}

	static u32 UTimer(u32 interval, void* param)
	{
		if(param) ((Display*)param)->renderer->UpdateScreen();
		return interval;
	}

	void UpdateFPS()
	{
		wxString caption;

		if(!CurGameInfo.name.IsEmpty() && !CurGameInfo.serial.IsEmpty())
		{
			caption = wxString::Format("%s - %s", CurGameInfo.name, CurGameInfo.serial);
		}
		else
		{
			caption = "Unknown game";
		}

		SDL_WM_SetCaption
		(
			wxString::Format("%s | FPS: %d", caption, m_framecount), wxEmptyString
		);

		m_framecount = 0;
	}

	virtual void SetBufferAddr(const u64 addr)
	{
		if( addr < Memory.Video_FrameBuffer.GetStartAddr() )
		{
			ConLog.Warning("Addr too small! (%08X)", addr);
			renderer->m_topaddr = Memory.Video_FrameBuffer.GetStartAddr();
		}
		else
		{
			renderer->m_topaddr = addr;
		}
	}

	void SetMode(const u64 b_w, const u64 w, const u64 h)
	{
		renderer->m_b_width = b_w;
		renderer->m_width = w;
		renderer->m_height = h;
	}

	void UpdateScreen()
	{
		m_framecount++;
		renderer->UpdateScreen();
	}

	void UpdatePixels()
	{
		renderer->UpdatePixels();
	}
};