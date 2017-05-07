#include <output.h>
#include <fstream>

namespace gspan {

void Output::push_back(const string &str) {
  buffer_.push_back(str);  
}

void Output::push_back(const string &str, size_t nsupport) {
  support_.push_back(nsupport);
  buffer_.push_back(str);  
}

void Output::push_back(const string &str, size_t nsupport, int parent_id) {
  support_.push_back(nsupport);
  parent_.push_back(parent_id);
  buffer_.push_back(str);  
}

void Output::push_back(const string &str, size_t nsupport, size_t graph_id, int parent_id) {
  graph_id_.push_back(graph_id);
  support_.push_back(nsupport);
  parent_.push_back(parent_id);
  buffer_.push_back(str);  
}

void Output::save() {
  std::ofstream out(output_file_.c_str());

  for (size_t i = 0; i < buffer_.size(); ++i) {
    out << "t # " << graph_id_[i] << " * " << support_[i] << std::endl;
    if (parent_[i] == -1)
      out << "parent : -1" << std::endl;
    else
      out << "parent : " << parent_[i] << std::endl;
    out << buffer_[i] << std::endl;
  }
  out.close();
}

}  // namespace gspan
