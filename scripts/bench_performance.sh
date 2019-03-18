#!/bin/bash

SUPPORT=(0.5 0.4 0.3 0.2 0.1 0.08 0.06)

for((i=0;i<${#SUPPORT[@]};++i))
do
  export OMP_NUM_THREADS=1
  ./build/gbolt -i extern/data/Compound_422 -s ${SUPPORT[$i]}
done

