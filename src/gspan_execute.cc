#include <gspan.h>
#include <database.h>
#include <common.h>

namespace gspan {

void GSpan::execute() {
  Database *database = Database::get_instance();
  vector<Graph> graphs;
  vector<Graph> prune_graphs;
  // Phase 1: construct an initial graph
  #ifdef GSPAN_PERFORMANCE  
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  database->construct_graphs(graphs);
  nsupport_ = graphs.size() * support_;
  // TODO: find frequent edges
  find_frequent_nodes(graphs);

  // Phase 2: prune the initial graph by frequent labels
  database->construct_graphs(frequent_labels_, prune_graphs);
  #ifdef GSPAN_PERFORMANCE  
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "GSPAN construct graph time: " << elapsed; 
  CPU_TIMER_START(elapsed, time_start);
  #endif

  // Phase 3: graph mining
  project(prune_graphs);
  #ifdef GSPAN_PERFORMANCE  
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "GSPAN mine graph time: " << elapsed;
  #endif
}

void GSpan::project(const vector<Graph> &graphs) {
  ProjectionMap projection_map;

  // Construct the first edge
  for (size_t i = 0; i < graphs.size(); ++i) {
    const Graph &graph = graphs[i];

    for (size_t j = 0; j < graph.size(); ++j) {
      const struct vertex_t *vertex = graph.get_p_vertex(j);
      Edges edges;

      if (get_forward_init(*vertex, graph, edges)) {
        for (size_t k = 0; k < edges.size(); ++k) {
          const struct vertex_t *vertex_from = graph.get_p_vertex(edges[k]->from);
          const struct vertex_t *vertex_to = graph.get_p_vertex(edges[k]->to);
          // Push dfs code according to the same edge label
          struct dfs_code_t dfs_code(0, 1, vertex_from->label, edges[k]->label, vertex_to->label);
          // Push all the graphs
          struct prev_dfs_t prev_dfs(graphs[i].get_id(), edges[k], NULL);
          projection_map[dfs_code].push_back(prev_dfs);
        }
      }
    }
  }
  // Mine subgraphs
  int prev_id = -1;
  for (ProjectionMap::iterator it = projection_map.begin(); it != projection_map.end(); ++it) {
    // Parital pruning, like apriori
    if ((it->second).size() < nsupport_)
      continue;
    dfs_codes_.push_back(dfs_code_t(0, 1,
      (it->first).from_label, (it->first).edge_label, (it->first).to_label));
    mine_subgraph(graphs, prev_id, it->second);
    prev_id = -1;
    dfs_codes_.pop_back();
  }
}

}  // namespace gspan
