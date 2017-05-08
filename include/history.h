#ifndef HISTORY_H_
#define HISTORY_H_

#include <common.h>
#include <graph.h>

namespace gspan{

class History {
 public:
  History() {}

  void build(const struct prev_dfs_t &start, const Graph &graph);

  bool has_edges(size_t index) const {
    return has_edges_[index];
  }

  bool has_vertice(size_t index) const {
    return has_vertice_[index];
  }

  const struct edge_t *get_p_edge(size_t index) const{
    return edges_[index];
  }

  ~History() {
    delete has_edges_;
    delete has_vertice_;
  }

 private:
  Edges edges_;
  bool *has_edges_;
  bool *has_vertice_;
};

}  // namespace gspan

#endif  // HISTORY_H_

