#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <stack>
#include <list>
#include <vector>
#include <stdint.h>
using namespace std;

#ifdef NO_DEBUG
#	undef _DEBUG
#endif

struct nullstream {};
template <typename T>
nullstream & operator<<(nullstream & o, T const & x){return o;}

#ifdef _DEBUG
	ostream &Log = cout;
#elif defined(NO_DEBUG)
	nullstream Log;
#else
	ofstream Log("mx_log.txt");
#endif

#define SAFE_DELETE(x) if(x != NULL) {delete x; x = NULL;}
#define SAFE_DELETE_ARRAY(x) if(x != NULL) {delete []x; x = NULL;}
#define SAFE_DELETE_SPECIAL(x,y) if(x != NULL) {y(x); x = NULL;}
typedef uint32_t Color;

#define TILE_NONE 0
#define TILE_BASIC_GROUND 1
#define TILE_WALL 2

class Map
{
protected:
	char *data;
	int width;
	int height;
public:
	Map(int width, int height,char fill = 0)
	{
		data = new char[width*height];
		memset(data,fill,width*height);
		this->width = width;
		this->height = height;
	}
	Map(Map &map)
	{
		this->data = new char[map.width*map.height];
		memcpy(data,map.data,map.width*map.height);
		width = map.width;
		height = map.height;
	}
	void fill(char value)
	{
		memset(data,value,width*height);
	}
	void replace(char value_old,char value_new)
	{
		for(int y=0;y<height;y++)
		{
			for(int x=0;x<width;x++)
			{
				if(get(x,y) == value_old)
				{
					set(x,y,value_new);
				}
			}
		}
	}
	void set(int x, int y, char value)
	{
		data[x+y*width] = value;
	}
	char get(int x, int y)
	{
		return data[x+y*width];
	}
	int get_width()
	{
		return width;
	}
	int get_height()
	{
		return height;
	}
	~Map()
	{
		SAFE_DELETE_ARRAY(data);
	}
};
class GameLoop
{
public:
	virtual void step() = 0;
};
class SDLSurface
{
protected:
	SDL_Surface *surface;
	int width;
	int height;
	bool failed;
public:
	SDLSurface(int width, int height)
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
	}
	SDLSurface(const char *filename, bool use_key=false, Color color_key=0xFF80FF)
	{
		Log << "Loading image from \"" << filename << "\"... ";
		SDL_Surface *loadedImage;
		SDL_Surface *optimizedImage;
		loadedImage = IMG_Load(filename);
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
	SDLSurface(SDL_Surface *surface)
	{
		this->surface = surface;
		width = surface->w;
		height = surface->h;
	}
	void lock()
	{
		if(surface != NULL)
		{
			SDL_LockSurface(surface);
		}
	}
	void unlock()
	{
		if(surface != NULL)
		{
			SDL_UnlockSurface(surface);
		}
	}
	void set_pixel(int x, int y, Color color)
	{
		if(surface != NULL)
		{
			*(Color*)((char*)surface->pixels+y*surface->pitch+x*4) = color;
		}
	}
	Color get_pixel(int x, int y)
	{
		return *(Color*)((char*)surface->pixels+y*surface->pitch+x*4);
	}
	int get_width()
	{
		return width;
	}
	int get_height()
	{
		return height;
	}
	bool get_failed()
	{
		return failed;
	}
	void fill_rect(int x, int y, int width, int height, Color color)
	{
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;
		SDL_FillRect(surface,&rect,color);
	}
	void blit(SDLSurface *surface, int x, int y)
	{
		SDL_Rect pos;
		pos.x = x;
		pos.y = y;
		SDL_BlitSurface(surface->surface,0,this->surface,&pos);
	}
	SDL_Surface *get_surface()
	{
		return surface;
	}
	~SDLSurface()
	{
		SAFE_DELETE_SPECIAL(surface,SDL_FreeSurface);
	}
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
	SDLGraphics()
	{
		quit = false;
		started = false;
		mouse_x = 0;
		mouse_y = 0;
	}
	bool start()
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
	int get_mouse_x()
	{
		return mouse_x;
	}
	int get_mouse_y()
	{
		return mouse_y;
	}
	bool set_video_mode(int width, int height, int bpp, bool fullscreen, bool hw=true)
	{
		Log << "Setting video mode... ";
		SDL_Surface *screen_surface = SDL_SetVideoMode(width,height,bpp,SDL_DOUBLEBUF | (hw?SDL_HWSURFACE:0) | (fullscreen?SDL_FULLSCREEN:0));
		if(screen == NULL)
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
	SDLSurface *get_screen()
	{
		return screen;
	}
	void flip()
	{
		SDL_Flip(screen->get_surface());
	}
	void game_loop(GameLoop *game)
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
	void close()
	{
		SDL_Quit();
	}
	~SDLGraphics()
	{
		if(started)
		{
			close();
		}
	}
};

#define Graphics SDLGraphics
#define Surface SDLSurface

class TileRenderer
{
protected:
	Graphics *graphics;
	Surface *basic_tile;
	Surface *wall_tile;
	vector<Map*> levels;
	Surface *tile_types[256];
	int camera_x;
	int camera_y;

	static const int tile_width=32;
	static const int tile_height=16;
	static const int tile_ground_height=10;
public:
	TileRenderer(Graphics *graphics, vector<Map*> &levels)
	{
		this->graphics = graphics;
		this->levels = levels;
		basic_tile = new Surface("images/basic_tile.png",true);
		wall_tile = new Surface("images/wall_tile.png",true);
		for(int i=0;i<256;i++)
		{
			tile_types[i] = 0;
		}
		tile_types[TILE_BASIC_GROUND] = basic_tile;
		tile_types[TILE_WALL] = wall_tile;
		camera_x = 0;
		camera_y = 300;
	}
	pair<int,int> to_screen(int x, int y)
	{
		pair<int,int> pos;	
		pos.first = camera_x+x*tile_width/2+y*tile_width/2;
		pos.second = camera_y+y*tile_height/2-x*tile_height/2;
		return pos;
	}
	pair<int,int> to_tile(int x, int y)
	{
		x = x - camera_x;
		y = y - camera_y;
		pair<int,int> pos;
		pos.first = -(y*2-x-tile_width/2)/tile_width;
		pos.second = (y+x/2-tile_height/2)/tile_height;
		return pos;
	}
	void render()
	{
		Surface *screen = graphics->get_screen();
		int level_id = 0;
		for(vector<Map*>::iterator it=levels.begin();it<levels.end();it++)
		{
			int level_width = (*it)->get_width();
			int level_height = (*it)->get_height();
			for(int y=0;y<level_height;y++)
			{
				for(int x=level_width-1;x>=0;x--)
				{
					char type_id = (*it)->get(x,y);
					Surface *current_tile = tile_types[type_id];
					if(current_tile != NULL)
					{
						screen->blit(current_tile,camera_x+x*tile_width/2+y*tile_width/2,camera_y+y*tile_height/2-x*tile_height/2-level_id*tile_ground_height);
					}
				}
			}
			level_id++;
		}
	}
	~TileRenderer()
	{
		SAFE_DELETE(basic_tile);
		SAFE_DELETE(wall_tile);
	}
};

class Application : public GameLoop
{
protected:
	Graphics *graphics;
	Surface *player;
	Surface *minimap;
	Surface *selection;
	Map *wall_map;
	Map *ground_map;
	TileRenderer *tiler;
	bool quit;

	Surface *generate_minimap(Map *map)
	{
		Surface *minimap = new Surface(map->get_width(),map->get_height());
		minimap->lock();
		for(int y=0;y<map->get_height();y++)
		{
			for(int x=0;x<map->get_width();x++)
			{
				Color color;
				char tile = map->get(x,y);
				if(tile == TILE_NONE)
				{
					color = 0xFFFFFFFF;
				}
				else if(tile == TILE_WALL)
				{
					color = 0xFF00FF00;
				}
				else
				{
					Log << "Warning: unknown tile type: " << tile << "\n";
					color = 0xFF0000FF;
				}
				minimap->set_pixel(x,y,color);
			}
		}
		minimap->unlock();
		return minimap;
	}

	Map *generate_map(int width,int height)
	{
		Log << "Generating maze... ";
		int maze_width = width*2;
		int maze_height = height*2;
		unsigned int seed = (unsigned int)time(NULL);
		srand(seed);
		pair<int,int> exit;
		exit.first = (rand()%(maze_width/2))*2;
		exit.second = (rand()%(maze_height/2))*2;
		stack<pair<int,int>> stack;
		Map *map = new Map(maze_width,maze_height,TILE_WALL);
		map->set(exit.first,exit.second,TILE_NONE);
		stack.push(pair<int,int>(exit.first,exit.second));
		unsigned int max_stack = 0;
		while(!stack.empty())
		{
			pair<int,int> pos = stack.top();
			stack.pop();
			pair<int,int> neighbors[4] = {pair<int,int>(pos.first-2,pos.second),pair<int,int>(pos.first+2,pos.second),pair<int,int>(pos.first,pos.second-2),pair<int,int>(pos.first,pos.second+2)};
			list<pair<int,int>> usable_neighbors;
			for(int i=0;i<4;i++)
			{
				pair<int,int> current = neighbors[i];
				if((current.first >= 0) && (current.first < maze_width) && (current.second >= 0) && (current.second < maze_height))
				{
					usable_neighbors.push_back(current);
				}
			}
			while(!usable_neighbors.empty())
			{
				list<pair<int,int>>::iterator it = usable_neighbors.begin();
				for(unsigned int i=0;i<rand()%usable_neighbors.size();i++)
				{
					it++;
				}
				pair<int,int> chosen = *it;
				if(map->get(chosen.first,chosen.second) == TILE_WALL)
				{
					map->set((pos.first+chosen.first)/2,(pos.second+chosen.second)/2,TILE_NONE);
					map->set(chosen.first,chosen.second,TILE_NONE);
					stack.push(chosen);
				}
				usable_neighbors.remove(chosen);
			}
			if(stack.size() > max_stack)
			{
				max_stack = stack.size();
			}
		}
		Log << "ok\n";
		Log << "Maximal stack size: " << max_stack << "\n";
		return map;
	}
	
	Map *resize_map(Map *map)
	{
		Map *new_map = new Map((map->get_width()-2)*2+1,(map->get_height()-2)*2+1);
		for(int y=0;y<(map->get_height()-2)/2;y++)
		{
			for(int x=0;x<(map->get_width()-2)/2;x++)
			{
				char tile1 = map->get(x*2,y*2);
				char tile2 = map->get(x*2+1,y*2);
				char tile3 = map->get(x*2,y*2+1);
				char tile4 = map->get(x*2+1,y*2+1);
				for(int i=0;i<3;i++)
				{
					new_map->set(1+x*4+i,1+y*4+3,tile3);
					new_map->set(1+x*4+3,1+y*4+i,tile2);
				}
				new_map->set(1+x*4+3,1+y*4+3,tile4);
				for(int y2=0;y2<3;y2++)
				{
					for(int x2=0;x2<3;x2++)
					{
						new_map->set(1+x*4+x2,1+y*4+y2,tile1);
					}
				}
			}
		}
		new_map->set(0,0,TILE_WALL);
		new_map->set(new_map->get_width()-1,new_map->get_height()-1,TILE_WALL);
		for(int i=1;i<new_map->get_width();i++)
		{
			new_map->set(i,0,TILE_WALL);
			new_map->set(i,new_map->get_height()-1,TILE_WALL);
		}
		for(int i=1;i<new_map->get_height();i++)
		{
			new_map->set(0,i,TILE_WALL);
			new_map->set(new_map->get_width()-1,i,TILE_WALL);
		}
		return new_map;
	}

	void load()
	{
		player = new Surface("images/player.png");
		selection = new Surface("images/selection_tile.png",true);
		Log << "\n";
		Map *wall_map_small = generate_map(64,64);
		wall_map = resize_map(wall_map_small);
		minimap = generate_minimap(wall_map);
		SAFE_DELETE(wall_map_small);
		ground_map = new Map(64,64,TILE_BASIC_GROUND);
		vector<Map*> levels;
		levels.push_back(ground_map);
		levels.push_back(wall_map);
		levels.push_back(wall_map);
		tiler = new TileRenderer(graphics,levels);
	}

public:
	Application()
	{
		tiler = NULL;
		wall_map = NULL;
		ground_map = NULL;
		minimap = NULL;
		player = NULL;
		graphics = NULL;
		selection = NULL;
		quit = false;
	}

	void step()
	{
		Surface *screen = graphics->get_screen();
		screen->fill_rect(0,0,screen->get_width(),screen->get_height(),0xFF80FF);
		tiler->render();
		screen->blit(minimap,0,0);
		pair<int,int> tile = tiler->to_tile(graphics->get_mouse_x(),graphics->get_mouse_y());
		pair<int,int> screen_pos = tiler->to_screen(tile.first,tile.second);
		Log << tile.first << " " << tile.second << "\n";
		screen->blit(selection,screen_pos.first,screen_pos.second);
	}

	int start()
	{
		graphics = new Graphics();
		if(!graphics->start())
		{
			return 1;
		}
		if(!graphics->set_video_mode(800,600,32,false,true))
		{
			return 1;
		}
		Log << "Loading resources...\n";
		load();
		Log << "\nEntering game loop\n";
		graphics->game_loop(this);
		return 0;
	}

	~Application()
	{
		SAFE_DELETE(selection);
		SAFE_DELETE(wall_map);
		SAFE_DELETE(ground_map);
		SAFE_DELETE(minimap);
		SAFE_DELETE(player);
		SAFE_DELETE(graphics);
		SAFE_DELETE(tiler);
	}
};

void show_system_info()
{
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
