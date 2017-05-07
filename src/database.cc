#include <database.h>
#include <graph.h>
#include <common.h>
#include <fstream>
#include <cstdlib>

namespace gspan {

void Database::read_input(const string &input_file, const string &separator) {
  std::ifstream fin(input_file.c_str());
  char line[FILE_MAX_LINE];

  if (!fin.is_open()) {
    LOG(FATAL) << "Open file: " << input_file << " error!";
  }

  size_t num_line = 0;
  while (fin.getline(line, FILE_MAX_LINE)) {
    char *pch = NULL;
    pch = strtok(line, separator.c_str());
    input_.resize(num_line + 1);
    if (*pch == 't') {
      ++num_graph_;
    }
    while (pch != NULL) {
      input_[num_line].push_back(string(pch));
      pch = strtok(NULL, separator.c_str());
    }
    ++num_line;
  }
  fin.close();
}

// Construct graph
void Database::construct_graphs(vector<Graph> &graphs) {
  size_t graph_index = 0;
  size_t edge_id = 0;
  graphs.resize(num_graph_);
  Vertice *vertice = graphs[graph_index].get_p_vertice();

  for (size_t i = 0; i < input_.size(); ++i) {
    if (input_[i][0] == "t") {
      if (i != 0) {
        graphs[graph_index].set_nedges(edge_id);
        vertice = graphs[++graph_index].get_p_vertice();
        edge_id = 0;
      }
      graphs[graph_index].set_id(atoi(input_[i][2].c_str()));
    } else if (input_[i][0] == "v") {
      size_t id = atoi(input_[i][1].c_str());
      size_t label = atoi(input_[i][2].c_str());
      struct vertex_t vertex(id, label);
      vertice->push_back(vertex);
    } else if (input_[i][0] == "e") {
      size_t from = atoi(input_[i][1].c_str());
      size_t to = atoi(input_[i][2].c_str());
      size_t label = atoi(input_[i][3].c_str());
      // Add an edge
      struct edge_t edge(from, label, to, edge_id);
      // Forward direction edge
      (*vertice)[from].edges.push_back(edge);
      // Backward direction edge
      edge.from = to;
      edge.to = from;
      (*vertice)[to].edges.push_back(edge);
      ++edge_id;
    } else {
      LOG(ERROR) << "Reading input error!";
    }
  }
  graphs[graph_index].set_nedges(edge_id);
}

// Construct graph by labels
void Database::construct_graphs(const unordered_map<size_t, size_t> &frequent_labels, vector<Graph> &graphs) {
  vector<size_t> labels;
  unordered_map<size_t, size_t> id_map;
  size_t graph_index = 0;
  size_t edge_id = 0;
  size_t vertex_id = 0;
  graphs.resize(num_graph_);
  Vertice *vertice = graphs[graph_index].get_p_vertice();

  for (size_t i = 0; i < input_.size(); ++i) {
    if (input_[i][0] == "t") {
      if (i != 0) {
        graphs[graph_index].set_nedges(edge_id);
        vertice = graphs[++graph_index].get_p_vertice();
        edge_id = 0;
        vertex_id = 0;
        labels.clear();
        id_map.clear();
      }
      graphs[graph_index].set_id(atoi(input_[i][2].c_str()));
    } else if (input_[i][0] == "v") {
      size_t id = atoi(input_[i][1].c_str());
      size_t label = atoi(input_[i][2].c_str());
      labels.push_back(label);
      // Find a node with frequent label
      if (frequent_labels.find(label) != frequent_labels.end()) {
        struct vertex_t vertex(vertex_id, label);
        vertice->push_back(vertex);
        id_map[id] = vertex_id;
        ++vertex_id;
      }
    } else if (input_[i][0] == "e") {
      size_t from = atoi(input_[i][1].c_str());
      size_t to = atoi(input_[i][2].c_str());
      size_t label = atoi(input_[i][3].c_str());
      size_t label_from = labels[from];
      size_t label_to = labels[to];
      // Find an edge with frequent label
      if (frequent_labels.find(label_from) != frequent_labels.end() &&
        frequent_labels.find(label_to) != frequent_labels.end()) {
        struct edge_t edge(id_map[from], label, id_map[to], edge_id);
        // First edge
        (*vertice)[id_map[from]].edges.push_back(edge);
        // Second edge
        edge.from = id_map[to];
        edge.to = id_map[from];
        (*vertice)[id_map[to]].edges.push_back(edge);
        ++edge_id;
      }
    } else {
      LOG(ERROR) << "Reading input error!";
    }
  }
  graphs[graph_index].set_nedges(edge_id);
}

}  // namespace gspan
