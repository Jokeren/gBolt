#include <algorithm>
#include "gspan.h"
#include "history.h"

namespace gspan {
	uint32_t GSpan::counting_support(const Projection& projection)
	{
		int32_t prev_id = -1;
		uint32_t size = 0;

		for (size_t i = 0; i < projection.size(); ++i) {
			if (prev_id != projection[i].id) {
				prev_id = projection[i].id;
				++size;
			} else {
				//do nothing
			}
		}

		return size;
	}

	void GSpan::build_graph(Graph& graph)
	{
		uint32_t edge_id = 0;
		Vertice vertice;

		for (size_t i = 0; i < _m_dfs_codes.size(); ++i) {
			//debug
			//printf("%u %u here0\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to);
			vertice.resize(std::max(static_cast<size_t>(_m_dfs_codes[i].from + 1), 
						vertice.size()));
			vertice.resize(std::max(static_cast<size_t>(_m_dfs_codes[i].to + 1),
					   	vertice.size()));

			//debug
			//printf("%u %u here1\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to);

			vertice[_m_dfs_codes[i].from].label = _m_dfs_codes[i].from_label;
			vertice[_m_dfs_codes[i].from].id = _m_dfs_codes[i].from;
			vertice[_m_dfs_codes[i].to].label = _m_dfs_codes[i].to_label;
			vertice[_m_dfs_codes[i].to].id = _m_dfs_codes[i].to;

			//debug
			//printf("%u %u here2\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to);
			struct edge_t edge;
			edge.from = _m_dfs_codes[i].from;
			edge.to = _m_dfs_codes[i].to;
			edge.label = _m_dfs_codes[i].edge_label;
			edge.id = edge_id;
			++edge_id;
			//debug
			//printf("%u %u here3\n", edge.from, edge.to);

			vertice[edge.from].edges.push_back(edge);

			//debug
			//printf("%u %u here4\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to);
			//second edge
			edge.from = _m_dfs_codes[i].to;
			edge.to = _m_dfs_codes[i].from;
			//please change it, very tricky
			vertice[edge.from].edges.push_back(edge);
			//debug
			//printf("%u %u here5\n", _m_dfs_codes[i].from, _m_dfs_codes[i].to);
		}

		graph.set_nedges(edge_id);
		graph.set_vertice(vertice);
	}

	bool GSpan::is_min()
	{
		//debug
#ifdef DEBUG
		printf("is_min size %zu\n", _m_dfs_codes.size());
#endif

		if (_m_dfs_codes.size() == 1)
			return true;

		_m_min_graph.clear();

#ifdef DEBUG
		printf("is_min here0\n");
#endif
		build_graph(_m_min_graph);

		_m_min_dfs_codes.clear();
		ProjectionMap projection_map;

		for (size_t j = 0; j < _m_min_graph.size(); ++j) {
			const struct vertex_t& vertex = _m_min_graph.get_vertex(j);	
			Edges edges;	

			if (get_forward_init(vertex, _m_min_graph, edges)) {
				for (size_t k = 0; k < edges.size(); ++k) {
					struct dfs_code_t dfs_code;
					//push dfs code according to the same edge label
					const struct vertex_t& vertex_from = _m_min_graph.get_vertex(edges[k]->from);
					dfs_code.from_label = vertex_from.label;
					dfs_code.from = 0;

					dfs_code.edge_label = edges[k]->label;

					const struct vertex_t& vertex_to = _m_min_graph.get_vertex(edges[k]->to);
					dfs_code.to = 1;
					dfs_code.to_label = vertex_to.label;

					//push_back all the graphs
					struct pre_dfs_t pre_dfs;
					pre_dfs.id = 0;
					pre_dfs.edge = edges[k];
					pre_dfs.prev = NULL;

					projection_map[dfs_code].push_back(pre_dfs);
				}
			}
		}


		ProjectionMap::iterator it = projection_map.begin();
		_m_min_dfs_codes.push_back((struct dfs_code_t) {0, 1, 
				(it->first).from_label, (it->first).edge_label, (it->first).to_label});

		if (_m_dfs_codes[_m_min_dfs_codes.size() - 1] != _m_min_dfs_codes[_m_min_dfs_codes.size() - 1])
			return false;

		return projection_min(it->second);
	}

	bool GSpan::judge_backward(const std::vector<uint32_t>& right_most_path, const Projection& projection, 
			ProjectionMapBackward& projection_map_backward, uint32_t min_label)
	{
		// i > 1, because it cannot reach the path itself
		for (size_t i = right_most_path.size(); i > 1; --i) {
			for (size_t j = 0; j < projection.size(); ++j) {
				History *p_history = new History(&(projection[j]), _m_min_graph);
				p_history->build();

				const struct edge_t *last_edge = p_history->get_edge(right_most_path[0]);
				const struct vertex_t& last_node = _m_min_graph.get_vertex(last_edge->to);

				const struct edge_t *edge = p_history->get_edge(right_most_path[i - 1]);
				const struct vertex_t& to_node = _m_min_graph.get_vertex(edge->to);
				const struct vertex_t& from_node = _m_min_graph.get_vertex(edge->from);

				for (size_t k = 0; k < last_node.edges.size(); ++k) {
					if (p_history->has_edges(last_node.edges[k].id))
						continue;

					if (!p_history->has_vertice(last_node.edges[k].to))
						continue;

					if (last_node.edges[k].to == edge->from && 
							(last_node.edges[k].label > edge->label ||
							 (last_node.edges[k].label == edge->label &&
							  last_node.label > to_node.label))) {
						uint32_t from_id = _m_min_dfs_codes[right_most_path[0]].to;
						uint32_t to_id = _m_min_dfs_codes[right_most_path[i - 1]].from; 

						struct dfs_code_t dfs_code;
						dfs_code.from = from_id;
						dfs_code.to = to_id;
						dfs_code.from_label = last_node.label;
						dfs_code.to_label = from_node.label;
						dfs_code.edge_label = (last_node.edges[k]).label;

						struct pre_dfs_t pre_dfs;
						pre_dfs.id = 0;
						pre_dfs.edge = &(last_node.edges[k]); 
						pre_dfs.prev = &(projection[j]);
						projection_map_backward[dfs_code].push_back(pre_dfs);		
					}
				}
			}

			if (projection_map_backward.size() != 0) 
				return true;
		}
		return false;
	}


