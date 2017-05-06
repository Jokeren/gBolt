cmake_minimum_required (VERSION 2.6)
project (gspan)
# The version number.
set (GSPAN_VERSION_MAJOR 0)
set (GSPAN_VERSION_MINOR 1)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/include/config.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
set(GSPAN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GSPAN_SOURCE_DIR "${PROJECT_SOURCE_DIR}/new_src")

include_directories(${GSPAN_INCLUDE_DIR})

set(HEADERS
  "${GSPAN_INCLUDE_DIR}/common.h"
  "${GSPAN_INCLUDE_DIR}/config.h"
  "${GSPAN_INCLUDE_DIR}/database.h"
  "${GSPAN_INCLUDE_DIR}/graph.h"
  "${GSPAN_INCLUDE_DIR}/gspan.h"
  "${GSPAN_INCLUDE_DIR}/history.h"
  "${GSPAN_INCLUDE_DIR}/output.h")

set(SOURCES
  "${GSPAN_SOURCE_DIR}/history.cc"
  "${GSPAN_SOURCE_DIR}/database.cc"
  "${GSPAN_SOURCE_DIR}/output.cc"
  "${GSPAN_SOURCE_DIR}/gspan.cc"
  "${GSPAN_SOURCE_DIR}/gspan_count.cc"
  "${GSPAN_SOURCE_DIR}/gspan_execute.cc"
  "${GSPAN_SOURCE_DIR}/gspan_extend.cc"
  "${GSPAN_SOURCE_DIR}/gspan_mine.cc")
 
# add the executable
add_executable(gspan ${SOURCES})
TARGET_LINK_LIBRARIES(gspan -lglog -lgflags)
