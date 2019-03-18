
## Experiments

### Platform

We ran experiments on a *E5-2695 v4* machine with two sockets, rendering 72 hardware threads with hyper-threading enabled. The machine we used has *256GB* main memory.

The results showed below used `extern/data/Compound_422`. Memory consumption data are taken from Valgrind. 

### Performance

Table 1. Single Thread Performance Comparison

Support  | gSpan Time/s | gBolt Time/s | Speedup | gSpan Mem/bytes | gBolt Mem/bytes | Reduction | Subgraphs
:-----: | :---:  | :---:  | :---: | :----: | :----: | :---: | :----: |
0.5   | 0.11 | 0.04 | 2.75x   |       |      | | 32    |
0.4   | 0.15 | 0.04 | 3.75x |       |      | | 60    |
0.3   | 0.21 | 0.05  | 4.20x  |       |      | | 124   |
0.2   | 0.79 | 0.11  | 7.18x |       |      | | 936   |
0.1   | 7.27 | 0.76  | 9.56x |       |      | | 15972 |
0.08  | 11.4 | 1.18  | 9.66x | ~3.76G | ~49.6M | 76x  | 28558 |
0.06  | 143.5 | 20.8  | 6.99x | ~37.3G | ~187M | 200x | 65259 |

### Scalability

Table 2. Multi-thread Scalability Comparison (support=0.06)

Threads  | gSpan Time/s | gBolt Time/s | Speedup | gSpan Scale | gBolt Scale 
:-----:  | :--------:   | :--------:   | :---:   | :------: | :------: | 
1        | 143.2        | 20.8          | 6.8x    |   1      |   1   |
2        | 103.6        | 10.8          | 9.6x    |   1.4     |   1.9   | 
4        | 85.3         | 6.42          | 13.3x    |   1.2    |   1.7 |
8        | 75.1         | 3.52          | 21.3x    |   1.1    |   1.8 | 
16       | 65.4         | 2.10         | 31.1x   |   1.2    |   1.7 | 
32       | 63.2         | 1.12         | 56.4x   |   1.0   |   1.9 | 
64       | 119.9        | 0.90         | 133.2x   |   0.5   |   1.2 |

### Comparison with [gaston](http://liacs.leidenuniv.nl/~nijssensgr/gaston/download.html)

Table 3. Single Thread Performance Comparison

Support  | gaston Time/s | gBolt Time/s | Speedup | gaston Mem/bytes | gBolt Mem/bytes | Reduction | Subgraphs
:-----: | :---:  | :---:  | :---: | :----: | :----: | :---: | :----: |
0.5   | 0.01 | 0.04 | 0.25x   |       |      | | 32    |
0.4   | 0.01 | 0.04 | 0.25x |       |      | | 60    |
0.3   | 0.01 | 0.05  | 0.2x  |       |      | | 124   |
0.2   | 0.03 | 0.11  | 0.27x |       |      | | 936   |
0.1   | 0.21 | 0.76  | 0.27x |       |      | | 15972 |
0.08  | 0.33 | 1.18  | 0.27x | ~6M | ~49.6M | 0.12x  | 28558 |
0.06  | 4.59 | 20.8  | 0.22x | ~8M | ~187M | 0.04x | 65259 |

With a single thread, gaston is faster than gBolt because it handles paths, trees, and graphs separately. Currently, gaston has two shortcomings as it does not support multi-threading and lacks parent graph support.

We would like to do more research in the future on subgraph mining and invent new algorithms for large scale graphs.
