***gBolt***
==============

***gBolt***--very fast implementation for gSpan algorithm in data mining

## Features

***gBolt*** has gained 35x speedup in comparison to [Yan's](https://www.cs.ucsb.edu/~xyan/software/gSpan.htm) original implementation with multi-threading, even 1-6 times faster with a single thread. It also reduces more than 100 folds memory usage, which make it feasible to be adopted on personal computers.  

***gBolt*** is ***fast***:

1. Adopt task parallel programming.
2. Incorporate **C++11** hastable and hashset.
3. Use contiguous memory storage.
4. Use partial pruning.

***gBolt*** is ***memory efficient***:

1. Incorporate **C++11** emplace_back method.
2. Reconstruct a graph with frequent edges and nodes before mining.
3. Reuse memory to store temporal data.

***gBolt*** is ***correct***:

1. We ran experiment for `data/Compound_422` and `data/Chemical_340` with minimal support from 0.1 to 0.9, and the results are exactly the same as Yan's gSpan-64. 

## Experiments

### Platform

We ran experiments on *E5-2670v2* with two sockets, rendering 32 hardware threads with hyper-threading enabled. Besides, the machine we used has *64GB* main memory.

### Results

We list its relative performance by following two tables:

Table 1. Single Thread Performance Comparison

Support  | gSpan Time/s | gBolt Time/s | Speedup | gSpan Mem/byte | gBolt Mem/byte | Reduction | Subgraphs
:-----: | :---:  | :---:  | :---: | :----: | :----: | :---: | :----: |
0.5   | 0.16 | 0.08 | 1x   |       |      | | 32    |
0.4   | 0.2  | 0.09 | 1.2x |       |      | | 60    |
0.3   | 0.3  | 0.1  | 2x   |       |      | | 124   |
0.2   | 1.1  | 0.2  | 4.5x |       |      | | 936   |
0.1   | 10.8 | 1.5  | 6.2x |       |      | | 15972 |
0.09  | 11.7 | 1.7  | 5.8x | ~2.1G | ~37M | 55x  | 17511 |
0.08  | 17.1 | 2.4  | 6.1x | ~3.3G | ~35M | 93x  | 28558 |
0.07  | 38.3 | 5.3  | 6.2x | ~7.6G | ~55M | 137x | 65259 |

In **Table 1**'s *Subgraphs* field, we have included graphs that only contain a single node. Because for support greater than *0.1*, ***gBolt*** terminates very quickly, we have not logged its memory consumption.

Table 2. Multi-thread Scalability Comparison


Threads  | gSpan Time/s | gBolt Time/s | Speedup | gSpan Scale | gBolt Scale 
:-----:  | :--------:   | :--------:   | :---:   | :------: | :------: | 
1        | 38.3         | 5.3          | 6.2x    |   1      |   1   | 
2        | 22.1         | 2.9          | 6.6x    |   1.7    |   1.8 |
4        | 16.4         | 1.5          | 9.9x    |   1.3    |   1.9 | 
8        | 13.7         | 0.85         | 15.1x   |   1.2    |   1.8 | 
16       | 14.1         | 0.46         | 29.6x   |   0.97   |   1.8 | 
32       | 15.0         | 0.40         | 36.5x   |   0.94   |   1.2 |


**Table 2** shows the results of mining `data/Compound_422` with `support = 0.07`.

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
    
## Reference
    
## Extension

***gBolt** is designed for efficiency, so we have not developed utilities for it. If you want `Python`, `C`, or Graphical interfaces, please contact us. We will be glad if you could contribute ideas and codes for ***gBolt***.
