#include <map>
#include "gspan.h"
#include "database.h"

namespace gspan {
	GSpanReturnCode GSpan::find_frequent_nodes() 
	{
		DataBase *database = DataBase::get_instance();
		_m_graphs = database->get_graph();

		std::map<uint32_t, uint32_t> labels;

		for (size_t i = 0; i < _m_graphs.size(); ++i) {
			std::set<uint32_t> s;
			for (size_t j = 0; j < _m_graphs[i].size(); ++j) {
				const struct vertex_t& vertex = _m_graphs[i].get_vertex(j);	
				s.insert(vertex.label);	
			}

			for (std::set<uint32_t>::iterator it = s.begin(); it != s.end(); ++it) {
				++labels[*it];
			}
		}

		for (std::map<uint32_t, uint32_t>::iterator it = labels.begin(); it != labels.end(); ++it) {
			if (it->second >= _m_nsupport) {
				_m_frequent_labels.insert(std::make_pair(it->first, it->second));
			}
		}

		return GSPAN_SUCCESS;
	}

	GSpanReturnCode GSpan::project() 
	{
		//Notice: the original graph has not been reconstructed!!!
		//one optimization point
		ProjectionMap projection_map;

		//construct the first edge
		for (size_t i = 0; i < _m_graphs.size(); ++i) {
			const Graph& graph = _m_graphs[i];
			for (size_t j = 0; j < graph.size(); ++j) {
				const struct vertex_t& vertex = graph.get_vertex(j);	
				if (_m_frequent_labels.find(vertex.label) == _m_frequent_labels.end()) 
					continue;

				Edges edges;	
				if (get_forward_init(vertex, graph, edges)) {
					for (size_t k = 0; k < edges.size(); ++k) {
						struct dfs_code_t dfs_code;
						//push dfs code according to the same edge label
						const struct vertex_t& vertex_from = graph.get_vertex(edges[k]->from);
						dfs_code.from_label = vertex_from.label;
						dfs_code.from = 0;

						dfs_code.edge_label = edges[k]->label;

						const struct vertex_t& vertex_to = graph.get_vertex(edges[k]->to);
						dfs_code.to = 1;
						dfs_code.to_label = vertex_to.label;

						//push_back all the graphs
						struct pre_dfs_t pre_dfs;
						pre_dfs.id = _m_graphs[i].get_id();
						pre_dfs.edge = edges[k];
						pre_dfs.prev = NULL;

						projection_map[dfs_code].push_back(pre_dfs);
					}
				}
			}
		}	

		//start sub graph mining
		int32_t prev_label = -1;
		for (ProjectionMap::reverse_iterator it = projection_map.rbegin(); it != projection_map.rend(); ++it) {
			if (prev_label != (it->first).from_label) {
				prev_label = (it->first).from_label;
				uint32_t nsupport = _m_frequent_labels[prev_label];

				report(prev_label, nsupport);
			}

			//another parital pruneing, like apriori
			if ((it->second).size() < _m_nsupport)
				continue;

			_m_dfs_codes.push_back((struct dfs_code_t)
					{0, 1, (it->first).from_label, (it->first).edge_label, (it->first).to_label});

	//		if ((it->second)[0].id != 0) {
	//			uint32_t aa = 0;
	//			uint32_t bb = 0;
	//			uint32_t cc = 0;
	//		}
			subgraph_mining(it->second);

			_m_dfs_codes.pop_back();		
		}

		return GSPAN_SUCCESS;
	}

	GSpanReturnCode GSpan::subgraph_mining(Projection& projection)
	{
		uint32_t nsupport = counting_support(projection);
		if (nsupport < _m_nsupport) 
			return GSPAN_SUCCESS;

		//debug
		bool flag = is_min();
		if (!flag)
			return GSPAN_SUCCESS;

		report(nsupport);
#ifdef DEBUG
		printf("subgraph_mining here0\n");
#endif
		std::vector<uint32_t> right_most_path;
		build_right_most_path(_m_dfs_codes, right_most_path);

		uint32_t min_label = _m_dfs_codes[0].from_label;
		//enumerate backward paths and forward paths by different rules
		//
#ifdef DEBUG
		printf("subgraph_mining here1\n");
#endif
		ProjectionMapBackward projection_map_backward;
		ProjectionMapForward projection_map_forward;
		enumerate(projection, right_most_path, projection_map_backward, 
				projection_map_forward, min_label);
#ifdef DEBUG
		printf("subgraph_mining here2\n");
#endif

		//recursive mining according to the paper, 
		//which should be in the order of the first backward to the last backward,
		//and then followed by the last forward to the first forward
		
		for (ProjectionMapBackward::iterator it = projection_map_backward.begin();
				it != projection_map_backward.end(); ++it) {
			_m_dfs_codes.push_back((struct dfs_code_t)
					{(it->first).from, (it->first).to, (it->first).from_label, (it->first).edge_label, (it->first).to_label});
			subgraph_mining(it->second);
			_m_dfs_codes.pop_back();
		}	

		for (ProjectionMapForward::reverse_iterator it = projection_map_forward.rbegin();
				it != projection_map_forward.rend(); ++it) {
			_m_dfs_codes.push_back((struct dfs_code_t) 
					{(it->first).from, (it->first).to, (it->first).from_label, (it->first).edge_label, (it->first).to_label});
			subgraph_mining(it->second);
			_m_dfs_codes.pop_back();
		}
		
		return GSPAN_SUCCESS;
	}

	void GSpan::report(uint32_t label, uint32_t nsupport)
	{
		printf("t # %u * %u\n", _m_report_idx, nsupport);
		++_m_report_idx;

		printf("v 0 %u\n", label);
		printf("\n");
	}

	void GSpan::report(uint32_t nsupport)
	{
		printf("t # %u * %u\n", _m_report_idx, nsupport);
		++_m_report_idx;

		Graph graph;
		build_graph(graph);
		
		for (size_t i = 0; i < graph.size(); ++i) {
			const struct vertex_t& vertex = graph.get_vertex(i);
			printf("v %u %u\n", vertex.id, vertex.label);
		}

		for (size_t i = 0; i < _m_dfs_codes.size(); ++i) {
			printf("e %u %u %u\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to, 
				_m_dfs_codes[i].edge_label);
		}

		printf("\n");
	}
} // namespace gspan
