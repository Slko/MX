#pragma once

#include <cstdint>

#define SAFE_DELETE(x) if(x != NULL) {delete x; x = NULL;}
#define SAFE_DELETE_ARRAY(x) if(x != NULL) {delete []x; x = NULL;}
#define SAFE_DELETE_SPECIAL(x,y) if(x != NULL) {y(x); x = NULL;}
typedef std::uint32_t Color;
