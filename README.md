DataMining-gSpan
==============

gSpan algorithm in data mining

**Features:**

1. Support basic sub-structure mining.
2. Multi-thread. (200% speed-up than gSpan-64 provided by Yan)
3. Minimized memory consumption.

**Usage:**
	
	cd src
    ./gspan -file [file_name] -output [output_file_name] -support [support: float]

or

	./script/Chemical_340.sh

###example:
Go to you src directory
```
cd ./src
```	
use make to generate the .exe file	
```	
make
```
run gaston on your graph.data with the support=0.1 and you will get the result in Chemical.output	
```	
./gspan -file ../data/Chemical_340 -output Chemical.output -support 0.1
```

**Notice:**

1. The parameter in the `common.h` file, called `THREAD_NUM`, is to control the number of threads. Currently I adopt a very coarse data seperation method. 

**Progress:**

Future:

1. Refractor

3/20/2016

1. Add different metrics
2. Memory consumption bug fixed
3. Write result to disk

12/4/2014

1. Change parameter using patterns
2. Change log writter
