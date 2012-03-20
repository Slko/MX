#pragma once

#define USE_SDL

#ifdef USE_SDL
#	include "sdlsurface.h"
#	include "sdlgraphics.h"
	typedef SDLGraphics Graphics;
	typedef SDLSurface Surface;
#endif
