
## Experiments

### Platform

We ran experiments on *E5-2695 v4* with two sockets, rendering 72 hardware threads with hyper-threading enabled. The machine we used has *256GB* main memory.

The results showed below used `extern/data/Compound_422`.

### Performance

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

### Scalability

Table 2. Multi-thread Scalability Comparison (support=0.06)

Threads  | gSpan Time/s | gBolt Time/s | Speedup | gSpan Scale | gBolt Scale 
:-----:  | :--------:   | :--------:   | :---:   | :------: | :------: | 
1        | 38.3         | 5.3          | 6.2x    |   1      |   1   | 
2        | 22.1         | 2.9          | 6.6x    |   1.7    |   1.8 |
4        | 16.4         | 1.5          | 9.9x    |   1.3    |   1.9 | 
8        | 13.7         | 0.85         | 15.1x   |   1.2    |   1.8 | 
16       | 14.1         | 0.46         | 29.6x   |   0.97   |   1.8 | 
32       | 15.0         | 0.40         | 36.5x   |   0.94   |   1.2 |

### Comparison with gaston

With single thread, gaston is faster than gBolt because it handles paths, trees, and graphs separately. Currently, gaston has two shortcomings as it does not support multi-threading and lacks parent graph support.

We would like to do more research in the future on subgraph mining and invent new algorithms for large scale graphs.
