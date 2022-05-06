#!/bin/sh

set -eux

GCC_COMMAND=${C_COMPILER:-"gcc"}
GXX_COMMAND=${CXX_COMPILER:-"g++"}
OPT_FLAGS=${QULACS_OPT_FLAGS:-"-mtune=native -march=native"}

rm -rf ./build
mkdir -p ./build
cd ./build
GCC_COMMAND="emcc"
GXX_COMMAND="em++"
emcmake cmake -D CMAKE_C_COMPILER=$GCC_COMMAND -D CMAKE_CXX_COMPILER=$GXX_COMMAND -D OPT_FLAGS="$OPT_FLAGS" -D CMAKE_BUILD_TYPE=Release -D USE_GPU:STR=No -D "Boost_INCLUDE_DIR=../../boost" ..
make -j $(nproc)
cd ../
