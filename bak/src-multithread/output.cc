#include "output.h"

namespace gspan {
	void Output::push_back(const std::string& str, uint32_t nsupport)
	{
		std::stringstream ss;
		ss << "t # " << _m_start_idx << " * " << nsupport << "\n";
		++_m_start_idx;

		_m_buffer.push_back(ss.str());
		_m_buffer.push_back(str);	
	}

	void Output::print()
	{
		for (size_t i = 0; i < _m_buffer.size(); ++i) {
			printf("%s", _m_buffer[i].c_str());
		}
	}
}
