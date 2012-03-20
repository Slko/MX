#pragma once

#include "defines.h"
#include <cstring>

class Map
{
protected:
	char *data;
	int width;
	int height;
public:
	Map(int width, int height,char fill = 0);
	Map(Map &map);
	void fill(char value);
	void replace(char value_old,char value_new);
	void set(int x, int y, char value);
	char get(int x, int y);
	int get_width();
	int get_height();
	~Map();
};
