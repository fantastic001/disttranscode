#!/bin/bash

THIRD_PARTY_DIR="$(readlink -f $1)"

mkdir -p $THIRD_PARTY_DIR/ffmpeg 
mkdir -p $THIRD_PARTY_DIR/cmake 
mkdir -p $THIRD_PARTY_DIR/clang 
mkdir -p $THIRD_PARTY_DIR/gcc



function setup_toolchain() {
    pushd .
    gcc_version=7.1.0
    wget https://ftp.gnu.org/gnu/gcc/gcc-${gcc_version}/gcc-${gcc_version}.tar.bz2
    wget https://ftp.gnu.org/gnu/gcc/gcc-${gcc_version}/gcc-${gcc_version}.tar.bz2.sig
    wget https://ftp.gnu.org/gnu/gnu-keyring.gpg
    signature_invalid=`gpg --verify --no-default-keyring --keyring ./gnu-keyring.gpg gcc-${gcc_version}.tar.bz2.sig`
    if [ $signature_invalid ]; then echo "Invalid signature" ; exit 1 ; fi
    tar -xvjf gcc-${gcc_version}.tar.bz2
    cd gcc-${gcc_version}
    ./contrib/download_prerequisites
    cd ..
    mkdir gcc-${gcc_version}-build
    cd gcc-${gcc_version}-build
    $PWD/../gcc-${gcc_version}/configure --prefix=$THIRD_PARTY_DIR/gcc --enable-languages=c,c++ --disable-multilib
    make -j$(nproc)
    make install
    popd 
}
echo "Third party directory: " $THIRD_PARTY_DIR

function build_ffmpeg() {
    ./configure  --prefix=$THIRD_PARTY_DIR/ffmpeg/ --disable-x86asm --enable-shared
    make 
    make install 
}


function build_cmake() {
    ./bootstrap 
    make 
    make install DESTDIR=$THIRD_PARTY_DIR/cmake
}

function build_clang() {
    mkdir build
    cd build 
    cmake \
        -DLLVM_ENABLE_PROJECTS=clang \
        -G "Unix Makefiles" \
        -DCMAKE_INSTALL_PREFIX=$THIRD_PARTY_DIR/clang \
        -DCMAKE_MODULE_PATH=$THIRD_PARTY_DIR/cmake/usr/local/share/cmake-3.23/Modules/ \
        ../llvm 
    make 
    make install
    cd .. 
}

mkdir -p $THIRD_PARTY_DIR/build
pushd $THIRD_PARTY_DIR/build


setup_toolchain
export CC=$THIRD_PARTY_DIR/gcc/bin/gcc
export CXX=$THIRD_PARTY_DIR/gcc/bin/g++

# CMake 
git clone https://github.com/Kitware/CMake
pushd CMake 
build_cmake
popd
export PATH=$THIRD_PARTY_DIR/cmake/usr/local/bin/:$PATH

# FFMpeg
git clone https://git.ffmpeg.org/ffmpeg.git 
pushd ffmpeg 
build_ffmpeg
popd
export PATH=$THIRD_PARTY_DIR/ffmpeg/bin:$PATH
export LD_LIBRARY_PATH=$THIRD_PARTY_DIR/ffmpeg/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$THIRD_PARTY_DIR/ffmpeg/lib/pkgconfig:$PKG_CONFIG_PATH

# Clang
git clone --depth=1 https://github.com/llvm/llvm-project.git
pushd llvm-project 
build_clang
popd


popd
export PATH=$THIRD_PARTY_DIR/clang/bin:$PATH
export LD_LIBRARY_PATH=$THIRD_PARTY_DIR/clang/lib:$LD_LIBRARY_PATH

echo  "Build disttranscode as:"
echo  "PKG_CONFIG_PATH=$THIRD_PARTY_DIR/ffmpeg/lib/pkgconfig/:\$PKG_CONFIG_PATH LD_LIBRARY_PATH=$THIRD_PARTY_DIR/ffmpeg/lib/:\$LD_LIBRARY_PATH $THIRD_PARTY_DIR/cmake/usr/local/bin/cmake -DCMAKE_MODULE_PATH=$THIRD_PARTY_DIR/cmake/usr/local/share/cmake-3.23/Modules/ ."


