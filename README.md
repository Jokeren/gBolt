GBolt
==============

GBolt--very fast implementation for gSpan algorithm in data mining

## Install

#### Prerequisites

- *g++* or *icc* with **C++11** support
- *glog*: `sudo apt-get install libgoogle-glog-*` or install from [source](https://github.com/google/glog).
- *gflags*: `sudo apt-get install libgflags-*` or install from [source](https://github.com/gflags/gflags).
- *cmake*: `sudo apt-get install cmake` or install from [source](https://cmake.org/).
- *openmp* environment

#### Steps

Build gbolt:

    mkdir build && cd build
    cmake ..
    make
    
Run an example:

    ./build/gbolt -input_file data/Chemical_340 -support 0.2 
    
Arguments help:

    ./build/gbolt -helpshort

Multi-thread support:

    export OMP_NUM_THREADS=<hardware threads num for recommendation>
