cGBolt
==============

GBolt--very fast implementation for gSpan algorithm in data mining

## Features

GBolt is both memory and speed efficient, and it has gained 25x speedup in comparison to Yan's original implementation with multi-threading, even 3-7 times faster with a single thread. We list its relative performance by following two pictures:

GBolt is fast and efficient with the following improvments:

1. Adopt task parallel programming.
2. Incorporate **C++11** hastable and hashset.
3. Incorporate **C++11** emplace_back method.
4. Use contiguous memory storage.
5. Use partial pruning.
6. Reconstruct a graph with frequent edges and nodes before mining.
7. Reuse memory to store temporal data.

## Install

#### Prerequisites

- *g++* or *icc* with **C++11** support.
- *glog*: `sudo apt-get install libgoogle-glog-*` or install from [source](https://github.com/google/glog).
- *gflags*: `sudo apt-get install libgflags-*` or install from [source](https://github.com/gflags/gflags).
- *cmake*: `sudo apt-get install cmake` or install from [source](https://cmake.org/).
- *openmp* environment.

#### Steps

    mkdir build && cd build
    cmake ..
    make
    
## Usage

Run an example:

    ./build/gbolt -input_file data/Chemical_340 -support 0.2 
    
Arguments help:

    ./build/gbolt -helpshort

Multi-thread support:

    export OMP_NUM_THREADS=<hardware threads num for recommendation>
