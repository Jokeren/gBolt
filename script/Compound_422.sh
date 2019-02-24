export OMP_NUM_THREADS=1
./build/gbolt -support 0.2 -input_file extern/data/Compound_422 -output_file /tmp/gbolt_tmp -nodes -pattern
RES1=`cat extern/results/Compound_422.output.t0-0.2 | wc -l`
RES2=`cat /tmp/gbolt_tmp | wc -l`
if [[$RES1 $ne $RES2]]; then
  exit 1
fi
