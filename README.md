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
$ cd build
```

OSX:

```
$ cmake ..
$ make -j 4
```

iPhoneSimulator:

```
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=SIMULATOR -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf/protobuf
$ make -j 4
```

iPhone:

```
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=OS -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf/protobuf
$ make -j 4
```
