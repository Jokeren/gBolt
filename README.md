DataMining-gSpan
==============

gSpan algorithm in data mining

**Features:**

1. Support basic sub-structure mining.
2. Multi-thread. (200% speed-up than gSpan-64 provided by Yan)
3. Minimized memory consumption.

**Usage:**
	
    ./gspan -file [file_name] -support [support: float] &> log

###example:
Go to you src directory
```
cd ./src
```	
use make to generate the .exe file	
```	
make
```
run gaston on your graph.data with the support=0.1 and you will get the result on a file named log_03112015	
```	
./gspan -file graph.data -support 0.1 &> log_03112015
```

**Notice:**

1. You should use `&>` or other redirect operation since I have not included log-writer in this version, but I will update it in a month with other features. 

3. The parameter in the `common.h` file, called `THREAD_NUM`, is to control the number of threads. Currently I adopt a very coarse data seperation method. 


**Progress:**

1. Change parameter using patterns
2. Change log writter

12/4/2014

1. Reduce memory consumption.
2. Rearrange the gSpan class code.
3. Profling.
4. Multi-thread.
