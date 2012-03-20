#pragma once

#include "defines.h"
#include "SDL.h"
#include "SDL_image.h"

class SDLSurface
{
protected:
	SDL_Surface *surface;
	int width;
	int height;
	bool failed;
public:
	SDLSurface(int width, int height);
	SDLSurface(const char *filename, bool use_key=false, Color color_key=0xFF80FF);
	SDLSurface(SDL_Surface *surface);
	void lock();
	void unlock();
	void set_pixel(int x, int y, Color color);
	Color get_pixel(int x, int y);
	int get_width();
	int get_height();
	bool get_failed();
	void fill_rect(int x, int y, int width, int height, Color color);
	void blit(SDLSurface *surface, int x, int y);
	SDL_Surface *get_surface();
	~SDLSurface();
};
