#ifndef COMMON_H_
#define COMMON_H_

#include <map>
#include <string>
#include <set>
#include <vector>
#include <config.h>
#include <sys/time.h>
#include <glog/logging.h>

#define FILE_MAX_LINE 1024

#define CPU_TIMER_START(elapsed_time, t1) \
  do { \
    elapsed_time = 0.0; \
    gettimeofday(&t1, NULL); \
  } while (0) 

#define CPU_TIMER_END(elapsed_time, t1, t2) \
  do { \
    gettimeofday(&t2, NULL); \
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; \
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0; \
    elapsed_time /= 1000.0; \
  } while (0)

namespace gspan {

using std::map;
using std::string;
using std::set;
using std::vector;

}  // namespace gspan

#endif  // COMMON_H_
