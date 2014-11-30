#include "seperator.h"

static const uint32_t MAX_LENGTH = 1000;

namespace gspan {
	uint32_t Seperator::seperate(const char* file_path, Input& stream) {
		char line[MAX_LENGTH];

		FILE *fp = fopen(file_path, "r+");

		if (fp == NULL) {
			fprintf(stderr, "error occurs when reading file %s\n", file_path);
			exit(GSPAN_ERROR);
		}
		
		uint32_t ncount = 0;

		while (fgets(line, MAX_LENGTH - 1, fp) != NULL) {
			stream.resize(ncount + 1);
			char *pch = NULL;
			pch = strtok(line, _m_token);
			while (pch != NULL) {
				stream[ncount].push_back(std::string(pch));
				pch = strtok(NULL, _m_token);
			}
			ncount++;
		}	

		fclose(fp);

		return ncount;
	};
}//namespace gspan

