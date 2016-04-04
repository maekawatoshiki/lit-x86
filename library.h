#ifndef _LIBRARY_LIT_
#define _LIBRARY_LIT_

#include "common.h"

typedef struct {
	std::string name;
	void *handle;
	int no;
} lib_t;

namespace LibraryList {
	std::vector<lib_t> lib;
	int count;

	void free_libraries();
	int append(std::string);
	bool is(std::string);
	lib_t *get(std::string);
	uint32_t call(std::string, std::string);
};

#endif // _LIBRARY_LIT_
