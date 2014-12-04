#include <cstdio>
#define MFILE "../data/graph.data"

static const uint32_t MAX_LENGTH = 1000;


int main()
{
	FILE *fp = fopen(MFILE, "r+");

		if (fp == NULL) {
			fprintf(stderr, "error occurs when reading file %s\n", file_path);
			exit(GSPAN_ERROR);
		}

		char line[MAX_LENGTH];
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

	return 0;
	
}
