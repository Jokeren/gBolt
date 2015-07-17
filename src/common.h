#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdint.h>

typedef std::vector<std::vector<std::string> > Input;

enum GSpanReturnCode {
	GSPAN_SUCCESS,
	GSPAN_WARNING,
	GSPAN_ERROR,
	GSPAN_NCOUNT
};

static const uint32_t THREAD_NUM = 12;

#endif
