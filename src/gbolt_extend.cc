#include <gbolt.h>
#include <history.h>
#include <common.h>

namespace gbolt {

void GBolt::enumerate(
  const vector<Graph> &graphs,
  const DfsCodes &dfs_codes,
  const Projection &projection,
  const vector<int> &right_most_path,
  int min_label,
  ProjectionMapBackward &projection_map_backward,
  ProjectionMapForward &projection_map_forward) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  History *history = instance->history;
  for (auto i = 0; i < projection.size(); ++i) {
    const Graph &graph = graphs[projection[i].id];
    history->build(projection[i], graph);

    get_backward(projection[i], graph, dfs_codes, right_most_path, projection_map_backward);
    get_first_forward(projection[i], graph, dfs_codes, right_most_path, min_label, projection_map_forward);
    get_other_forward(projection[i], graph, dfs_codes, right_most_path, min_label, projection_map_forward);
  }
}

bool GBolt::get_forward_init(const struct vertex_t &vertex, const Graph &graph, Edges &edges) {
  for (auto i = 0; i < vertex.edges.size(); ++i) {
    int to = vertex.edges[i].to;
    const struct vertex_t *next_vertex = graph.get_p_vertex(to);
    // Partial pruning: if the first label is greater than the second label, then there must be
    // another graph whose second label is greater than the first label.
    if (vertex.label <= next_vertex->label) {
      edges.emplace_back(&(vertex.edges[i]));
    }
  }
  return !edges.empty();
}

void GBolt::get_backward(
  const struct prev_dfs_t &prev_dfs,
  const Graph &graph,
  const DfsCodes &dfs_codes,
  const vector<int> &right_most_path,
  ProjectionMapBackward &projection_map_backward) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  History *history = instance->history;
  const struct edge_t *last_edge = history->get_p_edge(right_most_path[0]);
  const struct vertex_t *last_node = graph.get_p_vertex(last_edge->to);

  for (auto i = right_most_path.size(); i > 1; --i) {
    const struct edge_t *edge = history->get_p_edge(right_most_path[i - 1]);
    for (auto j = 0; j < (last_node->edges).size(); ++j) {
      if (history->has_edges((last_node->edges[j]).id))
        continue;
      const struct vertex_t *from_node = graph.get_p_vertex(edge->from);
      const struct vertex_t *to_node = graph.get_p_vertex(edge->to);
      if (last_node->edges[j].to == edge->from &&
          (last_node->edges[j].label > edge->label ||
           (last_node->edges[j].label == edge->label &&
            last_node->label >= to_node->label))) {
        int from_id = dfs_codes[right_most_path[0]]->to;
        int to_id = dfs_codes[right_most_path[i - 1]]->from;
        struct dfs_code_t dfs_code(from_id, to_id,
          last_node->label, (last_node->edges[j]).label, from_node->label);
        projection_map_backward[dfs_code].
          emplace_back(graph.get_id(), &(last_node->edges[j]), &(prev_dfs));
      }
    }
  }
}

void GBolt::get_first_forward(
  const struct prev_dfs_t &prev_dfs,
  const Graph &graph,
  const DfsCodes &dfs_codes,
  const vector<int> &right_most_path,
  int min_label,
  ProjectionMapForward &projection_map_forward) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  History *history = instance->history;
  const struct edge_t *last_edge = history->get_p_edge(right_most_path[0]);
  const struct vertex_t *last_node = graph.get_p_vertex(last_edge->to);

  for (auto i = 0; i < (last_node->edges).size(); ++i) {
    const struct edge_t *edge = &(last_node->edges[i]);
    const struct vertex_t *to_node = graph.get_p_vertex(edge->to);
    // Partial pruning: if this label is less than the minimum label, then there
    // should exist another lexicographical order which renders the same letters, but
    // in the asecending order.
    // Could we perform the same partial pruning as other extending methods?
    // No, we cannot, for this time, the extending id is greater the the last node
    if (history->has_vertice(edge->to) || to_node->label < min_label)
      continue;
    int to_id = dfs_codes[right_most_path[0]]->to;
    struct dfs_code_t dfs_code(to_id, to_id + 1,
      last_node->label, edge->label, to_node->label);
    projection_map_forward[dfs_code].
      emplace_back(graph.get_id(), edge, &(prev_dfs));
  }
}

void GBolt::get_other_forward(
  const struct prev_dfs_t &prev_dfs,
  const Graph &graph,
  const DfsCodes &dfs_codes,
  const vector<int> &right_most_path,
  int min_label,
  ProjectionMapForward &projection_map_forward) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  History *history = instance->history;
  for (auto i = 0; i < right_most_path.size(); ++i) {
    const struct edge_t *cur_edge = history->get_p_edge(right_most_path[i]);
    const struct vertex_t *cur_node = graph.get_p_vertex(cur_edge->from);
    const struct vertex_t *cur_to = graph.get_p_vertex(cur_edge->to);

    for (auto j = 0; j < cur_node->edges.size(); ++j) {
      const struct vertex_t *to_node = graph.get_p_vertex(cur_node->edges[j].to);
      // Partial pruning: guarantees that extending label is greater
      // than the minimum one
      if (history->has_vertice(to_node->id) ||
        to_node->id == cur_to->id || to_node->label < min_label)
        continue;
      if (cur_edge->label < cur_node->edges[j].label ||
          (cur_edge->label == cur_node->edges[j].label &&
           cur_to->label <= to_node->label)) {
        int from_id = dfs_codes[right_most_path[i]]->from;
        int to_id = dfs_codes[right_most_path[0]]->to;
        struct dfs_code_t dfs_code(from_id, to_id + 1, cur_node->label,
          cur_node->edges[j].label, to_node->label);
        projection_map_forward[dfs_code].
          emplace_back(graph.get_id(), &(cur_node->edges[j]), &(prev_dfs));
      }
    }
  }
}

}  // namespace gbolt
