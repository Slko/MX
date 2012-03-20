#pragma once

#include "debug.h"
#include "map.h"
#include "config.h"
#include "tilerenderer.h"

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

	Surface *generate_minimap(Map *map);
	Map *generate_map(int width,int height);
	Map *resize_map(Map *map);
	void load();
public:
	Application();
	void step();
	int start();
	~Application();
};
