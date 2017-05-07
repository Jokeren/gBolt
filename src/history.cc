#include <history.h>
#include <graph.h>
#include <algorithm>

namespace gspan {

void History::build(const struct prev_dfs_t &start, const Graph &graph) {
  const struct prev_dfs_t *cur_dfs = &start;
  has_edges_.resize(graph.get_nedges() + 1);
  has_vertice_.resize(graph.size() + 1);

  while (cur_dfs != NULL) {
    edges_.push_back(cur_dfs->edge);
    has_edges_[cur_dfs->edge->id] = true;
    has_vertice_[cur_dfs->edge->from] = true;
    has_vertice_[cur_dfs->edge->to] = true;
    cur_dfs = cur_dfs->prev;
  }

  // Reverse edges list, very important
  std::reverse(edges_.begin(), edges_.end());
}

}  // namespace gspan
