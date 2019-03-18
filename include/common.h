#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <sys/time.h>
#ifndef GBOLT_SERIAL
#include <omp.h>
#endif
#include <config.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>
#include <vector>

#define FILE_MAX_LINE 1024

#define DEFAULT_PATH_LEN 16

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

#define LOG_INFO(...) \
  do { \
    struct timeval cur; \
    gettimeofday(&cur, NULL); \
    fprintf(stdout, "[%d:%d] ", cur.tv_sec, cur.tv_usec); \
    fprintf(stdout, "INFO: "); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n"); \
  } while (0)

#define LOG_WARNING(...) \
  do { \
    struct timeval cur; \
    gettimeofday(&cur, NULL); \
    fprintf(stdout, "[%d:%d] ", cur.tv_sec, cur.tv_usec); \
    fprintf(stderr, "ERROR: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
  } while (0)

#define LOG_ERROR(...) \
  do { \
    struct timeval cur; \
    gettimeofday(&cur, NULL); \
    fprintf(stdout, "[%f:%f] ", cur.tv_sec, cur.tv_usec); \
    fprintf(stderr, "ERROR: "); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit(1); \
  } while (0)

namespace gbolt {

using std::map;
using std::unordered_map;
using std::string;
using std::set;
using std::unordered_set;
using std::vector;

}  // namespace gbolt

#endif  // INCLUDE_COMMON_H_
