#include <history.h>
#include <graph.h>
#include <algorithm>

namespace gspan {

void History::build(const struct prev_dfs_t &start, const Graph &graph) {
  const struct prev_dfs_t *cur_dfs = &start;
  size_t edge_size = 0;
  has_edges_ = new bool[graph.get_nedges() + 1]();
  has_vertice_ = new bool[graph.size() + 1]();

  // Reduce push back memory costs
  while (cur_dfs != NULL) {
    cur_dfs = cur_dfs->prev;
    ++edge_size;
  }
  cur_dfs = &start;
  edges_.reserve(edge_size);

  for (size_t i = 0; i < edge_size; ++i) {
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