	bool GSpan::judge_forward(const std::vector<uint32_t>& right_most_path, const Projection& projection, 
			ProjectionMapForward& projection_map_forward, uint32_t min_label)
	{
		for (size_t i = 0; i < projection.size(); ++i) {
			History *p_history = new History(&(projection[i]), _m_min_graph);
			p_history->build();

			const struct edge_t *last_edge = p_history->get_edge(right_most_path[0]);
			const struct vertex_t& last_node = _m_min_graph.get_vertex(last_edge->to);

			for (size_t j = 0; j < (last_node.edges).size(); ++j) {
				const struct edge_t *edge = &(last_node.edges[j]);
				const struct vertex_t& to_node = _m_min_graph.get_vertex(edge->to);

				//partial pruning: if this label is less than the minimum label, then there
				//should exist another lexicographical order which renders the same letters, but
				//in the asecending order

				//question: can we perform the same partial pruning as other extending methods?
				//no, we cannot, for this time, the extending id is greater the the last node
				if (p_history->has_vertice(edge->to) || to_node.label < min_label)
					continue;

				uint32_t to_id = _m_min_dfs_codes[right_most_path[0]].to;
				struct dfs_code_t dfs_code;
				dfs_code.from = to_id;
				dfs_code.to = to_id + 1;
				dfs_code.from_label = last_node.label;
				dfs_code.to_label = to_node.label;
				dfs_code.edge_label = edge->label;

				struct pre_dfs_t pre_dfs;
				pre_dfs.id = 0;
				pre_dfs.edge = edge; 
				pre_dfs.prev = &(projection[i]);
				projection_map_forward[dfs_code].push_back(pre_dfs);		
			}
		}

		if (projection_map_forward.size() == 0) {
			for (size_t i = 0; i < right_most_path.size(); ++i) {
				for (size_t j = 0; j < projection.size(); ++j) {
					History *p_history = new History(&(projection[j]), _m_min_graph);
					p_history->build();

					const struct edge_t *cur_edge = p_history->get_edge(right_most_path[i]);
					const struct vertex_t& cur_node = _m_min_graph.get_vertex(cur_edge->from);
					const struct vertex_t& cur_to = _m_min_graph.get_vertex(cur_edge->to);

					for (size_t k = 0; k < cur_node.edges.size(); ++k) {
						const struct vertex_t& to_node = _m_min_graph.get_vertex(cur_node.edges[k].to);

						//another partial pruning, guarantees that extending label is greater 
						//than the minimum one
						if (cur_edge->to == to_node.id ||
								p_history->has_vertice(to_node.id) || 
								to_node.label < min_label)
							continue;

						if (cur_edge->label < cur_node.edges[k].label ||
								(cur_edge->label == cur_node.edges[k].label && 
								 cur_to.label <= to_node.label)) {
							uint32_t from_id = _m_min_dfs_codes[right_most_path[i]].from;
							uint32_t to_id = _m_min_dfs_codes[right_most_path[0]].to;
							struct dfs_code_t dfs_code;

							dfs_code.from = from_id;
							dfs_code.to = to_id + 1;
							dfs_code.from_label = cur_node.label;
							dfs_code.edge_label = cur_node.edges[k].label;
							dfs_code.to_label = to_node.label;

							struct pre_dfs_t pre_dfs;
							pre_dfs.id = 0;
							pre_dfs.edge = &(cur_node.edges[k]); 
							pre_dfs.prev = &(projection[j]);
							projection_map_forward[dfs_code].push_back(pre_dfs);		
						}
					}
				}

				if (projection_map_forward.size() != 0)
					break;
			}
		}

		if (projection_map_forward.size() != 0)
			return true;
		else
			return false;
	}

	bool GSpan::projection_min(const Projection& projection)
	{
		std::vector<uint32_t> right_most_path;
		build_right_most_path(_m_min_dfs_codes, right_most_path);


		ProjectionMapBackward projection_map_backward;

		uint32_t min_label = _m_min_dfs_codes[0].from_label;

		if (judge_backward(right_most_path, projection, projection_map_backward, min_label)) {
			ProjectionMapBackward::iterator it = projection_map_backward.begin();	

			_m_min_dfs_codes.push_back((struct dfs_code_t) {(it->first).from, (it->first).to, 
					(it->first).from_label, (it->first).edge_label, (it->first).to_label});

			if (_m_dfs_codes[_m_min_dfs_codes.size() - 1] != _m_min_dfs_codes[_m_min_dfs_codes.size() - 1])
				return false;

			return projection_min(it->second);
		}

		ProjectionMapForward projection_map_forward;

		if (judge_forward(right_most_path, projection, projection_map_forward, min_label)) {
			ProjectionMapForward::iterator it = projection_map_forward.begin();

			_m_min_dfs_codes.push_back((struct dfs_code_t) {(it->first).from, (it->first).to, 
					(it->first).from_label, (it->first).edge_label, (it->first).to_label});

			if (_m_dfs_codes[_m_min_dfs_codes.size() - 1] != _m_min_dfs_codes[_m_min_dfs_codes.size() - 1])
				return false;

			return projection_min(it->second);
		}

		return true;
	}
}
