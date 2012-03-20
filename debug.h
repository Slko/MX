#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>

#ifdef NO_DEBUG
#	undef _DEBUG
#endif

struct nullstream {};
template <typename T>
nullstream & operator<<(nullstream & o, T const & x){return o;}

#ifdef _DEBUG
	extern std::ostream Log;
#elif defined(NO_DEBUG)
	extern nullstream Log;
#else
	extern std::ofstream Log;
#endif
