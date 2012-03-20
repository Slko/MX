#include "config.h"
#include "debug.h"
#include "application.h"

void show_system_info()
{
#ifdef USE_SDL
	Log << "System information:\n";
	const SDL_version *linked_version = SDL_Linked_Version();
	SDL_version compiled_version;
	SDL_VERSION(&compiled_version);
	Log << "Compiled with SDL version: " << (int)compiled_version.major << "." << (int)compiled_version.minor << "." << (int)compiled_version.patch << "\n";
	Log << "Running with SDL version: " << (int)linked_version->major << "." << (int)linked_version->minor << "." << (int)linked_version->patch << "\n";
	if((compiled_version.major != linked_version->major)||(compiled_version.minor != linked_version->minor)||(compiled_version.patch != linked_version->patch))
	{
		Log << "Warning: SDL version mismatch\n";
	}
	Log << "\n";
	const SDL_version *image_linked_version = IMG_Linked_Version();
	SDL_version image_compiled_version;
	SDL_IMAGE_VERSION(&image_compiled_version);
	Log << "Compiled with SDL_image version: " << (int)image_compiled_version.major << "." << (int)image_compiled_version.minor << "." << (int)image_compiled_version.patch << "\n";
	Log << "Running with SDL_image version: " << (int)image_linked_version->major << "." << (int)image_linked_version->minor << "." << (int)image_linked_version->patch << "\n";
	if((image_compiled_version.major != image_linked_version->major)||(image_compiled_version.minor != image_linked_version->minor)||(image_compiled_version.patch != image_linked_version->patch))
	{
		Log << "Warning: SDL_image version mismatch\n";
	}
	Log << "\n";
#endif
}

int main(int argc, char **args)
{
	Application app;
	Log << "Starting MX...\n\n";
	show_system_info();
	int result = app.start();
#if defined(_DEBUG)&&defined(WIN32)
	if(result != 0)
	{
		system("pause");
	}
#endif
	return result;
}
