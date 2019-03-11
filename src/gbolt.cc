#include <gbolt.h>
#include <database.h>
#include <common.h>
#include <cxxopts.hpp>

// Initialize instance
using gbolt::Database;
Database *Database::instance_ = new Database();

int main(int argc, char *argv[]) {
  cxxopts::Options options("gBolt", "very fast implementation for gSpan algorithm in data mining");
  options.add_options()
    ("i,input", "Input path of graph data", cxxopts::value<std::string>())
    ("o,output", "Output gbolt mining results", cxxopts::value<std::string>()->default_value(""))
    ("s,support", "Minimum subgraph frequency: (0.0, 1.0]", cxxopts::value<double>()->default_value("1.0"))
    ("m,mark", "Graph data separator", cxxopts::value<std::string>()->default_value(" "))
    ("p,parents", "Output subgraph parent ids",
     cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("d,dfs", "Output subgraph dfs patterns",
     cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("n,nodes", "Output frequent nodes",
     cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("h,help", "gBolt help");

  if (argc == 1) {
    LOG_INFO(options.help().c_str());
    return 0;
  }

  auto result = options.parse(argc, argv);

  if (result["help"].count() || result["h"].count()) {
    LOG_INFO(options.help().c_str());
    return 0;
  }

  std::string input = result["i"].count() ? result["i"].as<std::string>() : result["input"].as<std::string>();
  std::string output = result["o"].count() ? result["o"].as<std::string>() : result["output"].as<std::string>();
  double support = result["s"].count() ? result["s"].as<double>() : result["support"].as<double>();
  std::string mark = result["m"].count() ? result["m"].as<std::string>() : result["mark"].as<std::string>();
  bool parents = result["p"].count() ? result["p"].as<bool>() : result["parents"].as<bool>();
  bool dfs = result["d"].count() ? result["d"].as<bool>() : result["dfs"].as<bool>();
  bool nodes = result["n"].count() ? result["n"].as<bool>() : result["nodes"].as<bool>();

  if (support > 1.0 || support <= 0.0) {
    LOG_ERROR("Support value should be less than 1.0 and greater than 0.0");
  }
  // Read input
  #ifdef GBOLT_PERFORMANCE
  struct timeval time_start, time_end;
  double elapsed = 0.0;
  CPU_TIMER_START(elapsed, time_start);
  #endif
  Database::get_instance()->read_input(input, mark);
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG_INFO("gbolt read input time: %f", elapsed);
  CPU_TIMER_START(elapsed, time_start);
  #endif
  // Construct algorithm
  gbolt::GBolt gbolt(output, support);
  gbolt.execute();
  #ifdef GBOLT_PERFORMANCE
  CPU_TIMER_END(elapsed, time_start, time_end);
  LOG_INFO("gbolt execute time: %f", elapsed);
  #endif
  // Save results
  if (output.size() != 0) {
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_START(elapsed, time_start);
    #endif
    gbolt.save(parents, dfs, nodes);
    #ifdef GBOLT_PERFORMANCE
    CPU_TIMER_END(elapsed, time_start, time_end);
    LOG_INFO("gbolt save output time: %f", elapsed);
    #endif
  }
  return 0;
}
