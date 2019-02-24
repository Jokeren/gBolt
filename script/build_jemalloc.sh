cd ..
PWD=`pwd`
rm -rf install/jemalloc

# jemalloc
cd extern/jemalloc
./autogen.sh
./configure --prefix=${PWD}/../../install/jemalloc --enable-shared
make -j4
make install
