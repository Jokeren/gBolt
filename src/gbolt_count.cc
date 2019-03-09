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
    struct edge_t edge(dfs_codes[i]->from, dfs_codes[i]->edge_label, dfs_codes[i]->to, edge_id);
    (*vertice)[edge.from].edges.emplace_back(
      dfs_codes[i]->from, dfs_codes[i]->edge_label, dfs_codes[i]->to, edge_id);
    (*vertice)[edge.to].edges.emplace_back(
      dfs_codes[i]->to, dfs_codes[i]->edge_label, dfs_codes[i]->from, edge_id);
    ++edge_id;
  }
  graph.set_nedges(edge_id);
}

bool GBolt::is_min(const DfsCodes &dfs_codes) {
  if (dfs_codes.size() == 1)
    return true;

  ProjectionMap projection_map;
  // Reuse memory, TODO: necessary or not? just set an index?
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  Graph *min_graph = instance->min_graph;
  DfsCodes *min_dfs_codes = instance->min_dfs_codes;
  min_graph->clear();
  min_dfs_codes->clear();
  // Build min graph
  build_graph(dfs_codes, *min_graph);

  Path<prev_dfs_t> min_projection;
  struct dfs_code_t min_dfs_code;
  bool first_dfs_code = true;

  for (auto i = 0; i < min_graph->size(); ++i) {
    const struct vertex_t *vertex = min_graph->get_p_vertex(i);
    Edges edges;

    if (get_forward_init(*vertex, *min_graph, edges)) {
      for (auto j = 0; j < edges.size(); ++j) {
        // Push dfs code according to the same edge label
        const struct vertex_t *vertex_from = min_graph->get_p_vertex(edges[j]->from);
        const struct vertex_t *vertex_to = min_graph->get_p_vertex(edges[j]->to);
        struct dfs_code_t dfs_code(0, 1, vertex_from->label, edges[j]->label, vertex_to->label);
        // Push back all the graphs
        if (first_dfs_code || dfs_code_project_compare_(dfs_code, min_dfs_code)) {
          first_dfs_code = false;
          min_dfs_code = dfs_code;
          min_projection.reset();
        }
        if (dfs_code == min_dfs_code) {
          min_projection.resize(min_projection.size() + 1);
          min_projection.back().id = 0;
          min_projection.back().edge = edges[j];
          min_projection.back().prev = (const prev_dfs_t *)NULL;
        }
      }
    }
  }
  min_dfs_codes->push_back(&min_dfs_code);
  if (*(dfs_codes[min_dfs_codes->size() - 1]) != *((*min_dfs_codes)[min_dfs_codes->size() - 1])) {
    return false;
  }
  Path<int> *right_most_path = instance->right_most_path;
  right_most_path->reset();
  build_right_most_path(*min_dfs_codes, *right_most_path);
  return is_projection_min(dfs_codes, min_projection, *right_most_path);
}

bool GBolt::judge_backward(
  const Path<int> &right_most_path,
  const Path<prev_dfs_t> &projection,
  int min_label,
  Path<prev_dfs_t> &min_projection,
  struct dfs_code_t &min_dfs_code) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  Graph *min_graph = instance->min_graph;
  DfsCodes *min_dfs_codes = instance->min_dfs_codes;
  History *history = instance->history;
  bool first_dfs_code = true;

  // i > 1, because it cannot reach the path itself
  for (auto i = right_most_path.size(); i > 1; --i) {
    for (auto j = 0; j < projection.size(); ++j) {
      history->build_edges(projection[j], *min_graph);

      const struct edge_t *edge = history->get_p_edge(right_most_path[i - 1]);
      const struct edge_t *last_edge = history->get_p_edge(right_most_path[0]);
      const struct vertex_t *from_node = min_graph->get_p_vertex(edge->from);
      const struct vertex_t *last_node = min_graph->get_p_vertex(last_edge->to);
      const struct vertex_t *to_node = min_graph->get_p_vertex(edge->to);

      for (auto k = 0; k < last_node->edges.size(); ++k) {
        if (history->has_edges(last_node->edges[k].id))
          continue;
        if (last_node->edges[k].to == edge->from &&
            (last_node->edges[k].label > edge->label ||
             (last_node->edges[k].label == edge->label &&
              last_node->label >= to_node->label))) {
          int from_id = (*min_dfs_codes)[right_most_path[0]]->to;
          int to_id = (*min_dfs_codes)[right_most_path[i - 1]]->from;
          struct dfs_code_t dfs_code(from_id, to_id,
            last_node->label, (last_node->edges[k]).label, from_node->label);
          if (first_dfs_code || dfs_code_backward_compare_(dfs_code, min_dfs_code)) {
            first_dfs_code = false;
            min_dfs_code = dfs_code;
            min_projection.reset();
          }
          if (dfs_code == min_dfs_code) {
            min_projection.resize(min_projection.size() + 1);
            min_projection.back().id = 0;
            min_projection.back().edge = &(last_node->edges[k]);
            min_projection.back().prev = &(projection[j]);
          }
        }
      }
    }
    if (min_projection.size() != 0)
      return true;
  }
  return false;
}


