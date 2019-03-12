cmake_minimum_required (VERSION 2.6)
project (gbolt)
# The version number.
set(GBOLT_VERSION_MAJOR 1)
set(GBOLT_VERSION_MINOR 0)
set(GBOLT_PERFORMANCE 1)
set(GBOLT_SERIAL 0)

# Mode
set(CMAKE_BUILD_TYPE RelWithDebInfo)
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
set(CXXOPTS_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/extern/cxxopts/include")

INCLUDE_DIRECTORIES(${GBOLT_INCLUDE_DIR} ${CXXOPTS_INCLUDE_DIR})

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

find_library(JEMALLOC NAMES jemalloc PATHS ${JEMALLOC_DIR} PATH_SUFFIXES lib NO_DEFAULT_PATH)
if (JEMALLOC)
  TARGET_LINK_LIBRARIES(gbolt ${JEMALLOC})
endif()

