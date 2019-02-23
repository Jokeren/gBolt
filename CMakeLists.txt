cmake_minimum_required (VERSION 2.6)
project (gbolt)
# The version number.
set(GBOLT_VERSION_MAJOR 0)
set(GBOLT_VERSION_MINOR 1)
set(GBOLT_PERFORMANCE 1)

# Mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -std=c++11")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -g -std=c++11")

# Packages
find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
set(GBOLT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GLOG_DIR "${PROJECT_SOURCE_DIR}/install/glog")
set(GFLAGS_DIR "${PROJECT_SOURCE_DIR}/install/gflags")

INCLUDE_DIRECTORIES(${GBOLT_INCLUDE_DIR} ${GLOG_DIR}/include ${GFLAGS_DIR}/include)

set(GBOLT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(SOURCES
  "${GBOLT_SOURCE_DIR}/history.cc"
  "${GBOLT_SOURCE_DIR}/database.cc"
  "${GBOLT_SOURCE_DIR}/output.cc"
  "${GBOLT_SOURCE_DIR}/gbolt.cc"
  "${GBOLT_SOURCE_DIR}/gbolt_count.cc"
  "${GBOLT_SOURCE_DIR}/gbolt_execute.cc"
  "${GBOLT_SOURCE_DIR}/gbolt_extend.cc"
  "${GBOLT_SOURCE_DIR}/gbolt_mine.cc")
 
# add the executable
add_executable(gbolt ${SOURCES})

find_library(GLOG NAMES glog PATHS ${GLOG_DIR} PATH_SUFFIXES lib NO_DEFAULT_PATH)
if (NOT GLOG)
  message(FATAL_ERROR "glog not found")
endif()

find_library(GFLAGS NAMES gflags PATHS ${GFLAGS_DIR} PATH_SUFFIXES lib NO_DEFAULT_PATH)
if (NOT GFLAGS)
  message(FATAL_ERROR "gflags not found")
endif()

TARGET_LINK_LIBRARIES(gbolt ${GLOG} ${GFLAGS})
