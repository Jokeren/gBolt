#include "common.h"
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>

namespace gspan {
	class Output {
		public:
			Output(const char *output_file, size_t thread_id):
				_m_output_file(output_file), _m_tid(thread_id){}

			void set_start_idx(size_t idx) {
				_m_start_idx = idx;
			}

			size_t size() const {
				return _m_support.size();
			}

			void push_back(const std::string& str, uint32_t nsupport);

			void print();

		private:
			size_t _m_tid;
			FILE* _m_file;
			std::vector<uint32_t> _m_support;
			std::vector<std::string> _m_buffer;
			size_t _m_start_idx;
			const char *_m_output_file;
	};
}
