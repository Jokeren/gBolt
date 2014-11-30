#include "gspan.h"
#include "history.h"

namespace gspan {
	void GSpan::build_right_most_path(const DfsCodes& dfs_codes, 
			std::vector<uint32_t>& right_most_path)
	{
		int32_t prev_id = -1;

		for (size_t i = dfs_codes.size(); i > 0; --i) {
			if (dfs_codes[i - 1].from < dfs_codes[i - 1].to && 
					(right_most_path.empty() || prev_id == dfs_codes[i - 1].to)) {
				prev_id = dfs_codes[i - 1].from;
				right_most_path.push_back(i - 1);
			}
		}
	}

	void GSpan::enumerate(const Projection& projection, const std::vector<uint32_t>& right_most_path,
			ProjectionMapBackward& projection_map_backward, ProjectionMapForward& projection_map_forward, uint32_t min_label)
	{
		for (size_t i = 0; i < projection.size(); ++i) {
			History *p_history = new History(&(projection[i]), _m_graphs[projection[i].id]);
			p_history->build();

			get_backward(&(projection[i]), right_most_path, p_history, projection_map_backward);

			get_first_forward(&(projection[i]), right_most_path, p_history, projection_map_forward, min_label);

			get_other_forward(&(projection[i]), right_most_path, p_history, projection_map_forward, min_label);

			delete(p_history);
		}
		
#ifdef DEBUG
			struct dfs_code_t dfs_code;
			dfs_code.from = 1;
			dfs_code.to = 2;
			dfs_code.from_label = 0;
			dfs_code.to_label = 1;
			dfs_code.edge_label = 0;
			int32_t prev_id = -1;
			uint32_t size = 0;
			
			for (size_t i = 0; i < projection_map_forward[dfs_code].size(); ++i) {
				if (projection_map_forward[dfs_code][i].id != prev_id) {
					prev_id = projection_map_forward[dfs_code][i].id;
					size++;
				}

				printf("%u\n", size);
				printf("graph_id %u\n", projection_map_forward[dfs_code][i].id);

				const struct pre_dfs_t *p = &(projection_map_forward[dfs_code][i]);
				printf("%u %u %u\n", p->edge->from, p->edge->to, p->edge->label);

				p = p->prev;
				while (p) {
					printf("%u %u %u\n", p->edge->from, p->edge->to, p->edge->label);
					p = p->prev;
				}
				printf("\n");
			}
#endif

	}

	bool GSpan::get_forward_init(const struct vertex_t& vertex, const Graph& graph, Edges& edges)
	{
		for (size_t i = 0; i < vertex.edges.size(); ++i) {
			uint32_t to = vertex.edges[i].to;
			const struct vertex_t& next_vertex = graph.get_vertex(to);
			//partial pruning, if the first label is greater than the second label, then there must be
			//another graph whose second label is greater than the first label.
			if (vertex.label <= next_vertex.label) {
				edges.push_back(&(vertex.edges[i]));
			}
		}

		return !edges.empty();
	}

