#include <gbolt.h>
#include <database.h>
#include <common.h>
#include <algorithm>

namespace gbolt {

void GBolt::execute() {
  Database *database = Database::get_instance();
  vector<Graph> graphs;
  vector<Graph> prune_graphs;
  // Phase 1: construct an initial graph
  #ifdef GBOLT_PERFORMANCE
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  database->construct_graphs(graphs);
  nsupport_ = graphs.size() * support_;
  // TODO: find frequent edges
  find_frequent_nodes_and_edges(graphs);

  // Phase 2: prune the initial graph by frequent labels
  database->construct_graphs(frequent_vertex_labels_, frequent_edge_labels_, prune_graphs);
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "gbolt construct graph time: " << elapsed;
  CPU_TIMER_START(elapsed, time_start);
  #endif

  // Phase 3: graph mining
  init_instances(prune_graphs);
  project(prune_graphs);
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "gbolt mine graph time: " << elapsed;
  #endif
}

void GBolt::init_instances(const vector<Graph> &graphs) {
  int num_threads = omp_get_max_threads();
  gbolt_instances_ = new gbolt_instance_t[num_threads];

  // Prepare history instance
  int max_edges = 0;
  int max_vertice = 0;
  for (auto i = 0; i < graphs.size(); ++i) {
    max_edges = std::max(graphs[i].get_nedges(), max_edges);
    max_vertice = std::max(
      static_cast<int>(graphs[i].get_p_vertice()->size()), max_vertice);
  }

  // Init an instance for each thread
  for (auto i = 0; i < num_threads; ++i) {
    LOG(INFO) << "gbolt thread " << i << " create";
    string output_file_thread = output_file_ + ".t" + std::to_string(i);
    gbolt_instances_[i].history = new History(max_edges, max_vertice);
    gbolt_instances_[i].output = new Output(output_file_thread);
    gbolt_instances_[i].min_graph = new Graph();
    gbolt_instances_[i].min_dfs_codes = new DfsCodes();
    gbolt_instances_[i].right_most_path = new Path<int>(DEFAULT_PATH_LEN);
  }
}

void GBolt::project(const vector<Graph> &graphs) {
  ProjectionMap projection_map;

  // Construct the first edge
  for (auto i = 0; i < graphs.size(); ++i) {
    const Graph &graph = graphs[i];

    for (auto j = 0; j < graph.size(); ++j) {
      const struct vertex_t *vertex = graph.get_p_vertex(j);
      Edges edges;

      if (get_forward_init(*vertex, graph, edges)) {
        for (auto k = 0; k < edges.size(); ++k) {
          const struct vertex_t *vertex_from = graph.get_p_vertex(edges[k]->from);
          const struct vertex_t *vertex_to = graph.get_p_vertex(edges[k]->to);
          // Push dfs code according to the same edge label
          struct dfs_code_t dfs_code(0, 1, vertex_from->label, edges[k]->label, vertex_to->label);
          // Push all the graphs
          projection_map[dfs_code].emplace_back(graphs[i].get_id(), edges[k], (const struct prev_dfs_t *)NULL);
        }
      }
    }
  }
  // Mine subgraphs
  int prev_graph_id = -1;
  int prev_thread_id = omp_get_thread_num();
  DfsCodes dfs_codes;
  #pragma omp parallel
  #pragma omp single nowait
  {
    for (auto it = projection_map.begin(); it != projection_map.end(); ++it) {
      // Parital pruning, like apriori
      Projection &projection = it->second;
      int nsupport = count_support(projection);
      if (nsupport < nsupport_) {
        continue;
      }
      int from_label = (it->first).from_label;
      int edge_label = (it->first).edge_label;
      int to_label = (it->first).to_label;
      #pragma omp task shared(graphs, projection, prev_thread_id, prev_graph_id) firstprivate(dfs_codes, nsupport)
      {
        dfs_codes.emplace_back(&(it->first));
        mine_subgraph(graphs, dfs_codes, projection, nsupport, prev_thread_id, prev_graph_id);
      }
    }
  }
  #pragma omp taskwait
}

}  // namespace gbolt
