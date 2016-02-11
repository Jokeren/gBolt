#include <map>
#include <sstream>
#include "gspan.h"
#include "database.h"
static const uint32_t BUFFER_SIZE = 10240;

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

	GSpanReturnCode GSpan::reconstruct(Input& input)
	{
		Graph graph;
		Vertice vertice;
		std::vector<uint32_t> labels;
		std::map<uint32_t, uint32_t> id_map;

		DataBase *database = DataBase::get_instance();
		database->clear();

		uint32_t graph_idx = 0;
		uint32_t edge_id = 0;
		uint32_t vertex_id = 0;

		for (size_t i = 0; i < input.size(); ++i) {
			if (input[i][0] == "t") {
				if (i != 0) {
					graph.set_nedges(edge_id);
					graph.set_vertice(vertice);
					edge_id = 0;
					vertex_id = 0;
					database->push_graph(graph);	
					graph.clear();
					vertice.clear();
					labels.clear();
					id_map.clear();
				}

				char indicator, seperator;
				uint32_t idx;
				indicator = input[i][0][0];
				seperator = input[i][1][0];
				sscanf(input[i][2].c_str(), "%u", &idx);

				if (graph_idx != idx) {
					fprintf(stderr, "reading input warning! %u %u\n", graph_idx, idx);	
				}
				graph.set_id(idx);

				++graph_idx;
			} else if (input[i][0] == "v") {
				char indicator;
				uint32_t id, label;
				indicator = input[i][0][0];
				sscanf(input[i][1].c_str(), "%u", &id);
				sscanf(input[i][2].c_str(), "%u", &label);

				struct vertex_t vertex;
				labels.push_back(label);

				if (_m_frequent_labels.find(label) != _m_frequent_labels.end()) {
					vertex.label = label;	
					vertex.id = vertex_id;
					vertice.push_back(vertex);
					id_map[id] = vertex_id;
					++vertex_id;
				}
			} else if (input[i][0] == "e") {
				char indicator;
				uint32_t from, to, label;
				indicator = input[i][0][0];
				sscanf(input[i][1].c_str(), "%u", &from);
				sscanf(input[i][2].c_str(), "%u", &to);
				sscanf(input[i][3].c_str(), "%u", &label);
				//debug
				//printf("edge label %u\n", label);

				uint32_t label_from = labels[from];
				uint32_t label_to = labels[to];

				if (_m_frequent_labels.find(label_from) == _m_frequent_labels.end() ||
						_m_frequent_labels.find(label_to) == _m_frequent_labels.end())
					continue;

				struct edge_t edge;
				edge.from = id_map[from];
				edge.to = id_map[to];
				edge.label = label;
				edge.id = edge_id;
				++edge_id;

				//first edge
				vertice[id_map[from]].edges.push_back(edge);

				//second edge
				edge.from = id_map[to];
				edge.to = id_map[from];
				vertice[id_map[to]].edges.push_back(edge);
			} else {
				fprintf(stderr, "reading input warning!");	
			}
		}

		graph.set_vertice(vertice);
		database->push_graph(graph);	

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

		if (!split(projection_map)) {
			fprintf(stderr, "split thread error! size %zu, thread_size %u\n", 
					projection_map.size(), THREAD_NUM);
			return GSPAN_ERROR;
		}

		multi_subgraph_init();

		size_t prev_idx = 0;
		for (size_t i = 0; i < THREAD_NUM; ++i) {
			_m_output[i]->set_start_idx(prev_idx);
			_m_output[i]->print();

			prev_idx += _m_output[i]->size();
		}	

		return GSPAN_SUCCESS;
	}


	GSpanReturnCode GSpan::subgraph_mining(Projection& projection, size_t tid, size_t prev_id)
	{
		uint32_t nsupport = counting_support(projection);
		if (nsupport < _m_nsupport) 
			return GSPAN_SUCCESS;

		//debug
		bool flag = is_min(tid);
		if (!flag)
			return GSPAN_SUCCESS;

		report(nsupport, tid, projection, prev_id);
		prev_id = _m_output[tid]->size() - 1;
#ifdef DEBUG
		printf("subgraph_mining here0\n");
#endif
		std::vector<uint32_t> right_most_path;
		build_right_most_path(_m_dfs_codes[tid], right_most_path);

		uint32_t min_label = _m_dfs_codes[tid][0].from_label;
		//enumerate backward paths and forward paths by different rules
		//
#ifdef DEBUG
		printf("subgraph_mining here1\n");
#endif
		ProjectionMapBackward projection_map_backward;
		ProjectionMapForward projection_map_forward;
		enumerate(projection, right_most_path, projection_map_backward, 
				projection_map_forward, min_label, tid);
#ifdef DEBUG
		printf("subgraph_mining here2\n");
#endif

		//recursive mining according to the paper, 
		//which should be in the order of the first backward to the last backward,
		//and then followed by the last forward to the first forward
		
		for (ProjectionMapBackward::iterator it = projection_map_backward.begin();
				it != projection_map_backward.end(); ++it) {
			_m_dfs_codes[tid].push_back((struct dfs_code_t)
					{(it->first).from, (it->first).to, (it->first).from_label, (it->first).edge_label, (it->first).to_label});
			subgraph_mining(it->second, tid, prev_id);
			_m_dfs_codes[tid].pop_back();
		}	

		for (ProjectionMapForward::reverse_iterator it = projection_map_forward.rbegin();
				it != projection_map_forward.rend(); ++it) {
			_m_dfs_codes[tid].push_back((struct dfs_code_t) 
					{(it->first).from, (it->first).to, (it->first).from_label, (it->first).edge_label, (it->first).to_label});
			subgraph_mining(it->second, tid, prev_id);
			_m_dfs_codes[tid].pop_back();
		}
		
		return GSPAN_SUCCESS;
	}

	void GSpan::report(uint32_t label, uint32_t nsupport, size_t tid)
	{
		printf("t # %u * %u\n", _m_report_idx, nsupport);
		++_m_report_idx;

		printf("v 0 %u\n", label);
		printf("\n");
	}

	void GSpan::report(uint32_t nsupport, size_t tid)
	{
		Graph graph;
		build_graph(graph, tid);

		std::stringstream ss;
		for (size_t i = 0; i < graph.size(); ++i) {
			const struct vertex_t& vertex = graph.get_vertex(i);
			ss << "v " << vertex.id << " " << vertex.label << "\n";
		}

		for (size_t i = 0; i < _m_dfs_codes[tid].size(); ++i) {
			ss << "e " << _m_dfs_codes[tid][i].from << " " << _m_dfs_codes[tid][i].to
				<<" " <<_m_dfs_codes[tid][i].edge_label << "\n";
		}

		ss << "\n";

		_m_output[tid]->push_back(ss.str(), nsupport);
	}

	void GSpan::report(uint32_t nsupport, size_t tid, Projection& projection, int32_t prev_id)
	{
		Graph graph;
		build_graph(graph, tid);

		std::stringstream ss;
		
		for (size_t i = 0; i < graph.size(); ++i) {
			const struct vertex_t& vertex = graph.get_vertex(i);
			ss << "v " << vertex.id << " " << vertex.label << std::endl;
		}

		for (size_t i = 0; i < _m_dfs_codes[tid].size(); ++i) {
			ss << "e " << _m_dfs_codes[tid][i].from << " " << _m_dfs_codes[tid][i].to
				<<" " <<_m_dfs_codes[tid][i].edge_label << std::endl;
		}

		ss << "x: ";
		uint32_t prev = 0;
		for (size_t i = 0; i < projection.size(); ++i) {
			if (i == 0 || projection[i].id != prev) {
				prev = projection[i].id;
				ss << prev << " ";
			}
		}
		ss << std::endl;

		_m_output[tid]->push_back(ss.str(), nsupport, _m_output[tid]->size(), prev_id);

		ss << std::endl;
	}
} // namespace gspan
