#include <gspan.h>
#include <graph.h>
#include <common.h>
#include <sstream>

namespace gspan {

void GSpan::find_frequent_nodes(const vector<Graph> &graphs) {
  unordered_map<size_t, size_t> vertex_labels;
  unordered_map<size_t, size_t> edge_labels;

  for (size_t i = 0; i < graphs.size(); ++i) {
    unordered_set<size_t> vertex_set;
    unordered_set<size_t> edge_set;
    for (size_t j = 0; j < graphs[i].size(); ++j) {
      const struct vertex_t *vertex = graphs[i].get_p_vertex(j);
      vertex_set.insert(vertex->label);
      for (size_t k = 0; k < (vertex->edges).size(); ++k) {
        edge_set.insert(vertex->edges[k].label);
      }
    }
    for (unordered_set<size_t>::iterator it = vertex_set.begin(); it != vertex_set.end(); ++it) {
      ++vertex_labels[*it];
    }
    for (unordered_set<size_t>::iterator it = edge_set.begin(); it != edge_set.end(); ++it) {
      ++edge_labels[*it];
    }
  }
  for (unordered_map<size_t, size_t>::iterator it = vertex_labels.begin();
    it != vertex_labels.end(); ++it) {
    if (it->second >= nsupport_) {
      frequent_vertex_labels_.insert(std::make_pair(it->first, it->second));
    }
  }
  for (unordered_map<size_t, size_t>::iterator it = edge_labels.begin();
    it != edge_labels.end(); ++it) {
    if (it->second >= 2 * nsupport_) {  // undirected edges
      frequent_edge_labels_.insert(std::make_pair(it->first, it->second));
    }
  }
}

void GSpan::report(const DfsCodes &dfs_codes, const Projection &projection, size_t nsupport, int prev_id) {
  std::stringstream ss;
  Graph graph;
  build_graph(dfs_codes, graph);

  for (size_t i = 0; i < graph.size(); ++i) {
    const struct vertex_t *vertex = graph.get_p_vertex(i);
    ss << "v " << vertex->id << " " << vertex->label << std::endl;
  }
  for (size_t i = 0; i < dfs_codes.size(); ++i) {
    ss << "e " << dfs_codes[i].from << " " << dfs_codes[i].to
      << " " << dfs_codes[i].edge_label << std::endl;
  }
  ss << "x: ";
  size_t prev = 0;
  for (size_t i = 0; i < projection.size(); ++i) {
    if (i == 0 || projection[i].id != prev) {
      prev = projection[i].id;
      ss << prev << " ";
    }
  }
  ss << std::endl;
  gspan_instance_t *instance = gspan_instances_ + omp_get_thread_num();
  Output *output = instance->output;
  output->push_back(ss.str(), nsupport, output->size(), prev_id);
}

void GSpan::save(bool output_parent) {
  #pragma omp parallel
  {
    gspan_instance_t *instance = gspan_instances_ + omp_get_thread_num();
    Output *output = instance->output;
    output->save(output_parent);
  }
}

void GSpan::mine_subgraph(
  const vector<Graph> &graphs,
  const DfsCodes &dfs_codes,
  const Projection &projection,
  size_t prev_nsupport,
  int prev_id) {
  if (!is_min(dfs_codes)) {
    return;
  }
  report(dfs_codes, projection, prev_nsupport, prev_id);
  gspan_instance_t *instance = gspan_instances_ + omp_get_thread_num();
  Output *output = instance->output;
  prev_id = output->size() - 1;

  // Find right most path
  vector<size_t> right_most_path;
  build_right_most_path(dfs_codes, right_most_path);
  size_t min_label = dfs_codes[0].from_label;

  // Enumerate backward paths and forward paths by different rules
  ProjectionMapBackward projection_map_backward;
  ProjectionMapForward projection_map_forward;
  enumerate(graphs, dfs_codes, projection, right_most_path, min_label,
    projection_map_backward, projection_map_forward);
  // Recursive mining: first backward, last backward, and then last forward to the first forward
  for (ProjectionMapBackward::iterator it = projection_map_backward.begin();
    it != projection_map_backward.end(); ++it) {
    Projection &projection = it->second;
    size_t nsupport = count_support(projection);
    if (nsupport < nsupport_) {
      continue;
    }
    size_t from = (it->first).from;
    size_t to = (it->first).to;
    size_t from_label = (it->first).from_label;
    size_t edge_label = (it->first).edge_label;
    size_t to_label = (it->first).to_label;
    #pragma omp task shared(graphs, dfs_codes, projection, prev_id) firstprivate(nsupport) if (nsupport > 1.2 * nsupport_)
    {
      DfsCodes dfs_codes_copy(dfs_codes);
      dfs_codes_copy.push_back(dfs_code_t(from, to, from_label, edge_label, to_label));
      mine_subgraph(graphs, dfs_codes_copy, projection, nsupport, prev_id);
    }
  }
  for (ProjectionMapForward::iterator it = projection_map_forward.begin();
    it != projection_map_forward.end(); ++it) {
    Projection &projection = it->second;
    size_t nsupport = count_support(projection);
    if (nsupport < nsupport_) {
      continue;
    }
    size_t from = (it->first).from;
    size_t to = (it->first).to;
    size_t from_label = (it->first).from_label;
    size_t edge_label = (it->first).edge_label;
    size_t to_label = (it->first).to_label;
    #pragma omp task shared(graphs, dfs_codes, projection, prev_id) firstprivate(nsupport) if (nsupport > 1.2 * nsupport_)
    {
      DfsCodes dfs_codes_copy(dfs_codes);
      dfs_codes_copy.push_back(dfs_code_t(from, to, from_label, edge_label, to_label));
      mine_subgraph(graphs, dfs_codes_copy, projection, nsupport, prev_id);
    }
  }
  #pragma omp taskwait
}

}  // namespace gspan
