#include <gbolt.h>
#include <database.h>
#include <common.h>
#include <gflags/gflags.h>

DEFINE_string(i, "", "Input path of graph data");
DEFINE_string(o, "", "Output gbolt mining results");
DEFINE_double(s, 1.0, "Minimum subgraph frequency: (0.0, 1.0)");
DEFINE_string(sep, " ", "Graph data separator");
DEFINE_bool(p, false, "Output subgraph parent ids");
DEFINE_bool(d, false, "Output subgraph dfs patterns");
DEFINE_bool(n, false, "Output frequent nodes");

// Initialize instance
using gbolt::Database;
Database *Database::instance_ = new Database();

int main(int argc, char *argv[]) {
  std::string version_string = GBOLT_VERSION_MAJOR + "." + GBOLT_VERSION_MINOR;
  google::SetVersionString(version_string);
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_i == "") {
    LOG(FATAL) << "Input file should not be empty";
  }
  if (FLAGS_s > 1.0 || FLAGS_s <= 0.0) {
    LOG(FATAL) << "Support value should be less than 1.0 and greater than 0.0";
  }
  // Read input
  #ifdef GBOLT_PERFORMANCE
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  Database::get_instance()->read_input(FLAGS_i, FLAGS_sep);
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "gbolt read input time: " << elapsed;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  // Construct algorithm
  gbolt::GBolt gbolt(FLAGS_o, FLAGS_s);
  gbolt.execute();
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG(INFO) << "gbolt execute time: " << elapsed;
  #endif
  // Save results
  if (FLAGS_o.size() != 0) {
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_START(elapsed, time_start);
    #endif
    gbolt.save(FLAGS_p, FLAGS_d, FLAGS_n);
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG(INFO) << "gbolt save output time: " << elapsed;
    #endif
  }
  return 0;
}
