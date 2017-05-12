#include <gbolt.h>
#include <database.h>
#include <common.h>
#include <gflags/gflags.h>

DEFINE_string(input_file, "", "Input path of graph data");
DEFINE_string(output_file, "", "Output gbolt mining results");
DEFINE_double(support, 1.0, "Minimum subgraph frequency: (0.0, 1.0)");
DEFINE_string(separator, " ", "Graph data separator");
DEFINE_bool(parent, false, "Output subgraph parent ids");
DEFINE_bool(pattern, false, "Output subgraph patterns");

// Initialize instance
using gbolt::Database;
Database *Database::instance_ = new Database();

int main(int argc, char *argv[]) {
  std::string version_string = GBOLT_VERSION_MAJOR + "." + GBOLT_VERSION_MINOR;
  gflags::SetVersionString(version_string);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_input_file == "") {
    LOG(FATAL) << "Input file should not be empty";
  }
  if (FLAGS_support > 1.0 || FLAGS_support <= 0.0) {
    LOG(FATAL) << "Support value should be less than 1.0 and greater than 0.0";
  }
  // Read input
  #ifdef GBOLT_PERFORMANCE
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  Database::get_instance()->read_input(FLAGS_input_file, FLAGS_separator);

  // Construct algorithm
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "gbolt read input time: " << elapsed;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  gbolt::GBolt gbolt(FLAGS_output_file, FLAGS_support);
  gbolt.execute();

  // Save results
  if (FLAGS_output_file.size() != 0) {
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG(INFO) << "gbolt execute time: " << elapsed;
    CPU_TIMER_START(elapsed, time_start);
    #endif
    gbolt.save(FLAGS_parent, FLAGS_pattern);
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG(INFO) << "gbolt save output time: " << elapsed;
    #endif
  }
  return 0;
}
