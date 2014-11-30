#ifndef GSPAN_H
#define GSPAN_H
#include <set>
#include <map>
#include "graph.h"
#include "common.h"

namespace gspan {
	class Seperator;
	class History;

	class GSpan {
		public:
			GSpan():_m_report_idx(0) {};

			GSpan(const char *file_path, uint32_t support) :
				_m_file_path(file_path), _m_support(support), _m_report_idx(0) {};

			void execute(const char *seperator_type, const char *file_path, double support);

		private:
			typedef std::map<struct dfs_code_t, Projection, struct dfs_code_project_compare_t> ProjectionMap;
			typedef std::map<struct dfs_code_t, Projection, struct dfs_code_backward_compare_t> ProjectionMapBackward;
			typedef std::map<struct dfs_code_t, Projection, struct dfs_code_forward_compare_t> ProjectionMapForward;

			GSpanReturnCode read_input(Input& input);

			GSpanReturnCode find_frequent_nodes();

			GSpanReturnCode project();

			GSpanReturnCode subgraph_mining(Projection& projection);

			//extend
			void build_right_most_path(const DfsCodes& dfs_codes, std::vector<uint32_t>& right_most_path);

			void enumerate(const Projection& projection, const std::vector<uint32_t>& right_most_path,
					ProjectionMapBackward& projection_map_backward, ProjectionMapForward& projection_map_forward, uint32_t min_label);

			bool get_forward_init(const struct vertex_t& vertex, const Graph& graph, Edges& edges);

			void get_first_forward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
					ProjectionMapForward& projection_map_forward, uint32_t min_label);

			void get_other_forward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
					ProjectionMapForward& projection_map_forward, uint32_t min_label);

			void get_backward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
					ProjectionMapBackward& projection_map_backward);

			//count
			uint32_t counting_support(const Projection& projection);

			void build_graph(Graph& graph);

			bool is_min();

			bool projection_min(const Projection& projection);

			bool judge_backward(const std::vector<uint32_t>& right_most_path, const Projection& projection, 
					ProjectionMapBackward& projection_map_backward, uint32_t min_label);

			bool judge_forward(const std::vector<uint32_t>& right_most_path, const Projection& projection, 
					ProjectionMapForward& projection_map_forward, uint32_t min_label);

			//report
			void report(uint32_t nsupport);	

			void report(uint32_t label, uint32_t nsupport);	

		private:
			std::vector<Graph> _m_graphs;
			std::map<uint32_t, uint32_t> _m_frequent_labels;
			Seperator *_m_seperator;
			DfsCodes _m_dfs_codes;
			DfsCodes _m_min_dfs_codes;
			Graph _m_min_graph;
			double _m_support;
			uint32_t _m_nsupport;
			const char *_m_file_path;
			uint32_t _m_report_idx;
	};
}//namespace gspan

#endif
