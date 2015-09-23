#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef NDEBUG
// not use debug
	#define debug(s, ...) ((void)0)
#else /* !NDEBUG */
	// Use debug
	#include "lit.h"
	void debug(char *str, ...) {
		va_list args;
		va_start(args, str);
		vprintf(str, args);
		va_end(args);
	}
#endif

#endif
