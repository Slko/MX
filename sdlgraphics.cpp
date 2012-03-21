#include "sdlgraphics.h"
#include "debug.h"

SDLGraphics::SDLGraphics()
{
	quit = false;
	started = false;
	mouse_x = 0;
	mouse_y = 0;
}

bool SDLGraphics::start()
{
	Log << "Initialising SDL... ";
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Log << "failed\n";
		return false;
	}
	SDL_WM_SetCaption("MX",NULL);
	Log << "ok\n";
	started = true;
	return true;
}

int SDLGraphics::get_mouse_x()
{
	return mouse_x;
}

int SDLGraphics::get_mouse_y()
{
	return mouse_y;
}

bool SDLGraphics::set_video_mode(int width, int height, int bpp, bool fullscreen, bool hw)
{
	Log << "Setting video mode... ";
	SDL_Surface *screen_surface = SDL_SetVideoMode(width,height,bpp,SDL_DOUBLEBUF | (hw?SDL_HWSURFACE:SDL_SWSURFACE) | (fullscreen?SDL_FULLSCREEN:0));
	if(screen_surface == NULL)
	{
		Log << "failed: unable to set video mode " << width << "x" << height << "x" << bpp << ", " << (fullscreen?"fullscreen":"windowed") << (hw?", hardware-accelerated":"") << "\n";
		return false;
	}
	else
	{
		screen = new SDLSurface(screen_surface);
		Log << "ok\n";
		return true;
	}
}

SDLSurface *SDLGraphics::get_screen()
{
	return screen;
}

void SDLGraphics::flip()
{
	SDL_Flip(screen->get_surface());
}

void SDLGraphics::game_loop(GameLoop *game)
{
	SDL_Event event;
	while(!quit)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if(event.type == SDL_MOUSEMOTION)
			{
				mouse_x = event.motion.x;
				mouse_y = event.motion.y;
			}
		}
		game->step();
		flip();
	}
}

void SDLGraphics::stop()
{
	quit = true;
}

void SDLGraphics::close()
{
	SDL_Quit();
}

SDLGraphics::~SDLGraphics()
{
	if(started)
	{
		close();
	}
}
