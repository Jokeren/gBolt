#include <history.h>
#include <graph.h>
#include <algorithm>

namespace gbolt {

// Reduce push back memory costs
// while (cur_dfs != NULL) {
//   edges_.push_back(cur_dfs->edge);
//   has_edges_[cur_dfs->edge->id] = true;
//   has_vertice_[cur_dfs->edge->from] = true;
//   has_vertice_[cur_dfs->edge->to] = true;
//   cur_dfs = cur_dfs->prev;
// }
void History::build(const prev_dfs_t &start, const Graph &graph) {
  auto *cur_dfs = &start;
  memset(has_edges_, false, sizeof(bool) * graph.get_nedges() + 1);
  memset(has_vertice_, false, sizeof(bool) * graph.size() + 1);
  edge_size_ = 0;

  // Reduce push back memory costs
  while (cur_dfs != NULL) {
    edges_[edge_size_++] = cur_dfs->edge;
    has_edges_[cur_dfs->edge->id] = true;
    has_vertice_[cur_dfs->edge->from] = true;
    has_vertice_[cur_dfs->edge->to] = true;
    cur_dfs = cur_dfs->prev;
  }
}

void History::build_edges(const prev_dfs_t &start, const Graph &graph) {
  auto *cur_dfs = &start;
  memset(has_edges_, false, sizeof(bool) * graph.get_nedges() + 1);
  edge_size_ = 0;

  // Reduce push back memory costs
  while (cur_dfs != NULL) {
    edges_[edge_size_++] = cur_dfs->edge;
    has_edges_[cur_dfs->edge->id] = true;
    cur_dfs = cur_dfs->prev;
  }
}

void History::build_edges_min(const MinProjection &projection, const Graph &graph, int start) {
  memset(has_edges_, false, sizeof(bool) * graph.get_nedges() + 1);
  edge_size_ = 0;

  // Reduce push back memory costs
  while (start != -1) {
    auto &cur_dfs = projection[start];
    edges_[edge_size_++] = cur_dfs.edge;
    has_edges_[cur_dfs.edge->id] = true;
    start = cur_dfs.prev;
  }
}

void History::build_vertice(const prev_dfs_t &start, const Graph &graph) {
  auto *cur_dfs = &start;
  memset(has_vertice_, false, sizeof(bool) * graph.size() + 1);
  edge_size_ = 0;

  // Reduce push back memory costs
  while (cur_dfs != NULL) {
    edges_[edge_size_++] = cur_dfs->edge;
    has_vertice_[cur_dfs->edge->from] = true;
    has_vertice_[cur_dfs->edge->to] = true;
    cur_dfs = cur_dfs->prev;
  }
}

void History::build_vertice_min(const MinProjection &projection, const Graph &graph, int start) {
  memset(has_vertice_, false, sizeof(bool) * graph.size() + 1);
  edge_size_ = 0;

  // Reduce push back memory costs
  while (start != -1) {
    auto &cur_dfs = projection[start];
    edges_[edge_size_++] = cur_dfs.edge;
    has_vertice_[cur_dfs.edge->from] = true;
    has_vertice_[cur_dfs.edge->to] = true;
    start = cur_dfs.prev; 
  }
}

}  // namespace gbolt
