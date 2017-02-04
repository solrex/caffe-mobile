Caffe on Mobile Devices
==================

TODO

Build
==================

All platform:

```
$ git clone --recursive https://github.com/solrex/caffe-compact.git
$ cd caffe-compact
$ mkdir build
```

OSX 1: Use brew installed protobuf

```
$ cd build
$ cmake ..
$ make -j 4
```

OSX 2: Use self built protobuf

```
$ cd third_party
$ ./build-protobuf-3.1.0.sh
$ cd ../build
$ cmake .. -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```

iPhoneSimulator:

```
$ cd third_party
$ ./build-protobuf-3.1.0.sh
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=SIMULATOR -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```

iPhone:

```
$ cd third_party
$ ./build-protobuf-3.1.0.sh
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=OS -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```
