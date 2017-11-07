#!/bin/bash
# Exit on any error

PROJECT_ROOT=$PWD

function merge-Protobuf {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Merging .a for iPhoneSimulator and iPhoneOS"
    echo "#####################"
    echo "$(tput sgr0)"
    set -x
    rm -rf ios-protobuf
    cp -r iPhoneOS-protobuf ios-protobuf
    rm -rf ios-protobuf/lib/*
    lipo -create iPhoneSimulator-protobuf/lib/libprotobuf-lite.a iPhoneOS-protobuf/lib/libprotobuf-lite.a -output ios-protobuf/lib/libprotobuf-lite.a
    lipo -create iPhoneSimulator-protobuf/lib/libprotobuf.a iPhoneOS-protobuf/lib/libprotobuf.a -output ios-protobuf/lib/libprotobuf.a
    set +x
}

cd $PROJECT_ROOT/third_party
./build-protobuf-3.1.0.sh iPhoneOS
./build-protobuf-3.1.0.sh iPhoneSimulator
merge-Protobuf

mkdir -p $PROJECT_ROOT/build_iPhoneOS
cd $PROJECT_ROOT/build_iPhoneOS
rm -rf *
cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
    -DIOS_PLATFORM=OS
make -j 4

mkdir -p $PROJECT_ROOT/build_iPhoneSimulator
cd $PROJECT_ROOT/build_iPhoneSimulator
rm -rf *
cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
    -DIOS_PLATFORM=SIMULATOR
make -j 4

cd $PROJECT_ROOT
set -x
rm -rf build_ios
mkdir -p build_ios/lib
cp -r build_iPhoneOS/include build_ios/include
lipo -create build_iPhoneOS/lib/libcaffe.a build_iPhoneSimulator/lib/libcaffe.a -output build_ios/lib/libcaffe.a
lipo -create build_iPhoneOS/lib/libproto.a build_iPhoneSimulator/lib/libproto.a -output build_ios/lib/libproto.a
set +x
