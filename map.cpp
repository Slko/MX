#include "map.h"

Map::Map(int width, int height,char fill)
{
	data = new char[width*height];
	std::memset(data,fill,width*height);
	this->width = width;
	this->height = height;
}

Map::Map(Map &map)
{
	this->data = new char[map.width*map.height];
	std::memcpy(data,map.data,map.width*map.height);
	width = map.width;
	height = map.height;
}

void Map::fill(char value)
{
	std::memset(data,value,width*height);
}

void Map::replace(char value_old,char value_new)
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

void Map::set(int x, int y, char value)
{
	data[x+y*width] = value;
}

char Map::get(int x, int y)
{
	return data[x+y*width];
}

int Map::get_width()
{
	return width;
}

int Map::get_height()
{
	return height;
}

Map::~Map()
{
	SAFE_DELETE_ARRAY(data);
}
