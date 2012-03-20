#pragma once

#include <vector>
#include <utility>
#include "config.h"
#include "map.h"

enum TileTypes
{
	TILE_NONE,
	TILE_BASIC_GROUND,
	TILE_WALL,
};

class TileRenderer
{
protected:
	Graphics *graphics;
	Surface *basic_tile;
	Surface *wall_tile;
	std::vector<Map*> levels;
	Surface *tile_types[256];
	int camera_x;
	int camera_y;

	static const int tile_width=32;
	static const int tile_height=16;
	static const int tile_ground_height=10;
public:
	TileRenderer(Graphics *graphics, std::vector<Map*> &levels);
	std::pair<int,int> to_screen(int x, int y);
	std::pair<int,int> to_tile(int x, int y);
	void render();
	~TileRenderer();
};
