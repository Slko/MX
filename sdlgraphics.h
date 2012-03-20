#pragma once

#include "defines.h"
#include "SDL.h"
#include "SDL_image.h"
#include "sdlsurface.h"

class GameLoop
{
public:
	virtual void step() = 0;
};


class SDLGraphics
{
protected:
	bool started;
	bool quit;
	SDLSurface *screen;
	int mouse_x;
	int mouse_y;
public:
	SDLGraphics();
	bool start();
	int get_mouse_x();
	int get_mouse_y();
	bool set_video_mode(int width, int height, int bpp, bool fullscreen, bool hw=true);
	SDLSurface *get_screen();
	void flip();
	void game_loop(GameLoop *game);
	void close();
	~SDLGraphics();
};
