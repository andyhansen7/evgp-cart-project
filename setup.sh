#!/usr/bin/env bash

set -euxo pipefail

rm -rf build

mkdir build
cd build

mkdir debug
mkdir release

cd release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cd ..

cd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd ..
