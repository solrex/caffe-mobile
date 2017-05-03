#!/bin/bash

cd ./third_party
echo $PWD
./build-protobuf-3.1.0.sh iPhoneOS
mkdir ../build
cd ../build
echo $PWD
cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
    -DIOS_PLATFORM=OS -DTHIRD_PARTY=1
make -j 4
