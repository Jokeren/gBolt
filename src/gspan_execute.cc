#include <gspan.h>
#include <database.h>
#include <common.h>
#include <algorithm>

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
  database->construct_graphs(frequent_vertex_labels_, frequent_edge_labels_, prune_graphs);
  #ifdef GSPAN_PERFORMANCE  
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "GSPAN construct graph time: " << elapsed; 
  CPU_TIMER_START(elapsed, time_start);
  #endif

  // Phase 3: graph mining
  init_instances(prune_graphs);
  project(prune_graphs);
  #ifdef GSPAN_PERFORMANCE  
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "GSPAN mine graph time: " << elapsed;
  #endif
}

void GSpan::init_instances(const vector<Graph> &graphs) {
  size_t num_threads = omp_get_max_threads();
  gspan_instances_ = new gspan_instance_t[num_threads];

  // Prepare history instance
  size_t max_edges = 0;
  size_t max_vertice = 0;
  for (size_t i = 0; i < graphs.size(); ++i) {
    max_edges = std::max(graphs[i].get_nedges(), max_edges);
    max_vertice = std::max(graphs[i].get_p_vertice()->size(), max_vertice);
  }

  // Init instance for each thread
  for (size_t i = 0; i < num_threads; ++i) {
    LOG(INFO) << "GSPAN thread " << i << " create";
    string output_file_thread = output_file_ + ".t" + std::to_string(i);
    gspan_instances_[i].history = new History(max_edges, max_vertice);
    gspan_instances_[i].output = new Output(output_file_thread);
    gspan_instances_[i].min_graph = new Graph();
    gspan_instances_[i].min_dfs_codes = new DfsCodes();
  }
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
  DfsCodes dfs_codes;
  #pragma omp parallel
  #pragma omp single nowait
  {
    for (ProjectionMap::iterator it = projection_map.begin(); it != projection_map.end(); ++it) {
      // Parital pruning, like apriori
      Projection &projection = it->second;
      size_t nsupport = count_support(projection);
      if (nsupport < nsupport_) {
        continue;
      }
      size_t from_label = (it->first).from_label;
      size_t edge_label = (it->first).edge_label;
      size_t to_label = (it->first).to_label;
      #pragma omp task shared(graphs, projection, prev_id) firstprivate(dfs_codes, nsupport) if (nsupport > 1.5 * nsupport_)
      {
        dfs_codes.push_back(dfs_code_t(0, 1, from_label, edge_label, to_label));
        mine_subgraph(graphs, dfs_codes, projection, nsupport, prev_id);
      }
    }
  }
  #pragma omp taskwait
}

}  // namespace gspan