	void GSpan::get_backward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
			ProjectionMapBackward& projection_map_backward)
	{
		const struct edge_t *last_edge = p_history->get_edge(right_most_path[0]);

		const Graph& graph = _m_graphs[prev_dfs->id];
		const struct vertex_t& last_node = graph.get_vertex(last_edge->to);

		for (size_t i = right_most_path.size(); i > 1; --i) {
			const struct edge_t *edge = p_history->get_edge(right_most_path[i - 1]);

			for (size_t j = 0; j < (last_node.edges).size(); ++j) {
				if (p_history->has_edges((last_node.edges[j]).id))
					continue;

				if (!p_history->has_vertice((last_node.edges[j]).to)) 
					continue;

				const struct vertex_t& from_node = graph.get_vertex(edge->from);
				const struct vertex_t& to_node = graph.get_vertex(edge->to);
				if (last_node.edges[j].to == edge->from &&
						(last_node.edges[j].label > edge->label ||
						 (last_node.edges[j].label == edge->label &&
						  last_node.label >= to_node.label))) {
					uint32_t from_id = _m_dfs_codes[right_most_path[0]].to;
					uint32_t to_id = _m_dfs_codes[right_most_path[i - 1]].from; 
					struct dfs_code_t dfs_code;

					dfs_code.from = from_id;
					dfs_code.to = to_id;
					dfs_code.from_label = last_node.label;
					dfs_code.to_label = from_node.label;
					dfs_code.edge_label = (last_node.edges[j]).label;

					struct pre_dfs_t pre_dfs;
					pre_dfs.id = graph.get_id();
					pre_dfs.edge = &(last_node.edges[j]); 
					pre_dfs.prev = prev_dfs;
					projection_map_backward[dfs_code].push_back(pre_dfs);		
				}
			}
		}
	}

	void GSpan::get_first_forward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
			ProjectionMapForward& projection_map_forward, uint32_t min_label)
	{
		const struct edge_t *last_edge = p_history->get_edge(right_most_path[0]);

		const Graph& graph = _m_graphs[prev_dfs->id];
		const struct vertex_t& last_node = graph.get_vertex(last_edge->to);

		for (size_t i = 0; i < (last_node.edges).size(); ++i) {
			const struct edge_t *edge = &(last_node.edges[i]);
			const struct vertex_t& to_node = graph.get_vertex(edge->to);

			//partial pruning: if this label is less than the minimum label, then there
			//should exist another lexicographical order which renders the same letters, but
			//in the asecending order

			//question: can we perform the same partial pruning as other extending methods?
			//no, we cannot, for this time, the extending id is greater the the last node
			if (p_history->has_vertice(edge->to) || to_node.label < min_label)
				continue;

			uint32_t to_id = _m_dfs_codes[right_most_path[0]].to;
			struct dfs_code_t dfs_code;

			dfs_code.from = to_id;
			dfs_code.to = to_id + 1;
			dfs_code.from_label = last_node.label;
			dfs_code.to_label = to_node.label;
			dfs_code.edge_label = edge->label;

			struct pre_dfs_t pre_dfs;
			pre_dfs.id = graph.get_id();
			pre_dfs.edge = edge; 
			pre_dfs.prev = prev_dfs;

#ifdef	DEBUG
			printf("size %zu\n", projection_map_forward[dfs_code].size());
#endif
			projection_map_forward[dfs_code].push_back(pre_dfs);		
		}
	}

	void GSpan::get_other_forward(const struct pre_dfs_t *prev_dfs, const std::vector<uint32_t>& right_most_path, const History *p_history,
			ProjectionMapForward& projection_map_forward, uint32_t min_label)
	{
		const Graph& graph = _m_graphs[prev_dfs->id];

		for (size_t i = 0; i < right_most_path.size(); ++i) {
			const struct edge_t *cur_edge = p_history->get_edge(right_most_path[i]);
			const struct vertex_t& cur_node = graph.get_vertex(cur_edge->from);
			const struct vertex_t& cur_to = graph.get_vertex(cur_edge->to);

			for (size_t j = 0; j < cur_node.edges.size(); ++j) {
				const struct vertex_t& to_node = graph.get_vertex(cur_node.edges[j].to);

				//another partial pruning, guarantees that extending label is greater 
				//than the minimum one
				if (to_node.id == cur_to.id || 
						p_history->has_vertice(to_node.id) || to_node.label < min_label)
					continue;

				if (cur_edge->label < cur_node.edges[j].label ||
						(cur_edge->label == cur_node.edges[j].label && 
						 cur_to.label <= to_node.label)) {

					uint32_t from_id = _m_dfs_codes[right_most_path[i]].from;
					uint32_t to_id = _m_dfs_codes[right_most_path[0]].to;
					struct dfs_code_t dfs_code;

					dfs_code.from = from_id;
					dfs_code.to = to_id + 1;
					dfs_code.from_label = cur_node.label;
					dfs_code.edge_label = cur_node.edges[j].label;
					dfs_code.to_label = to_node.label;

					struct pre_dfs_t pre_dfs;
					pre_dfs.id = graph.get_id();
					pre_dfs.edge = &(cur_node.edges[j]); 
					pre_dfs.prev = prev_dfs;
					projection_map_forward[dfs_code].push_back(pre_dfs);		
				}
			}
		}
	}
}//namespace gspan
