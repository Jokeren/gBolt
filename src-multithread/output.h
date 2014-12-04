#include "common.h"
#include <string>
#include <sstream>
#include <vector>

namespace gspan {
	class Output {
		public:
			Output(){}

			void set_start_idx(size_t idx) {
				_m_start_idx = idx;
			}

			size_t size() const {
				return _m_support.size();
			}

			void push_back(const std::string& str, uint32_t nsupport);

			void print();

		private:
			std::vector<uint32_t> _m_support;
			std::vector<std::string> _m_buffer;
			size_t _m_start_idx;
	};
}
