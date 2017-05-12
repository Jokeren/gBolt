#ifndef INCLUDE_GRAPH_H_
#define INCLUDE_GRAPH_H_

#include <common.h>
#include <vector>

namespace gspan {

// Direct edge structure
struct edge_t {
  edge_t(size_t from, size_t label, size_t to, size_t id) :
    from(from), label(label), to(to), id(id) {}

  size_t from;
  size_t label;
  size_t to;
  size_t id;
};
typedef vector<const struct edge_t *> Edges;

// dfs projection links
struct prev_dfs_t {
  prev_dfs_t(size_t id, const struct edge_t *edge, const struct prev_dfs_t *prev) :
    id(id), edge(edge), prev(prev) {}

  size_t id;
  const struct edge_t *edge;
  const struct prev_dfs_t *prev;
};
typedef vector<struct prev_dfs_t> Projection;

// dfs codes forward and backward compare
struct dfs_code_t {
  dfs_code_t(size_t from, size_t to, size_t from_label, size_t edge_label, size_t to_label) :
    from(from), to(to),
    from_label(from_label), edge_label(edge_label), to_label(to_label) {}

  dfs_code_t(const dfs_code_t &other) {
    this->from = other.from;
    this->to = other.to;
    this->from_label = other.from_label;
    this->edge_label = other.edge_label;
    this->to_label = other.to_label;
  }

  bool operator != (const struct dfs_code_t &t) const {
    return (from != t.from) || (to != t.to) ||
      (from_label != t.from_label) || (edge_label != t.edge_label) ||
      (to_label != t.to_label);
  }

  size_t from;
  size_t to;
  size_t from_label;
  size_t edge_label;
  size_t to_label;
};
typedef vector<struct dfs_code_t> DfsCodes;

struct dfs_code_project_compare_t {
  bool operator() (const struct dfs_code_t &first, const struct dfs_code_t &second) {
    if (first.from_label != second.from_label) {
      return first.from_label < second.from_label;
    } else {
      if (first.edge_label != second.edge_label) {
        return first.edge_label < second.edge_label;
      } else {
        return first.to_label < second.to_label;
      }
    }
  }
};

struct dfs_code_backward_compare_t {
  bool operator() (const struct dfs_code_t &first, const struct dfs_code_t &second) {
    if (first.to != second.to) {
      return first.to < second.to;
    } else {
      return first.edge_label < second.edge_label;
    }
  }
};

struct dfs_code_forward_compare_t {
  bool operator() (const struct dfs_code_t &first, const struct dfs_code_t &second) {
    if (first.from != second.from) {
      return first.from > second.from;
    } else {
      if (first.edge_label != second.edge_label) {
        return first.edge_label < second.edge_label;
      } else {
        return first.to_label < second.to_label;
      }
    }
  }
};

struct vertex_t {
  vertex_t() {}
  vertex_t(size_t id, size_t label) : id(id), label(label) {}

  size_t id;
  size_t label;
  vector<struct edge_t> edges;
};
typedef vector<struct vertex_t> Vertice;

class Graph {
 public:
  Graph() : id_(0), nedges_(0) {}

  size_t size() const {
    return vertice_.size();
  }

  void resize(size_t s) {
    vertice_.resize(s);
  }

  void set_id(size_t id) {
    id_ = id;
  }

  size_t get_id() const {
    return id_;
  }

  void set_nedges(size_t nedges) {
    nedges_ = nedges;
  }

  size_t get_nedges() const {
    return nedges_;
  }

  void set_vertice(const Vertice& vertice) {
    vertice_ = vertice;
  }

  Vertice *get_p_vertice() {
    return &vertice_;
  }

  const Vertice *get_p_vertice() const {
    return &vertice_;
  }

  struct vertex_t get_vertex(size_t index) {
    return vertice_[index];
  }

  const struct vertex_t get_vertex(size_t index) const {
    return vertice_[index];
  }

  struct vertex_t *get_p_vertex(size_t index) {
    return &vertice_[index];
  }

  const struct vertex_t *get_p_vertex(size_t index) const {
    return &vertice_[index];
  }

  void clear() {
    id_ = 0;
    nedges_ = 0;
    vertice_.clear();
  }

 private:
  size_t id_;
  size_t nedges_;
  Vertice vertice_;
};
}  // namespace gspan

#endif  // INCLUDE_GRAPH_H_
