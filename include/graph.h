#ifndef INCLUDE_GRAPH_H_
#define INCLUDE_GRAPH_H_

#include <common.h>
#include <vector>

namespace gbolt {

// Direct edge structure
struct edge_t {
  edge_t(int from, int label, int to, int id) :
    from(from), label(label), to(to), id(id) {}

  int from;
  int label;
  int to;
  int id;
};
typedef vector<const struct edge_t *> Edges;

// dfs projection links
struct prev_dfs_t {
  prev_dfs_t(int id, const struct edge_t *edge, const struct prev_dfs_t *prev) :
    id(id), edge(edge), prev(prev) {}

  int id;
  const struct edge_t *edge;
  const struct prev_dfs_t *prev;
};
typedef vector<struct prev_dfs_t> Projection;

// dfs codes forward and backward compare
struct dfs_code_t {
  dfs_code_t(int from, int to, int from_label, int edge_label, int to_label) :
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

  int from;
  int to;
  int from_label;
  int edge_label;
  int to_label;
};
typedef vector<const struct dfs_code_t *> DfsCodes;

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
  vertex_t(int id, int label) : id(id), label(label) {}

  int id;
  int label;
  vector<struct edge_t> edges;
};
typedef vector<struct vertex_t> Vertice;

class Graph {
 public:
  Graph() : id_(0), nedges_(0) {}

  int size() const {
    return vertice_.size();
  }

  void resize(int s) {
    vertice_.resize(s);
  }

  void set_id(int id) {
    id_ = id;
  }

  int get_id() const {
    return id_;
  }

  void set_nedges(int nedges) {
    nedges_ = nedges;
  }

  int get_nedges() const {
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

  struct vertex_t get_vertex(int index) {
    return vertice_[index];
  }

  const struct vertex_t get_vertex(int index) const {
    return vertice_[index];
  }

  struct vertex_t *get_p_vertex(int index) {
    return &vertice_[index];
  }

  const struct vertex_t *get_p_vertex(int index) const {
    return &vertice_[index];
  }

  void clear() {
    id_ = 0;
    nedges_ = 0;
    vertice_.clear();
  }

 private:
  int id_;
  int nedges_;
  Vertice vertice_;
};

}  // namespace gbolt

#endif  // INCLUDE_GRAPH_H_
