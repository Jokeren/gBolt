#ifndef GSPAN_H_
#define GSPAN_H_

#include <common.h>
#include <graph.h>
#include <output.h>

namespace gspan {

class History;

class GSpan {
 public:
  GSpan(const string &output_file, double support) :
    output_(output_file), support_(support) {};

  void execute();

  void save();
 
 private:
  typedef map<struct dfs_code_t, Projection, struct dfs_code_project_compare_t> ProjectionMap;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_backward_compare_t> ProjectionMapBackward;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_forward_compare_t> ProjectionMapForward;
  
 private:
  // Mine
  void project(const vector<Graph> &graphs);

  void find_frequent_nodes(const vector<Graph> &graphs);

  void mine_subgraph(const vector<Graph> &graphs, int prev_id, Projection& projection);

  // Extend
  void build_right_most_path(const DfsCodes &dfs_codes, vector<size_t> &right_most_path);

  void enumerate(
    const vector<Graph> &graphs,
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
    const vector<size_t> &right_most_path,
    const History &history,
    size_t min_label,
    ProjectionMapForward& projection_map_forward);

  void get_other_forward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const vector<size_t> &right_most_path,
    const History &history,
    size_t min_label,
    ProjectionMapForward& projection_map_forward);

  void get_backward(
    const struct prev_dfs_t &prev_dfs, 
    const Graph &graph,
    const vector<size_t> &right_most_path,
    const History &history,
    ProjectionMapBackward& projection_map_backward);

  // Count
  size_t count_support(const Projection &projection);

  void build_graph(Graph &graph);

  bool is_min();

  bool is_projection_min(const Projection &projection);

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
  void report(const Projection &projection, int prev_id, size_t nsupport);

 private:
  // Graphs after reconstructing
  vector<Graph> graphs_;
  // Single instance of minigraph
  Graph min_graph_;
  map<size_t, size_t> frequent_labels_;
  DfsCodes dfs_codes_;
  DfsCodes min_dfs_codes_;
  Output output_;
  double support_;
  size_t nsupport_;
};
}  // namespace gspan

#endif  // GSPAN_H_
