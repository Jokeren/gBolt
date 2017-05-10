#include <gspan.h>
#include <database.h>
#include <common.h>
#include <gflags/gflags.h>

DEFINE_string(input_file, "", "Input path of graph data");
DEFINE_string(output_file, "", "Output gspan mining results");
DEFINE_double(support, 1.0, "Minimum subgraph frequency: (0.0, 1.0)");
DEFINE_string(separator, " ", "Graph data separator");
DEFINE_bool(parent, false, "Output subgraph parent ids");
DEFINE_bool(pattern, false, "Output subgraph patterns");

// Initialize instance
using gspan::Database;
Database *Database::instance_ = new Database();

int main(int argc, char *argv[]) {
  std::string version_string = GSPAN_VERSION_MAJOR + "." + GSPAN_VERSION_MINOR;
  gflags::SetVersionString(version_string);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_input_file == "") {
    LOG(FATAL) << "Input file should not be empty";
  }
  if (FLAGS_support > 1.0 || FLAGS_support <= 0.0) {
    LOG(FATAL) << "Support value should be less than 1.0 and greater than 0.0";
  }
  // Read input
  #ifdef GSPAN_PERFORMANCE  
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  Database::get_instance()->read_input(FLAGS_input_file, FLAGS_separator);

  // Construct algorithm
  #ifdef GSPAN_PERFORMANCE  
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "GSPAN read input time: " << elapsed; 
  CPU_TIMER_START(elapsed, time_start);
  #endif
  gspan::GSpan gspan(FLAGS_output_file, FLAGS_support);
  gspan.execute();

  // Save results
  if (FLAGS_output_file.size() != 0) {
    #ifdef GSPAN_PERFORMANCE  
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG(INFO) << "GSPAN execute time: " << elapsed;
    CPU_TIMER_START(elapsed, time_start);
    #endif
    gspan.save(FLAGS_parent, FLAGS_pattern);
    #ifdef GSPAN_PERFORMANCE  
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG(INFO) << "GSPAN save output time: " << elapsed;
    #endif
  }
  return 0;
}
