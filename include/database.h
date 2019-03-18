#ifndef INCLUDE_DATABASE_H_
#define INCLUDE_DATABASE_H_

#include <common.h>
#include <vector>
#include <string>

namespace gbolt {

class Graph;

class Database {
 public:
  static Database *get_instance() {
    return instance_;
  }

  // Return graph count
  void read_input(const string &input_file, const string &separator);

  // Construct graph
  void construct_graphs(vector<Graph> &graphs);

  // Construct graph by frequent labels
  void construct_graphs(
  #ifdef GBOLT_PERFORMANCE
    const unordered_map<int, std::vector<int> > &frequent_vertex_labels,
    const unordered_map<int, int> &frequent_edge_labels,
  #else
    const map<int, std::vector<int> > &frequent_vertex_labels,
    const map<int, int> &frequent_edge_labels,
  #endif
    vector<Graph> &graphs);

  ~Database() {
    delete instance_;
  }

 private:
  Database() {}

 private:
  static Database *instance_;
  vector<vector<string> > input_;
  int num_graph_ = 0;
};

}  // namespace gbolt

#endif  // INCLUDE_DATABASE_H_
