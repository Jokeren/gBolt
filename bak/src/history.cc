#include <algorithm>
#include "history.h"

namespace gspan {
	void History::build()
	{
		_m_has_edges.resize(_m_graph.get_nedges() + 1);
		_m_has_vertice.resize(_m_graph.size() + 1);

		const struct pre_dfs_t *start = _m_start;

		while (start != NULL) {
			_m_edges.push_back(start->edge);

			_m_has_edges[start->edge->id] = true;

			_m_has_vertice[start->edge->from] = true;

			_m_has_vertice[start->edge->to] = true;

			start = start->prev;
		}

		//important
		std::reverse(_m_edges.begin(), _m_edges.end());
	}
}//namespace gspan
