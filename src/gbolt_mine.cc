#include <gbolt.h>
#include <graph.h>
#include <common.h>
#include <sstream>

namespace gbolt {

void GBolt::find_frequent_nodes_and_edges(const vector<Graph> &graphs) {
  unordered_map<int, vector<int> > vertex_labels;
  unordered_map<int, int> edge_labels;

  for (auto i = 0; i < graphs.size(); ++i) {
    unordered_set<int> vertex_set;
    unordered_set<int> edge_set;
    for (auto j = 0; j < graphs[i].size(); ++j) {
      const struct vertex_t *vertex = graphs[i].get_p_vertex(j);
      vertex_set.insert(vertex->label);
      for (auto k = 0; k < (vertex->edges).size(); ++k) {
        edge_set.insert(vertex->edges[k].label);
      }
    }
    for (auto it = vertex_set.begin(); it != vertex_set.end(); ++it) {
      vertex_labels[*it].emplace_back(i);
    }
    for (auto it = edge_set.begin(); it != edge_set.end(); ++it) {
      ++edge_labels[*it];
    }
  }
  for (auto it = vertex_labels.begin(); it != vertex_labels.end(); ++it) {
    if (it->second.size() >= nsupport_) {
      frequent_vertex_labels_.insert(std::make_pair(it->first, it->second));
    }
  }
  for (auto it = edge_labels.begin();
    it != edge_labels.end(); ++it) {
    if (it->second >= nsupport_) {
      frequent_edge_labels_.insert(std::make_pair(it->first, it->second));
    }
  }
}

void GBolt::report(const DfsCodes &dfs_codes, const Projection &projection,
  int nsupport, int prev_thread_id, int prev_graph_id) {
  std::stringstream ss;
  Graph graph;
  build_graph(dfs_codes, graph);

  for (auto i = 0; i < graph.size(); ++i) {
    const struct vertex_t *vertex = graph.get_p_vertex(i);
    ss << "v " << vertex->id << " " << vertex->label << std::endl;
  }
  for (auto i = 0; i < dfs_codes.size(); ++i) {
    ss << "e " << dfs_codes[i].from << " " << dfs_codes[i].to
      << " " << dfs_codes[i].edge_label << std::endl;
  }
  ss << "x: ";
  int prev = 0;
  for (auto i = 0; i < projection.size(); ++i) {
    if (i == 0 || projection[i].id != prev) {
      prev = projection[i].id;
      ss << prev << " ";
    }
  }
  ss << std::endl;
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  Output *output = instance->output;
  output->push_back(ss.str(), nsupport, output->size(), prev_thread_id, prev_graph_id);
}

void GBolt::save(bool output_parent, bool output_pattern, bool output_frequent_nodes) {
  #pragma omp parallel
  {
    gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
    Output *output = instance->output;
    output->save(output_parent, output_pattern);
  }
  // Save output for frequent nodes
  if (output_frequent_nodes) {
    string output_file_nodes = output_file_ + ".nodes";
    output_frequent_nodes_ = new Output(output_file_nodes);

    int graph_id = 0;
    for (auto it = frequent_vertex_labels_.begin();
      it != frequent_vertex_labels_.end(); ++it) {
      std::stringstream ss;

      ss << "v 0 " + std::to_string(it->first);
      ss << std::endl;
      ss << "x: ";
      for (auto i = 0; i < it->second.size(); ++i) {
        ss << it->second[i] << " ";
      }
      ss << std::endl;

      output_frequent_nodes_->push_back(ss.str(), it->second.size(), graph_id++);
    }
    output_frequent_nodes_->save(false, true);
  }
}

void GBolt::mine_subgraph(
  const vector<Graph> &graphs,
  const DfsCodes &dfs_codes,
  const Projection &projection,
  int prev_nsupport,
  int prev_thread_id,
  int prev_graph_id) {
  if (!is_min(dfs_codes)) {
    return;
  }
  report(dfs_codes, projection, prev_nsupport, prev_thread_id, prev_graph_id);
  prev_thread_id = omp_get_thread_num();
  gbolt_instance_t *instance = gbolt_instances_ + prev_thread_id;
  Output *output = instance->output;
  prev_graph_id = output->size() - 1;

  // Find right most path
  vector<int> right_most_path;
  build_right_most_path(dfs_codes, right_most_path);
  int min_label = dfs_codes[0].from_label;

  // Enumerate backward paths and forward paths by different rules
  ProjectionMapBackward projection_map_backward;
  ProjectionMapForward projection_map_forward;
  enumerate(graphs, dfs_codes, projection, right_most_path, min_label,
    projection_map_backward, projection_map_forward);
  // Recursive mining: first backward, last backward, and then last forward to the first forward
  for (auto it = projection_map_backward.begin(); it != projection_map_backward.end(); ++it) {
    Projection &projection = it->second;
    int nsupport = count_support(projection);
    if (nsupport < nsupport_) {
      continue;
    }
    int from = (it->first).from;
    int to = (it->first).to;
    int from_label = (it->first).from_label;
    int edge_label = (it->first).edge_label;
    int to_label = (it->first).to_label;
    #pragma omp task shared(graphs, dfs_codes, projection, prev_thread_id, prev_graph_id) firstprivate(nsupport)
    {
      DfsCodes dfs_codes_copy(dfs_codes);
      dfs_codes_copy.emplace_back(from, to, from_label, edge_label, to_label);
      mine_subgraph(graphs, dfs_codes_copy, projection, nsupport, prev_thread_id, prev_graph_id);
    }
  }
  for (auto it = projection_map_forward.rbegin(); it != projection_map_forward.rend(); ++it) {
    Projection &projection = it->second;
    int nsupport = count_support(projection);
    if (nsupport < nsupport_) {
      continue;
    }
    int from = (it->first).from;
    int to = (it->first).to;
    int from_label = (it->first).from_label;
    int edge_label = (it->first).edge_label;
    int to_label = (it->first).to_label;
    #pragma omp task shared(graphs, dfs_codes, projection, prev_thread_id, prev_graph_id) firstprivate(nsupport)
    {
      DfsCodes dfs_codes_copy(dfs_codes);
      dfs_codes_copy.emplace_back(from, to, from_label, edge_label, to_label);
      mine_subgraph(graphs, dfs_codes_copy, projection, nsupport, prev_thread_id, prev_graph_id);
    }
  }
  #pragma omp taskwait
}

}  // namespace gbolt
