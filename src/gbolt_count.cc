#include <gbolt.h>
#include <history.h>
#include <path.h>
#include <algorithm>


namespace gbolt {

int GBolt::count_support(const Projection &projection) {
  int prev_id = -1;
  int size = 0;

  for (auto i = 0; i < projection.size(); ++i) {
    if (prev_id != projection[i].id) {
      prev_id = projection[i].id;
      ++size;
    }
  }
  return size;
}

void GBolt::build_graph(const DfsCodes &dfs_codes, Graph &graph) {
  int edge_id = 0;
  Vertice *vertice = graph.get_p_vertice();

  for (auto i = 0; i < dfs_codes.size(); ++i) {
    // Push vertice
    vertice->resize(std::max(dfs_codes[i]->from + 1, static_cast<int>(vertice->size())));
    vertice->resize(std::max(dfs_codes[i]->to + 1, static_cast<int>(vertice->size())));
    (*vertice)[dfs_codes[i]->from].label = dfs_codes[i]->from_label;
    (*vertice)[dfs_codes[i]->from].id = dfs_codes[i]->from;
    (*vertice)[dfs_codes[i]->to].label = dfs_codes[i]->to_label;
    (*vertice)[dfs_codes[i]->to].id = dfs_codes[i]->to;
    // Push an edge
    (*vertice)[dfs_codes[i]->from].edges.emplace_back(
      dfs_codes[i]->from, dfs_codes[i]->edge_label, dfs_codes[i]->to, edge_id);
    (*vertice)[dfs_codes[i]->to].edges.emplace_back(
      dfs_codes[i]->to, dfs_codes[i]->edge_label, dfs_codes[i]->from, edge_id);
    ++edge_id;
  }
  graph.set_nedges(edge_id);
}

bool GBolt::is_min(const DfsCodes &dfs_codes) {
  if (dfs_codes.size() == 1)
    return true;

  // Reuse memory, TODO: necessary or not? just set an index?
  #ifdef GBOLT_SERIAL
  gbolt_instance_t *instance = gbolt_instances_;
  #else
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  #endif
  Graph *min_graph = instance->min_graph;
  DfsCodes *min_dfs_codes = instance->min_dfs_codes;
  History *history = instance->history;
  MinProjection *min_projection = instance->min_projection;
  Path<int> *right_most_path = instance->right_most_path;

  // Clear cache data structures
  min_graph->clear();
  min_dfs_codes->clear();
  min_projection->reset();
  right_most_path->reset();
  // Build min graph
  build_graph(dfs_codes, *min_graph);

  dfs_code_t min_dfs_code;
  bool first_dfs_code = true;

  for (auto i = 0; i < min_graph->size(); ++i) {
    const vertex_t *vertex = min_graph->get_p_vertex(i);
    Edges edges;

    if (get_forward_init(*vertex, *min_graph, edges)) {
      for (auto j = 0; j < edges.size(); ++j) {
        // Push dfs code according to the same edge label
        const vertex_t *vertex_from = min_graph->get_p_vertex(edges[j]->from);
        const vertex_t *vertex_to = min_graph->get_p_vertex(edges[j]->to);
        dfs_code_t dfs_code(0, 1, vertex_from->label, edges[j]->label, vertex_to->label);
        // Push back all the graphs
        if (first_dfs_code || dfs_code_project_compare_(dfs_code, min_dfs_code)) {
          first_dfs_code = false;
          min_dfs_code = dfs_code;
          min_projection->reset();
        }
        if (dfs_code == min_dfs_code) {
          min_projection->resize(min_projection->size() + 1);
          min_projection->back().edge = edges[j];
          min_projection->back().prev = -1;
        }
      }
    }
  }
  min_dfs_codes->push_back(&min_dfs_code);
  if (*(dfs_codes[0]) != min_dfs_code) {
    return false;
  }
  build_right_most_path(*min_dfs_codes, *right_most_path);
  return is_projection_min(dfs_codes, *min_graph, *history,
    *min_dfs_codes, *right_most_path, *min_projection, 0);
}

bool GBolt::judge_backward(
  const Path<int> &right_most_path,
  const Graph &min_graph,
  History &history,
  dfs_code_t &min_dfs_code,
  DfsCodes &min_dfs_codes,
  MinProjection &projection,
  size_t projection_start_index,
  size_t projection_end_index) {
  int min_label = min_dfs_codes[0]->from_label;
  bool first_dfs_code = true;

  // i > 1, because it cannot reach the path itself
  for (auto i = right_most_path.size(); i > 1; --i) {
    for (auto j = projection_start_index; j < projection_end_index; ++j) {
      history.build_edges_min(projection, min_graph, j);

      const edge_t *edge = history.get_p_edge(right_most_path[i - 1]);
      const edge_t *last_edge = history.get_p_edge(right_most_path[0]);
      const vertex_t *from_node = min_graph.get_p_vertex(edge->from);
      const vertex_t *last_node = min_graph.get_p_vertex(last_edge->to);
      const vertex_t *to_node = min_graph.get_p_vertex(edge->to);

      for (auto k = 0; k < last_node->edges.size(); ++k) {
        if (history.has_edges(last_node->edges[k].id))
          continue;
        if (last_node->edges[k].to == edge->from &&
            (last_node->edges[k].label > edge->label ||
             (last_node->edges[k].label == edge->label &&
              last_node->label >= to_node->label))) {
          int from_id = min_dfs_codes[right_most_path[0]]->to;
          int to_id = min_dfs_codes[right_most_path[i - 1]]->from;
          dfs_code_t dfs_code(from_id, to_id,
            last_node->label, (last_node->edges[k]).label, from_node->label);
          if (first_dfs_code || dfs_code_backward_compare_(dfs_code, min_dfs_code)) {
            first_dfs_code = false;
            min_dfs_code = dfs_code;
            projection.resize(projection_end_index);
          }
          if (dfs_code == min_dfs_code) {
            projection.resize(projection.size() + 1);
            projection.back().edge = &(last_node->edges[k]);
            projection.back().prev = j;
          }
        }
      }
    }
    if (projection.size() > projection_end_index)
      return true;
  }
  return false;
}

bool GBolt::judge_forward(
  const Path<int> &right_most_path,
  const Graph &min_graph,
  History &history,
  dfs_code_t &min_dfs_code,
  DfsCodes &min_dfs_codes,
  MinProjection &projection,
  size_t projection_start_index,
  size_t projection_end_index) {
  int min_label = min_dfs_codes[0]->from_label;
  bool first_dfs_code = true;

  for (auto i = projection_start_index; i < projection_end_index; ++i) {
    history.build_vertice_min(projection, min_graph, i);

    const edge_t *last_edge = history.get_p_edge(right_most_path[0]);
    const vertex_t *last_node = min_graph.get_p_vertex(last_edge->to);

    for (auto j = 0; j < (last_node->edges).size(); ++j) {
      const edge_t *edge = &(last_node->edges[j]);
      const vertex_t *to_node = min_graph.get_p_vertex(edge->to);
      if (history.has_vertice(edge->to) || to_node->label < min_label)
        continue;
      int to_id = min_dfs_codes[right_most_path[0]]->to;
      dfs_code_t dfs_code(to_id, to_id + 1, last_node->label, edge->label, to_node->label);
      if (first_dfs_code || dfs_code_forward_compare_(dfs_code, min_dfs_code)) {
        first_dfs_code = false;
        min_dfs_code = dfs_code;
        projection.resize(projection_end_index);
      }
      if (dfs_code == min_dfs_code) {
        projection.resize(projection.size() + 1);
        projection.back().edge = edge;
        projection.back().prev = i;
      }
    }
  }

  if (projection.size() == projection_end_index) {
    for (auto i = 0; i < right_most_path.size(); ++i) {
      for (auto j = projection_start_index; j < projection_end_index; ++j) {
        history.build_vertice_min(projection, min_graph, j);

        const edge_t *cur_edge = history.get_p_edge(right_most_path[i]);
        const vertex_t *cur_node = min_graph.get_p_vertex(cur_edge->from);
        const vertex_t *cur_to = min_graph.get_p_vertex(cur_edge->to);

        for (auto k = 0; k < cur_node->edges.size(); ++k) {
          const vertex_t *to_node = min_graph.get_p_vertex(cur_node->edges[k].to);
          if (history.has_vertice(to_node->id) || cur_edge->to == to_node->id || to_node->label < min_label)
            continue;
          if (cur_edge->label < cur_node->edges[k].label ||
              (cur_edge->label == cur_node->edges[k].label &&
               cur_to->label <= to_node->label)) {
            int from_id = min_dfs_codes[right_most_path[i]]->from;
            int to_id = min_dfs_codes[right_most_path[0]]->to;
            dfs_code_t dfs_code(from_id, to_id + 1,
              cur_node->label, cur_node->edges[k].label, to_node->label);
            if (first_dfs_code || dfs_code_forward_compare_(dfs_code, min_dfs_code)) {
              first_dfs_code = false;
              min_dfs_code = dfs_code;
              projection.resize(projection_end_index);
            }
            if (dfs_code == min_dfs_code) {
              projection.resize(projection.size() + 1);
              projection.back().edge = &(cur_node->edges[k]);
              projection.back().prev = j;
            }
          }
        }
      }
      if (projection.size() > projection_end_index) {
        break;
      }
    }
  }
  if (projection.size() > projection_end_index)
    return true;
  else 
    return false;
}

bool GBolt::is_projection_min(
  const DfsCodes &dfs_codes,
  const Graph &min_graph,
  History &history,
  DfsCodes &min_dfs_codes,
  Path<int> &right_most_path,
  MinProjection &projection,
  size_t projection_start_index) {
  dfs_code_t min_dfs_code;
  size_t projection_end_index = projection.size();

  if (judge_backward(right_most_path, min_graph, history, min_dfs_code, min_dfs_codes,
    projection, projection_start_index, projection_end_index)) {
    min_dfs_codes.emplace_back(&min_dfs_code);
    // Dfs code not equals to min dfs code
    if (*(dfs_codes[min_dfs_codes.size() - 1]) != min_dfs_code) {
      return false;
    }
    // Current dfs code is min
    update_right_most_path(min_dfs_codes, right_most_path);
    return is_projection_min(dfs_codes, min_graph, history,
      min_dfs_codes, right_most_path, projection, projection_end_index);
  }

  if (judge_forward(right_most_path, min_graph, history, min_dfs_code, min_dfs_codes,
    projection, projection_start_index, projection_end_index)) {
    min_dfs_codes.emplace_back(&min_dfs_code);
    // Dfs code not equals to min dfs code
    if (*(dfs_codes[min_dfs_codes.size() - 1]) != min_dfs_code) {
      return false;
    }
    // Current dfs code is min
    update_right_most_path(min_dfs_codes, right_most_path);
    return is_projection_min(dfs_codes, min_graph, history,
      min_dfs_codes, right_most_path, projection, projection_end_index);
  }

  return true;
}

}  // namespace gbolt
