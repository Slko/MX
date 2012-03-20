#pragma once

#define USE_SDL

#ifdef USE_SDL
#	include "sdlsurface.h"
#	include "sdlgraphics.h"
#	define Graphics SDLGraphics
#	define Surface SDLSurface
#endif
