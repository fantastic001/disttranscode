#!/bin/bash

THIRD_PARTY_DIR="$(readlink -f $1)"

mkdir -p $THIRD_PARTY_DIR/ffmpeg 
mkdir -p $THIRD_PARTY_DIR/cmake 

echo "Third party directory: " $THIRD_PARTY_DIR

function build_ffmpeg() {
    ./configure  --prefix=$THIRD_PARTY_DIR/ffmpeg/ --disable-x86asm --enable-shared
    make 
    make install 
}


function build_cmake() {
    bootstrap 
    make 
    make install DESTDIR=$THIRD_PARTY_DIR/cmake
}

mkdir -p $THIRD_PARTY_DIR/build
cd $THIRD_PARTY_DIR/build
git clone https://github.com/Kitware/CMake
cd CMake 
build_cmake
cd .. 

git clone https://git.ffmpeg.org/ffmpeg.git 
cd ffmpeg 
build_ffmpeg

cd ../..



echo  "Build disttranscode as:"
echo  "PKG_CONFIG_PATH=$THIRD_PARTY_DIR/ffmpeg/lib/pkgconfig/:\$PKG_CONFIG_PATH LD_LIBRARY_PATH=$THIRD_PARTY_DIR/ffmpeg/lib/:\$LD_LIBRARY_PATH $THIRD_PARTY_DIR/cmake/usr/local/bin/cmake -DCMAKE_MODULE_PATH=$THIRD_PARTY_DIR/cmake/usr/local/share/cmake-3.23/Modules/ ."


