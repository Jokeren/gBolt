#ifndef GSPAN_H_
#define GSPAN_H_

#include <common.h>
#include <graph.h>
#include <history.h>
#include <output.h>

namespace gspan {

struct gspan_instance_t {
  Graph *min_graph;
  DfsCodes *min_dfs_codes;
  History *history;
  Output *output;

  ~gspan_instance_t() {
    delete this->min_graph;
    delete this->min_dfs_codes;
    delete this->history;
    delete this->output;
  }
};

class GSpan {
 public:
  GSpan(const string &output_file, double support) :
    output_file_(output_file), support_(support) {};

  void execute();

  void save(bool output_parent = false, bool output_pattern = false);

  ~GSpan() {
    delete[] gspan_instances_;
  }

 private:
  typedef map<struct dfs_code_t, Projection, struct dfs_code_project_compare_t> ProjectionMap;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_backward_compare_t> ProjectionMapBackward;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_forward_compare_t> ProjectionMapForward;

 private:
  // Mine
  void init_instances(const vector<Graph> &graphs);

  void project(const vector<Graph> &graphs);

  void find_frequent_nodes(const vector<Graph> &graphs);

  void mine_subgraph(
    const vector<Graph> &graphs,
    const DfsCodes &dfs_codes,
    const Projection &projection,
    size_t prev_nsupport,
    int prev_id);

  // Extend
  void build_right_most_path(const DfsCodes &dfs_codes, vector<size_t> &right_most_path);

  void enumerate(
    const vector<Graph> &graphs,
    const DfsCodes &dfs_codes,
    const Projection &projection,
    const vector<size_t> &right_most_path,
    size_t min_label,
    ProjectionMapBackward &projection_map_backward,
    ProjectionMapForward &projection_map_forward);

  bool get_forward_init(
    const struct vertex_t &vertex,
    const Graph &graph,
    Edges &edges);

  void get_first_forward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const vector<size_t> &right_most_path,
    size_t min_label,
    ProjectionMapForward& projection_map_forward);

  void get_other_forward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const vector<size_t> &right_most_path,
    size_t min_label,
    ProjectionMapForward& projection_map_forward);

  void get_backward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const vector<size_t> &right_most_path,
    ProjectionMapBackward& projection_map_backward);

  // Count
  size_t count_support(const Projection &projection);

  void build_graph(const DfsCodes &dfs_codes, Graph &graph);

  bool is_min(const DfsCodes &dfs_codes);

  bool is_projection_min(const DfsCodes &dfs_codes, const Projection &projection);

  bool judge_backward(
    const vector<size_t> &right_most_path,
    const Projection &projection,
    size_t min_label,
    ProjectionMapBackward &projection_map_backward);

  bool judge_forward(
    const vector<size_t> &right_most_path,
    const Projection &projection,
    size_t min_label,
    ProjectionMapForward &projection_map_forward);

  // Report
  void report(const DfsCodes &dfs_codes, const Projection &projection, size_t nsupport, int prev_id);

 private:
  // Graphs after reconstructing
  vector<Graph> graphs_;
  // Single instance of minigraph
  unordered_map<size_t, size_t> frequent_vertex_labels_;
  unordered_map<size_t, size_t> frequent_edge_labels_;
  string output_file_;
  double support_;
  size_t nsupport_;
  gspan_instance_t *gspan_instances_;
};
}  // namespace gspan

#endif  // GSPAN_H_
