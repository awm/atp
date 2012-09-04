#!/bin/sh
# to be run in the base ctemplate directory

mkdir -p build
cd build

export CFLAGS="-O2 -arch x86_64 -arch i386"
export CXXFLAGS="-O2 -arch x86_64 -arch i386"
export LDFLAGS="-arch x86_64 -arch i386"
../configure --disable-shared --prefix=`pwd`/install --disable-dependency-tracking && make && make install
