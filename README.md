GBolt
==============

GBolt--very fast implementation for gSpan algorithm in data mining

## Features

GBolt has gained 25x speedup in comparison to Yan's original implementation with multi-threading, even 3-7 times faster with a single thread. We list its relative performance by following two pictures:

GBolt is ***fast*** as:

1. Adopt task parallel programming.
2. Incorporate **C++11** hastable and hashset.
3. Use contiguous memory storage.
4. Use partial pruning.

GBolt is ***memory efficient*** as:

1. Incorporate **C++11** emplace_back method.
2. Reconstruct a graph with frequent edges and nodes before mining.
3. Reuse memory to store temporal data.

GBolt is ***correct*** as:

1. We ran experiment for `data/Compound_422` and `data/Chemical_340` with minimal support from 0.1 to 0.9, and the results are exactly the same as Yan's gSpan-64. 

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
    
## Extension

GBolt is designed for efficiency, so we have not developed utilities for it. If you want `Python`, `C`, or Graphical interfaces, please contact us. We will be glad if you could contribute to ideas and codes for GBolt.
