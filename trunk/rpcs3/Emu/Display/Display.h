#pragma once

#define HAVE_M_PI

#include "SDL.h"
#include "SDL_image.h"
#include "Emu/ElfLoader.h"
#include "Emu/Memory/Memory.h"
#include "Emu/Cell/CPU.h"

static const wxSize display_size = wxSize(640, 480);

class Display
{
	SDL_Surface* m_screen;
	int m_pixel_format;

	uint m_width, m_height, m_b_width;
	u32 m_topaddr;
	u32 m_framecount;

	SDL_TimerID m_utimer;
	SDL_TimerID m_fpstimer;

public:
	Display()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0) 
		{
			ConLog.Error("Unable to initialize SDL: %s", SDL_GetError());
			return;
		}

		m_topaddr = 0x0FFFFFFF;
		m_width = m_height = m_b_width = 0;
		m_framecount = 0;

		m_screen = SDL_SetVideoMode(display_size.GetWidth(), display_size.GetHeight(), 32, 
			/*SDL_RESIZABLE |*/ SDL_DOUBLEBUF);

		m_utimer = SDL_AddTimer(1000/30, UTimer, this);
		if(m_utimer == NULL) ConLog.Error("Error create utimer! (%s)", SDL_GetError());
		m_fpstimer = SDL_AddTimer(1000,  FPSTimer, this);
		if(m_fpstimer == NULL) ConLog.Error("Error create fpstimer! (%s)", SDL_GetError());

		const wxString icon0 = CurGameInfo.root + "\\ICON0.PNG";

		if(!wxFile::Access(icon0, wxFile::read))
		{
			ConLog.Error("Error loading %s: not found!", icon0.c_str());
			return;
		}

		SDL_Surface* tmp = IMG_LoadPNG_RW(SDL_RWFromFile(icon0, "rb"));
		if(tmp == NULL)
		{
			ConLog.Error("Error loading %s: %s!", icon0.c_str(), SDL_GetError());
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

		UpdateDisplay();
	}

	static u32 FPSTimer(u32 interval, void* param)
	{
		if(param != NULL) ((Display*)param)->UpdateFPS();
		return interval;
	}

	static u32 UTimer(u32 interval, void* param)
	{
		if(param != NULL) ((Display*)param)->UpdateDisplay();
		return interval;
	}

	~Display()
	{
		SDL_FreeSurface(m_screen);
		SDL_FreeSurface(SDL_GetVideoSurface());

		SDL_RemoveTimer(m_utimer);
		SDL_RemoveTimer(m_fpstimer);

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

	const u64 GetTicks()
	{
		LARGE_INTEGER count;
		QueryPerformanceCounter( &count );
		return count.QuadPart;
	}

	virtual void SetMode()
	{
		m_b_width = CPU.GPR[4];
		m_width = CPU.GPR[5];
		m_height = CPU.GPR[6];
	}

	virtual void SetBufferAddr()
	{
		m_topaddr = CPU.GPR[6];

		if( m_topaddr < 0x0FFFFFFFLL )
		{
			ConLog.Warning("Addr too small! (%08X)", m_topaddr);
		}
	}

	virtual void UpdateFPS()
	{
		wxString caption;

		if(!CurGameInfo.name.IsEmpty() && !CurGameInfo.serial.IsEmpty())
		{
			caption = wxString::Format("%s - %s", CurGameInfo.name.c_str(), CurGameInfo.serial.c_str());
		}
		else
		{
			caption = "Unknown game";
		}

		SDL_WM_SetCaption
		(
			wxString::Format("%s | FPS: %d", caption, m_framecount),
			"rpcs3 iterpreter"
		);

		m_framecount = 0;
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

	virtual void UpdateImage() //FIXME
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
				u8* Mem = Memory.GetMem(addr);
				if(Mem == NULL) break;

				const u8 r = Mem[addr + 0];
				const u8 g = Mem[addr + 1];
				const u8 b = Mem[addr + 2];
				const u8 a = Mem[addr + 3];

				DrawPixel(x, y, r, g, b, a);

				addr += 4;
			}
		}

		if ( SDL_MUSTLOCK(m_screen) ) SDL_UnlockSurface(m_screen);
	}

	virtual void Flip()
	{
		m_framecount++;
		SetBufferAddr();
		UpdateImage();
	}

	virtual void UpdateDisplay()
	{
		if(m_screen == NULL || m_screen->locked > 0) return;

		SDL_Flip(m_screen);
	}
};