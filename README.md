Caffe on Mobile Devices
==================

iOS/Android Caffe lib with demo APP (CPU_ONLY, NO_BACKWARD, NO_BOOST, NO_HDF5, NO_LEVELDB)

# Screenshots

## iPhone5s

![image](https://raw.githubusercontent.com/solrex/caffe-mobile/master/screenshot/CaffeSimpleiPhone5s.png)

## Meizu M3 note

![image](https://raw.githubusercontent.com/solrex/caffe-mobile/master/screenshot/CaffeSimpleM3-Note.jpg)

# For iPhone Simulator

## Step 1: Build Caffe-Mobile Lib with cmake

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ cd caffe-mobile/third_party
$ ./build-protobuf-3.1.0.sh iPhoneSimulator
$ mkdir ../build
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=SIMULATOR -DTHIRD_PARTY=1
$ make -j 4
```

## Step 2: Build iOS App: CaffeSimple with Xcode

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to CaffeSimple app directory.

```
$ cp $CAFFE/examples/mnist/lenet.prototxt \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.prototxt
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/weight.caffemodel
```

 - Check the batch size setting in net.prototxt, set it to `1` if needed.

```
$ diff $CAFFE/examples/mnist/lenet.prototxt \
       $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.prototxt
6c6
<   input_param { shape: { dim: 64 dim: 1 dim: 28 dim: 28 } }
---
>   input_param { shape: { dim: 1 dim: 1 dim: 28 dim: 28 } }
```

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, and press Command-R to build and run it on the simulator.

# For iPhone

## Step 1: Build Caffe-Mobile Lib with cmake

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ cd caffe-mobile/third_party
$ ./build-protobuf-3.1.0.sh iPhoneOS
$ mkdir ../build
$ cd ../build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/ios-cmake/toolchain/iOS.cmake \
  -DIOS_PLATFORM=OS -DTHIRD_PARTY=1
$ make -j 4
```

## Step 2: Build iOS App: CaffeSimple with Xcode

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to CaffeSimple app directory.

```
$ cp $CAFFE/examples/mnist/lenet.prototxt \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.prototxt
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/weight.caffemodel
```

 - Check the batch size setting in net.prototxt, set it to `1` if needed.

```
$ diff $CAFFE/examples/mnist/lenet.prototxt \
       $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.prototxt
6c6
<   input_param { shape: { dim: 64 dim: 1 dim: 28 dim: 28 } }
---
>   input_param { shape: { dim: 1 dim: 1 dim: 28 dim: 28 } }
```

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, and press Command-R to build and run it on your connected device.

# For Android (arm64-v8a only)

## Step 1: Build Caffe-Mobile Lib with cmake

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ cd caffe-mobile/third_party
$ ./build-protobuf-3.1.0.sh Android
$ ./build-openblas.sh
$ mkdir ../build
$ cd ../build
$ export NDK_HOME=/path/to/your/ndk # TODO
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/android-cmake/android.toolchain.cmake \
-DANDROID_NDK=$NDK_HOME \
-DANDROID_ABI="arm64-v8a" \
-DANDROID_NATIVE_API_LEVEL=21 \
-DTHIRD_PARTY=1
$ make -j 4
```

## Step 2: Copy Caffe-Mobile Lib to JniLib of CaffeSimple

```
$ mkdir -p ../examples/android/CaffeSimple/app/libs/arm64-v8a/
$ cp ../build/lib/libcaffe-jni.so ../examples/android/CaffeSimple/app/libs/arm64-v8a/
```

## Step 3: Build Android App: CaffeSimple with Android Studio

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to the SD card root of your Android mobile phone.

```
$ adb push $CAFFE/examples/mnist/lenet.prototxt \
     /sdcard/net.prototxt
$ adb push $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     /sdcard/weight.caffemodel
$ adb push $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/test_image.png \
     /sdcard/test_image.png
```

 - Check the batch size setting in net.prototxt, set it to `1` if needed.

```
$ diff $CAFFE/examples/mnist/lenet.prototxt \
       net.prototxt
6c6
<   input_param { shape: { dim: 64 dim: 1 dim: 28 dim: 28 } }
---
>   input_param { shape: { dim: 1 dim: 1 dim: 28 dim: 28 } }
```

 - Load the Android studio project inside the `$CAFFE_MOBILE/examples/android/CaffeSimple/` folder, and press Command-R to build and run it on your connected device.

# For MacOSX & Ubuntu

## Step 1: Install dependency

```
$ brew install protobuf # MacOSX
$ sudo apt install libprotobuf-dev protobuf-compiler libatlas-dev # Ubuntu
```

## Step 2: Build Caffe-Mobile Lib with cmake

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ mkdir build
$ cd ../build
$ cmake ..
$ make -j 4
```

## Step 3: Build Caffe-bin with cmake

```
$ brew install gflags
$ cmake .. -DTOOLS
$ make -j 4
```

# Thanks

 - Based on https://github.com/BVLC/caffe
 - Inspired by https://github.com/chyh1990/caffe-compact
 - Use https://github.com/Yangqing/ios-cmake
 - Use https://github.com/taka-no-me/android-cmake