bool GBolt::judge_forward(
  const Path<int> &right_most_path,
  const Path<prev_dfs_t> &projection,
  int min_label,
  Path<prev_dfs_t> &min_projection,
  struct dfs_code_t &min_dfs_code) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  Graph *min_graph = instance->min_graph;
  History *history = instance->history;
  DfsCodes *min_dfs_codes = instance->min_dfs_codes;
  bool first_dfs_code = true;

  for (auto i = 0; i < projection.size(); ++i) {
    history->build_vertice(projection[i], *min_graph);

    const struct edge_t *last_edge = history->get_p_edge(right_most_path[0]);
    const struct vertex_t *last_node = min_graph->get_p_vertex(last_edge->to);

    for (auto j = 0; j < (last_node->edges).size(); ++j) {
      const struct edge_t *edge = &(last_node->edges[j]);
      const struct vertex_t *to_node = min_graph->get_p_vertex(edge->to);
      if (history->has_vertice(edge->to) || to_node->label < min_label)
        continue;
      int to_id = (*min_dfs_codes)[right_most_path[0]]->to;
      struct dfs_code_t dfs_code(to_id, to_id + 1, last_node->label, edge->label, to_node->label);
      if (first_dfs_code || dfs_code_forward_compare_(dfs_code, min_dfs_code)) {
        first_dfs_code = false;
        min_dfs_code = dfs_code;
        min_projection.reset();
      }
      if (dfs_code == min_dfs_code) {
        min_projection.resize(min_projection.size() + 1);
        min_projection.back().id = 0;
        min_projection.back().edge = edge;
        min_projection.back().prev = &(projection[i]);
      }
    }
  }

  if (min_projection.size() == 0) {
    for (auto i = 0; i < right_most_path.size(); ++i) {
      for (auto j = 0; j < projection.size(); ++j) {
        history->build_vertice(projection[j], *min_graph);

        const struct edge_t *cur_edge = history->get_p_edge(right_most_path[i]);
        const struct vertex_t *cur_node = min_graph->get_p_vertex(cur_edge->from);
        const struct vertex_t *cur_to = min_graph->get_p_vertex(cur_edge->to);

        for (auto k = 0; k < cur_node->edges.size(); ++k) {
          const struct vertex_t *to_node = min_graph->get_p_vertex(cur_node->edges[k].to);
          if (history->has_vertice(to_node->id) || cur_edge->to == to_node->id || to_node->label < min_label)
            continue;
          if (cur_edge->label < cur_node->edges[k].label ||
              (cur_edge->label == cur_node->edges[k].label &&
               cur_to->label <= to_node->label)) {
            int from_id = (*min_dfs_codes)[right_most_path[i]]->from;
            int to_id = (*min_dfs_codes)[right_most_path[0]]->to;
            struct dfs_code_t dfs_code(from_id, to_id + 1,
              cur_node->label, cur_node->edges[k].label, to_node->label);
            if (first_dfs_code || dfs_code_forward_compare_(dfs_code, min_dfs_code)) {
              first_dfs_code = false;
              min_dfs_code = dfs_code;
              min_projection.reset();
            }
            if (dfs_code == min_dfs_code) {
              min_projection.resize(min_projection.size() + 1);
              min_projection.back().id = 0;
              min_projection.back().edge = &(cur_node->edges[k]);
              min_projection.back().prev = &(projection[j]);
            }
          }
        }
      }
      if (min_projection.size() != 0) {
        break;
      }
    }
  }
  if (min_projection.size() != 0)
    return true;
  else 
    return false;
}

bool GBolt::is_projection_min(const DfsCodes &dfs_codes, const Path<prev_dfs_t> &projection,
  Path<int> &right_most_path) {
  gbolt_instance_t *instance = gbolt_instances_ + omp_get_thread_num();
  DfsCodes *min_dfs_codes = instance->min_dfs_codes;
  Path<prev_dfs_t> min_projection;
  struct dfs_code_t min_dfs_code;

  int min_label = (*min_dfs_codes)[0]->from_label;

  if (judge_backward(right_most_path, projection, min_label, min_projection, min_dfs_code)) {
    min_dfs_codes->emplace_back(&min_dfs_code);
    // Dfs code not equals to min dfs code
    if (*(dfs_codes[min_dfs_codes->size() - 1]) != *((*min_dfs_codes)[min_dfs_codes->size() - 1])) {
      return false;
    }
    // Current dfs code is min
    update_right_most_path((*min_dfs_codes), right_most_path);
    return is_projection_min(dfs_codes, min_projection, right_most_path);
  }

  if (judge_forward(right_most_path, projection, min_label, min_projection, min_dfs_code)) {
    min_dfs_codes->emplace_back(&min_dfs_code);
    // Dfs code not equals to min dfs code
    if (*(dfs_codes[min_dfs_codes->size() - 1]) != *((*min_dfs_codes)[min_dfs_codes->size() - 1])) {
      return false;
    }
    // Current dfs code is min
    update_right_most_path((*min_dfs_codes), right_most_path);
    return is_projection_min(dfs_codes, min_projection, right_most_path);
  }
  return true;
}

}  // namespace gbolt
