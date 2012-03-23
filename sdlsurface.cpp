#include "sdlsurface.h"
#include "debug.h"

SDLSurface::SDLSurface(int width, int height)
{
	Log << "Creating surface... ";
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,32,0xFF,0xFF00,0xFF0000,0xFF000000);
	if(surface == NULL)
	{
		failed = true;
		Log << "failed\n";
	}
	else
	{
		Log << "ok\n";
		failed = false;
	}
	this->width = width;
	this->height = height;
	free_on_delete = true;
}

SDLSurface::SDLSurface(const char *filename, bool use_key, Color color_key)
{
	Log << "Loading image from \"" << filename << "\"... ";
	SDL_Surface *loadedImage;
	SDL_Surface *optimizedImage;
	loadedImage = IMG_Load(filename);
	free_on_delete = true;
	if(loadedImage != NULL)
	{
		optimizedImage = SDL_DisplayFormat(loadedImage);
		SDL_FreeSurface(loadedImage);
		bool key_error = false;
		if(use_key)
		{
			if(SDL_SetColorKey(optimizedImage,SDL_SRCCOLORKEY,color_key) == -1)
			{
				key_error = true;
			}
		}
		if(key_error)
		{
			Log << "ok with warning: unable to set color key\n";
		}
		else
		{
			Log << "ok\n";
		}
		surface = optimizedImage;
		failed = false;
		width = optimizedImage->w;
		height = optimizedImage->h;
	}
	else
	{
		Log << "failed\n";
		surface = NULL;
		failed = true;
		width = 0;
		height = 0;
	}
}

SDLSurface::SDLSurface(SDL_Surface *surface,bool free_on_delete)
{
	this->free_on_delete = free_on_delete;
	this->surface = surface;
	width = surface->w;
	height = surface->h;
}

void SDLSurface::lock()
{
	if(surface != NULL)
	{
		SDL_LockSurface(surface);
	}
}

void SDLSurface::unlock()
{
	if(surface != NULL)
	{
		SDL_UnlockSurface(surface);
	}
}

void SDLSurface::set_pixel(int x, int y, Color color)
{
	if(surface != NULL)
	{
		*(Color*)((char*)surface->pixels+y*surface->pitch+x*4) = color;
	}
}

Color SDLSurface::get_pixel(int x, int y)
{
	return *(Color*)((char*)surface->pixels+y*surface->pitch+x*4);
}

int SDLSurface::get_width()
{
	return width;
}

int SDLSurface::get_height()
{
	return height;
}

bool SDLSurface::get_failed()
{
	return failed;
}

void SDLSurface::fill_rect(int x, int y, int width, int height, Color color)
{
	SDL_Rect rect;
	rect.x = (Sint16)x;
	rect.y = (Sint16)y;
	rect.w = (Uint16)width;
	rect.h = (Uint16)height;
	SDL_FillRect(surface,&rect,color);
}

void SDLSurface::blit(SDLSurface *surface, int x, int y)
{
	SDL_Rect pos;
	pos.x = (Sint16)x;
	pos.y = (Sint16)y;
	SDL_BlitSurface(surface->surface,0,this->surface,&pos);
}

SDL_Surface *SDLSurface::get_surface()
{
	return surface;
}

SDLSurface::~SDLSurface()
{
	if(free_on_delete)
	{
		SAFE_DELETE_SPECIAL(surface,SDL_FreeSurface);
	}
	else
	{
		surface = NULL;
	}
}
