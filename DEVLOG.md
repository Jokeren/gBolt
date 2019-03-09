### Progress

2/20/2019:

- [x] Projection mini opt
- [x] Customized data structure
  - Use Path to replace vector
  - Use Rope to replace Path
- [ ] Software simplicity
  - Use header only repo to replace gflags
  - Use customized logs
  - Compatible with CMake-2.8
- [ ] Test harness
  - Test basic data structures
- [x] Reduce memory overhead
  - Dfs code pointer optimization
  - Reuse right most path
  - jemalloc option
- [x] Build everything in a single command
- [x] Use hpctraceviewer to improve OpenMP performance
- [x] Change repo name
- [x] Travis CI
- [ ] Deterministic map for debugging
- [ ] Python interfaces
  - Merge results
  - Graph data structure
  - Execution
- [x] Valgrind sanity check
- [ ] Docs
  - Optimizations
  - Customized data structures
  - Test results
- [ ] Research problem: build graph on-the-fly or only in check?

5/13/2017:

- [ ] Interfaces?
- [ ] Reading graph in parallel?

5/12/2017:

- [x] Check correctness
- [x] Document

5/9/2017:

- Emplace_back Speedup: 0.02x
- Task Parallel Speedup: 8x
- History Memory Optimize Speedup: 2x
- Refractor Speedup: 0.2x

4/30/2017:

*Start refractoring following issues.*

- [x] Use gflags for command line arguments.
- [x] Use glogs for logs
- [x] Use cmake for makefiles
- [x] Modify code styles
- [x] Modify class constructors
- [x] Modify multi-thread patterns
- [x] Modify memory access patterns (change *clear()*, *resize()*, *emplace_back()*)
- [x] Change int types to size_t
- [x] Pass by references
- [x] Find frequent edge labels
- [x] Incoporate C++11 hashset and hashmap 
- [ ] Refine algorithm (adjust projectionmap structures)
- [ ] Heuristic multi-thread bound
- [ ] Travis CI support

3/20/2016

- Add various metrics
- Memory consumption bug fixed
- Write results to disk while processing

12/4/2014

- Change command line arguments
- Change log writter
