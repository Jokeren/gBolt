#ifndef INCLUDE_GBOLT_H_
#define INCLUDE_GBOLT_H_

#include <common.h>
#include <graph.h>
#include <history.h>
#include <output.h>
#include <map>
#include <vector>
#include <string>

namespace gbolt {

struct gbolt_instance_t {
  Graph *min_graph = 0;
  DfsCodes *min_dfs_codes = 0;
  History *history = 0;
  Output *output = 0;

  ~gbolt_instance_t() {
    delete this->min_graph;
    delete this->min_dfs_codes;
    delete this->history;
    delete this->output;
  }
};

class GBolt {
 public:
  GBolt(const string &output_file, double support) :
    output_file_(output_file), support_(support),
    output_frequent_nodes_(0), gbolt_instances_(0) {}

  void execute();

  void save(bool output_parent = false, bool output_pattern = false, bool output_frequent_nodes = false);

  ~GBolt() {
    if (gbolt_instances_ != 0) {
      delete[] gbolt_instances_;
    }
    if (output_frequent_nodes_ != 0) {
      delete output_frequent_nodes_;
    }
  }

 private:
  typedef map<struct dfs_code_t, Projection, struct dfs_code_project_compare_t> ProjectionMap;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_backward_compare_t> ProjectionMapBackward;
  typedef map<struct dfs_code_t, Projection, struct dfs_code_forward_compare_t> ProjectionMapForward;

 private:
  // Mine
  void init_instances(const vector<Graph> &graphs);

  void project(const vector<Graph> &graphs);

  void find_frequent_nodes_and_edges(const vector<Graph> &graphs);

  void mine_subgraph(
    const vector<Graph> &graphs,
    const DfsCodes &dfs_codes,
    const Projection &projection,
    int prev_nsupport,
    int prev_thread_id,
    int prev_graph_id);

  // Extend
  void build_right_most_path(const DfsCodes &dfs_codes, vector<int> &right_most_path);

  void enumerate(
    const vector<Graph> &graphs,
    const DfsCodes &dfs_codes,
    const Projection &projection,
    const vector<int> &right_most_path,
    int min_label,
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
    const vector<int> &right_most_path,
    int min_label,
    ProjectionMapForward& projection_map_forward);

  void get_other_forward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const vector<int> &right_most_path,
    int min_label,
    ProjectionMapForward& projection_map_forward);

  void get_backward(
    const struct prev_dfs_t &prev_dfs,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const vector<int> &right_most_path,
    ProjectionMapBackward& projection_map_backward);

  // Count
  int count_support(const Projection &projection);

  void build_graph(const DfsCodes &dfs_codes, Graph &graph);

  bool is_min(const DfsCodes &dfs_codes);

  bool is_projection_min(const DfsCodes &dfs_codes, const Projection &projection);

  bool judge_backward(
    const vector<int> &right_most_path,
    const Projection &projection,
    int min_label,
    ProjectionMapBackward &projection_map_backward);

  bool judge_forward(
    const vector<int> &right_most_path,
    const Projection &projection,
    int min_label,
    ProjectionMapForward &projection_map_forward);

  // Report
  void report(const DfsCodes &dfs_codes, const Projection &projection,
    int nsupport, int prev_thread_id, int prev_graph_id);

 private:
  // Graphs after reconstructing
  vector<Graph> graphs_;
  // Single instance of minigraph
  unordered_map<int, vector<int> > frequent_vertex_labels_;
  unordered_map<int, int> frequent_edge_labels_;
  string output_file_;
  double support_;
  int nsupport_;
  Output *output_frequent_nodes_;
  gbolt_instance_t *gbolt_instances_;
};

}  // namespace gbolt

#endif  // INCLUDE_GBOLT_H_
