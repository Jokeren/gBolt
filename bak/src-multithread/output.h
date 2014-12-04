#include "common.h"
#include <string>
#include <sstream>
#include <vector>

namespace gspan {
	class Output {
		public:
			Output(size_t start_idx) : _m_start_idx(start_idx) {}

			void push_back(const std::string& str, uint32_t nsupport);

			void print();

		private:
			std::vector<std::string> _m_buffer;
			size_t _m_start_idx;
	};
}
