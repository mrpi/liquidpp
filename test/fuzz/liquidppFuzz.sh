#!/bin/bash

#rm -rf buildFuzz
#mkdir buildFuzz
#cd buildFuzz

export CC=clang-3.9
export CXX=clang++-3.9

cmake -DCMAKE_CXX_FLAGS="-fsanitize-coverage=edge -fsanitize=address -g -O3" ..
make -j4 liquidppFuzz

./test/fuzz/liquidppFuzz -timeout=2 ../build/cases/

#cd ..
