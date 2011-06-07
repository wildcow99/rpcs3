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

		//SDL_TimerInit();

		m_screen = SDL_SetVideoMode(display_size.GetWidth(), display_size.GetHeight(), 32, SDL_DOUBLEBUF);

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

		SDL_Surface* m_image = SDL_DisplayFormat(tmp);
		SDL_FreeSurface(tmp);

		SDL_Rect src, dest;
 
		src.x = 0;
		src.y = 0;

		src.w = m_image->w;
		src.h = m_image->h;

		dest = ResizeImage(m_image);

		SDL_BlitSurface(m_image, &src, m_screen, &dest);
		SDL_FreeSurface(m_image);

		SDL_Flip(m_screen);
	}

	~Display()
	{
		SDL_FreeSurface(m_screen);
		SDL_FreeSurface(SDL_GetVideoSurface());
	}

	SDL_Rect ResizeImage(SDL_Surface* image)
	{
		SDL_Rect dest;

		if(image->w > display_size.GetWidth() || image->h > display_size.GetHeight())
		{
			if(image->w > image->h || image->w == image->h)
			{
				const float mul = ((float)display_size.GetWidth() / (float)image->w);

				dest.w = (int)((float)image->w * mul);
				dest.h = (int)((float)image->h * mul);
			}
			else
			{
				const float mul = ((float)display_size.GetHeight() / (float)image->h);

				dest.w = (int)((float)image->w * mul);
				dest.h = (int)((float)image->h * mul);
			}
		}
		else
		{
			dest.w = image->w;
			dest.h = image->h;
		}

		dest.x = (display_size.GetWidth()  - image->w) / 2;
		dest.y = (display_size.GetHeight() - image->h) / 2;

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
		m_topaddr = CPU.GPR[4];

		if( m_topaddr < 0x0FFFFFFFLL )
		{
			ConLog.Error("Addr too small! (%08X)", m_topaddr);
		}
	}

	virtual void UpdateFPS()
	{
		static float last = 0;
		const float current = (float)GetTicks();

		SDL_WM_SetCaption
		(
			wxString::Format("%s - %s | FPS: %.02f", CurGameInfo.name.c_str(), CurGameInfo.serial.c_str(),
			(60 * 100) * (last / (current * (60 * 100)))),
			"rpcs3 iterpreter"
		);

		last = current;
	}

	virtual void UpdateImage() //FIXME
	{
		if(SDL_LockSurface(m_screen) == 0)
		{
			u8* p = (u8*)m_screen->pixels;
			u32 addr = m_topaddr;

			for (uint y = 0; y < m_height; y++)
			{
				for(uint x = 0; x < m_width; x++, addr += m_screen->format->BytesPerPixel)
				{
					if(m_screen->format->format == SDL_PIXELFORMAT_ABGR8888)
					{
						u8* Mem = Memory.GetMem(addr);
						if(Mem == NULL) break;

						const int p_addr = (x + y * m_b_width) * 4;

						for(uint i=0; i<4; ++i)
						{
							p[p_addr + i] = Mem[addr + i];
						}
					}
				}
			}

			SDL_UnlockSurface(m_screen);
		}
	}

	virtual void Flip()
	{
		UpdateFPS();
		UpdateImage();
		SDL_Flip(m_screen);
	}
};