#ifndef DATABASE_H_
#define DATABASE_H_

#include <common.h>

namespace gspan {

class Graph;

class Database {
 public:
  static Database *get_instance() {
    return instance_;
  }

  void read_input(const string &input_file, const string &separator);

  // construct graph
  void construct_graphs(vector<Graph> &graphs);

  // construct graph by frequent labels
  void construct_graphs(const unordered_map<size_t, size_t> &frequent_labels, vector<Graph> &graphs);

  ~Database() {
    delete instance_;
  }

 private:
  Database() {};

 private:
  static Database *instance_;
  vector<vector<string> > input_;
};

}  // namespace gspan

#endif  // DATABASE_H_
