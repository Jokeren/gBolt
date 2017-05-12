#OPENMP
export OMP_MAX_THREADS=$1
export OMP_NUM_THREADS=$1
export KMP_AFFINITY=granularity=fine,compact,1,0
export OMP_DYNAMIC="FALSE" 
