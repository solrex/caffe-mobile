#!/bin/bash

cd ./third_party
./build-protobuf-3.1.0.sh iPhoneOS
mkdir ../build
cd ../build
rm -rf *
cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
    -DIOS_PLATFORM=OS
make -j 4
