#ifndef HISTORY_H
#define HISTORY_H
#include "graph.h"

namespace gspan{
	class History {
		public:
			History(const struct pre_dfs_t *pre_dfs, const Graph& graph) : 
				_m_start(pre_dfs), _m_graph(graph) {};

			void build();

			bool has_edges(size_t idx) const {
				return _m_has_edges[idx];
			}
		
			bool has_vertice(size_t idx) const {
				return _m_has_vertice[idx];
			}

			const struct edge_t* get_edge(size_t idx) const{
				return _m_edges[idx];
			}

		private:
			const struct pre_dfs_t *_m_start;
			const Graph& _m_graph;
			Edges _m_edges;
			std::vector<bool> _m_has_edges;	
			std::vector<bool> _m_has_vertice;	
	};
}//namespace gspan

#endif
