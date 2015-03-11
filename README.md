DataMining-gSpan
==============

gSpan algorithm in data mining

**Features:**

1. Support basic sub-structure mining.
2. Multi-thread. (200% speed-up than gSpan-64 provided by Yan)
3. Minimized memory consumption.

**Usage:**
	
    ./gspan -file [file_name] -support [support: float] &> log


example:

	cd ./src-multithread
	./gspan -file graph.data -support 0.1 &> log_03112015
	

**Notice:**

1. You should use `&>` or other redirect operation since I have not included log-writer in this version, but I will update it in a month with other features. 

2. Only `src-multithread` directory contains the most efficient codes, other directories are used for testing. I will remove them once I implemented all the expected features.

3. The parameter, called `NUM_THREAD`, is to control the number of threads in the `common.h` file. Currently I adopt a very coarse data seperation method. 


**Progress:**

1. MPI
2. Optimize the sequential version.
3. Detailed parallelism

12/4/2014

1. Reduce memory consumption.
2. Rearrange the gSpan class code.
3. Profling.
4. Multi-thread.
