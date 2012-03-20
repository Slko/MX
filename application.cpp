#include <stack>
#include <list>
#include <vector>
#include <time.h>
#include "application.h"
#include "defines.h"

Surface *Application::generate_minimap(Map *map)
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

Map *Application::generate_map(int width,int height)
{
	Log << "Generating maze... ";
	int maze_width = width*2;
	int maze_height = height*2;
	unsigned int seed = (unsigned int)time(NULL);
	srand(seed);
	std::pair<int,int> exit;
	exit.first = (rand()%(maze_width/2))*2;
	exit.second = (rand()%(maze_height/2))*2;
	std::stack< std::pair<int,int> > stack;
	Map *map = new Map(maze_width,maze_height,TILE_WALL);
	map->set(exit.first,exit.second,TILE_NONE);
	stack.push(std::pair<int,int>(exit.first,exit.second));
	unsigned int max_stack = 0;
	while(!stack.empty())
	{
		std::pair<int,int> pos = stack.top();
		stack.pop();
		std::pair<int,int> neighbors[4] = {std::pair<int,int>(pos.first-2,pos.second),std::pair<int,int>(pos.first+2,pos.second),std::pair<int,int>(pos.first,pos.second-2),std::pair<int,int>(pos.first,pos.second+2)};
		std::list< std::pair<int,int> > usable_neighbors;
		for(int i=0;i<4;i++)
		{
			std::pair<int,int> current = neighbors[i];
			if((current.first >= 0) && (current.first < maze_width) && (current.second >= 0) && (current.second < maze_height))
			{
				usable_neighbors.push_back(current);
			}
		}
		while(!usable_neighbors.empty())
		{
			std::list< std::pair<int,int> >::iterator it = usable_neighbors.begin();
			for(unsigned int i=0;i<rand()%usable_neighbors.size();i++)
			{
				it++;
			}
			std::pair<int,int> chosen = *it;
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
	
Map *Application::resize_map(Map *map)
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

void Application::load()
{
	player = new Surface("images/player.png");
	selection = new Surface("images/selection_tile.png",true);
	Log << "\n";
	Map *wall_map_small = generate_map(64,64);
	wall_map = resize_map(wall_map_small);
	minimap = generate_minimap(wall_map);
	SAFE_DELETE(wall_map_small);
	ground_map = new Map(64,64,TILE_BASIC_GROUND);
	std::vector<Map*> levels;
	levels.push_back(ground_map);
	levels.push_back(wall_map);
	levels.push_back(wall_map);
	tiler = new TileRenderer(graphics,levels);
}

Application::Application()
{
	tiler = NULL;
	wall_map = NULL;
	ground_map = NULL;
	minimap = NULL;
	player = NULL;
	graphics = NULL;
	selection = NULL;
}

void Application::step()
{
	Surface *screen = graphics->get_screen();
	screen->fill_rect(0,0,screen->get_width(),screen->get_height(),0xFF80FF);
	tiler->render();
	screen->blit(minimap,0,0);
	std::pair<int,int> tile = tiler->to_tile(graphics->get_mouse_x(),graphics->get_mouse_y());
	std::pair<int,int> screen_pos = tiler->to_screen(tile.first,tile.second);
	Log << tile.first << " " << tile.second << "\n";
	screen->blit(selection,screen_pos.first,screen_pos.second);
}

int Application::start()
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

Application::~Application()
{
	SAFE_DELETE(selection);
	SAFE_DELETE(wall_map);
	SAFE_DELETE(ground_map);
	SAFE_DELETE(minimap);
	SAFE_DELETE(player);
	SAFE_DELETE(graphics);
	SAFE_DELETE(tiler);
}
