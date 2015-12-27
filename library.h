#ifndef _LIBRARY_LIT_
#define _LIBRARY_LIT_

#include "lit.h"

typedef struct {
	std::string name;
	void *handle;
	int no;
} lib_t;

class LibraryList {
public:
	std::vector<lib_t> lib;
	int count;

	~LibraryList();
	int append(std::string);
	bool is(std::string);
	lib_t *get(std::string);
	uint32_t call(std::string, std::string);
};

extern LibraryList lib_list;

#endif // _LIBRARY_LIT_
