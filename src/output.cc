#include "output.h"

namespace gspan {
	void Output::push_back(const std::string& str)
	{
		_m_buffer.push_back(str);	
	}

	void Output::push_back(const std::string& str, uint32_t nsupport)
	{
		_m_support.push_back(nsupport);

		_m_buffer.push_back(str);	
	}

	void Output::push_back(const std::string& str, uint32_t nsupport, int32_t parent_id)
	{
		_m_support.push_back(nsupport);

		_m_parent.push_back(parent_id);

		_m_buffer.push_back(str);	
	}

	void Output::push_back(const std::string& str, uint32_t nsupport, size_t graph_id, int32_t parent_id)
	{
		_m_graph.push_back(graph_id);

		_m_support.push_back(nsupport);

		_m_parent.push_back(parent_id);

		_m_buffer.push_back(str);	
	}

	void Output::print()
	{
		std::stringstream ss;
		ss << _m_output_file;

		FILE* output_file = fopen(ss.str().c_str(), "a+");

		for (size_t i = 0; i < _m_buffer.size(); ++i) {
			fprintf(output_file, "t # %zu * %u\n", _m_start_idx + _m_graph[i], _m_support[i]);
			if (_m_parent[i] == -1)
				fprintf(output_file, "parent : -1\n");
			else
				fprintf(output_file, "parent : %d\n", _m_start_idx + _m_parent[i]);
			fprintf(output_file, "%s", _m_buffer[i].c_str());
		}

		fclose(output_file);
	}
}
