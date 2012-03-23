#pragma once

#include "defines.h"
#include <cstring>
#include <utility>

class Map
{
protected:
	unsigned char *data;
	int width;
	int height;
	int exit_x;
	int exit_y;
public:
	Map(int width, int height, unsigned char fill = 0);
	Map(Map &map);
	void fill(unsigned char value);
	void replace(unsigned char value_old, unsigned char value_new);
	void set_exit(int x, int y);
	std::pair<int,int> get_exit();
	void set(int x, int y, unsigned char value);
	unsigned char get(int x, int y);
	int get_width();
	int get_height();
	~Map();
};
