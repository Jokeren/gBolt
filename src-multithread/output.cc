#include "output.h"

namespace gspan {
	void Output::push_back(const std::string& str, uint32_t nsupport)
	{
		_m_support.push_back(nsupport);

		_m_buffer.push_back(str);	
	}

	void Output::print()
	{
		for (size_t i = 0; i < _m_buffer.size(); ++i) {
			printf("t # %zu * %u\n", _m_start_idx + i, _m_support[i]);
			printf("%s", _m_buffer[i].c_str());
		}
	}
}
