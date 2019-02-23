#ifndef INCLUDE_OUTPUT_H_
#define INCLUDE_OUTPUT_H_

#include <common.h>
#include <vector>
#include <string>

namespace gbolt {

class Output {
 public:
  explicit Output(const string &output_file): output_file_(output_file) {}

  int size() const {
    return support_.size();
  }

  void push_back(const string &str, int nsupport, int graph_id);

  void push_back(const string &str, int nsupport, int graph_id, int thread_id, int parent_id);

  void save(bool output_parent = false, bool output_pattern = false);

 private:
  vector<string> buffer_;
  vector<int> support_;
  vector<int> thread_id_;
  vector<int> parent_id_;
  vector<int> graph_id_;
  const string output_file_;
};

}  // namespace gbolt

#endif  // INCLUDE_OUTPUT_H_
