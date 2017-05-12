#include <output.h>
#include <fstream>

namespace gbolt {

void Output::push_back(const string &str, size_t nsupport, size_t graph_id, int parent_id) {
  graph_id_.push_back(graph_id);
  support_.push_back(nsupport);
  parent_.push_back(parent_id);
  buffer_.push_back(str);
}

void Output::save(bool output_parent, bool output_pattern) {
  std::ofstream out(output_file_.c_str());

  for (size_t i = 0; i < buffer_.size(); ++i) {
    out << "t # " << graph_id_[i] << " * " << support_[i] << std::endl;
    if (output_parent == true) {
      if (parent_[i] == -1)
        out << "parent : -1" << std::endl;
      else
        out << "parent : " << parent_[i] << std::endl;
    }
    if (output_pattern == true) {
      out << buffer_[i] << std::endl;
    }
  }
  out.close();
}

}  // namespace gbolt
