#include "debug.h"

#ifdef _DEBUG
	std::ostream &Log = std::cout;
#elif defined(NO_DEBUG)
	nullstream Log;
#else
	std::ofstream Log("mx_log.txt");
#endif
