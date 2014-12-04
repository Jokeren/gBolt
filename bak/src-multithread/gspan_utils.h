#ifndef GSPAN_UTILS_H
#define GSPAN_UTILS_H
#include "common.h"

namespace gspan {
	const static uint32_t N_ARG = 3;
	const static char *ARGH[N_ARG] = { "-file", "-support", "-sep"};

	struct parameters_t {
		char *file_path;
		int sep_type;
		double support;
	};

	enum SEP_TYPE {
		DEFAULT,
		SEP_TYPE_NCOUNT
	};

	uint32_t get_parameters(struct parameters_t *p_parameters, int argc, char *argv[]);

	void print_parameters(const struct parameters_t *p_parameters);

	void get_seperator(const struct parameters_t *p_parameters, std::string& seperator);

	uint32_t get_narg();

	void usage();

}//namespace gspan

#endif
