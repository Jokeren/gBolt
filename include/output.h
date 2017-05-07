#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <common.h>

namespace gspan {

class Output {
 public:
  Output(const string &output_file): output_file_(output_file) {}

  size_t size() const {
    return support_.size();
  }

  void push_back(const string &str);

  void push_back(const string &str, size_t nsupport);

  void push_back(const string &str, size_t nsupport, int parent_id);

  void push_back(const string &str, size_t nsupport, size_t graph_id, int parent_id);

  void save();

 private:
  vector<string> buffer_;
  vector<size_t> support_;
  vector<int> parent_;
  vector<size_t> graph_id_;
  const string &output_file_;
};

}  // namespace gspan

#endif  // OUTPUT_H_
