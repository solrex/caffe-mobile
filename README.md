Caffe on Mobile Devices
==================

TODO

# For iPhone Simulator

## Step 1: Build Caffe-Mobile Lib with cmake&make

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ cd caffe-mobile/third_party
$ ./build-protobuf-3.1.0.sh
$ mkdir ../build
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=SIMULATOR -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```

## Step 2: Build Example iOS App: CaffeSimple with Xcode

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to CaffeSimple app directory.

```
$ cp $CAFFE/examples/mnist/lenet.prototxt $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/
```

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, and press Command-R to build and run it on the simulator.

# For iPhone
==================

## Step 1: Build Caffe-Mobile Lib with cmake&make

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ cd caffe-mobile/third_party
$ ./build-protobuf-3.1.0.sh
$ mkdir ../build
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=OS -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```

## Step 2: Build Example iOS App: CaffeSimple with Xcode

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to CaffeSimple app directory.

```
$ cp $CAFFE/examples/mnist/lenet.prototxt $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/
```

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, and press Command-R to build and run it on your connected device.

# For OSX Check built
==================

## Step 1: Build Caffe-Mobile Lib with cmake&make

### OSX 1: Use brew installed protobuf

```
$ brew install protobuf
$ cd build
$ cmake ..
$ make -j 4
```

### OSX 2: Use self built protobuf

```
$ cd third_party
$ ./build-protobuf-3.1.0.sh
$ cd ../build
$ cmake .. -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf
$ make -j 4
```

## Step 2: Build Caffe-bin with cmake&make

```
$ brew install gflags
$ cmake .. -DCMAKE_PREFIX_PATH=$PWD/../third_party/protobuf -DTOOLS
$ make -j 4
```
