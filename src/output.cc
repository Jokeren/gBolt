#include "output.h"

namespace gspan {
	void Output::push_back(const std::string& str, uint32_t nsupport)
	{
		_m_support.push_back(nsupport);

		_m_buffer.push_back(str);	
	}

	void Output::print()
	{
		std::stringstream ss;
		ss << _m_output_file << _m_tid;

		FILE* output_file = fopen(ss.str().c_str(), "w");

		for (size_t i = 0; i < _m_buffer.size(); ++i) {
			fprintf(output_file, "t # %zu * %u\n", _m_start_idx + i, _m_support[i]);
			fprintf(output_file, "%s", _m_buffer[i].c_str());
		}

		fclose(output_file);
	}
}
