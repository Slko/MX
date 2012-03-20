#include "tilerenderer.h"
#include "defines.h"

TileRenderer::TileRenderer(Graphics *graphics, std::vector<Map*> &levels)
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

std::pair<int,int> TileRenderer::to_screen(int x, int y)
{
	std::pair<int,int> pos;	
	pos.first = camera_x+x*tile_width/2+y*tile_width/2;
	pos.second = camera_y+y*tile_height/2-x*tile_height/2;
	return pos;
}

std::pair<int,int> TileRenderer::to_tile(int x, int y)
{
	x = x - camera_x;
	y = y - camera_y;
	std::pair<int,int> pos;
	pos.first = -(y*2-x-tile_width/2)/tile_width;
	pos.second = (y+x/2-tile_height/2)/tile_height;
	return pos;
}

void TileRenderer::render()
{
	Surface *screen = graphics->get_screen();
	int level_id = 0;
	for(std::vector<Map*>::iterator it=levels.begin();it<levels.end();it++)
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

TileRenderer::~TileRenderer()
{
	SAFE_DELETE(basic_tile);
	SAFE_DELETE(wall_tile);
}
