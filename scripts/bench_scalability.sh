#!/bin/bash

THREADS=(1 2 4 8 16 32 64)

for((i=0;i<${#THREADS[@]};++i))
do
  export OMP_NUM_THREADS=${THREADS[$i]}
  ./build/gbolt -i extern/data/Compound_422 -s 0.06
done
