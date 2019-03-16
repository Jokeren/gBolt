export OMP_NUM_THREADS=1

SUPPORT=(0.3 0.2 0.1 0.08)
ANSWER=(120 932 15965 28551)


for((i=0;i<${#SUPPORT[@]};++i))
do
  ./build/gbolt -s ${SUPPORT[$i]} -i extern/data/Compound_422 -o /tmp/gbolt_tmp
  RES=`cat /tmp/gbolt_tmp | wc -l`
  if [[$RES $ne ${ANSWER[$i]}]]; then
    exit 1
  fi
done
