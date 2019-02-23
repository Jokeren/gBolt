PWD=`pwd`
rm -rf install
mkdir install

# glog
cd extern/glog
./autogen.sh
./configure --prefix=${PWD}/../../install/glog --enable-shared
make -j4
make install

# gflags
cd ../gflags
./configure --prefix=${PWD}/../../install/gflags --enable-shared
make -j4
make install

cd ../..

# gBolt
rm -rf build
mkdir build && cd build
cmake ..
make -j4
