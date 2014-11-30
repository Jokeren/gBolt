#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>

typedef std::vector<std::vector<std::string> > Input;

enum GSpanReturnCode {
	GSPAN_SUCCESS,
	GSPAN_WARNING,
	GSPAN_ERROR,
	GSPAN_NCOUNT
};

#endif
