Caffe on Mobile Devices
==================

iOS/Android Caffe lib with demo APP (CPU_ONLY, NO_BACKWARD, NO_BOOST, NO_HDF5, NO_LEVELDB)

# Screenshots

iPhone5s | Meizu M3 note
:---------:| :-------------:
<img src="https://raw.githubusercontent.com/solrex/caffe-mobile/master/screenshot/CaffeSimpleiPhone5s.png" width="70%" /> | <img src="https://raw.githubusercontent.com/solrex/caffe-mobile/master/screenshot/CaffeSimpleM3-Note.jpg" width="40%" />

# For iPhone

## Step 1: Build Caffe-Mobile Lib with cmake

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ ./tools/build_ios.sh
```

## Step 2: Build iOS App: CaffeSimple with Xcode

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to CaffeSimple app directory. Note: Check the batch size setting in net.prototxt, set it to `1` if needed.

```
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/weight.caffemodel
```

 - If you want to use your self-defined caffe network, use `tools/prototxt2protobin.py net.prototxt` to convert your prototxt to protobin. Then place `net.protobin` to `$CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/`.

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, connect your iPhone to Mac, change target to "Your Name's iPhone", and press Command-R to build and run it on your connected device.

# For Android

## Step 1: Build Caffe-Mobile Lib with cmake

Test passed ANDROID_ABI:

 - [x] arm64-v8a
 - [x] armeabi
 - [x] armeabi-v7a with NEON (not stable)

```
$ git clone --recursive https://github.com/solrex/caffe-mobile.git
$ export NDK_HOME=/path/to/your/ndk
$ ./tools/build_android.sh
```

## Step 2: Build Android App: CaffeSimple with Android Studio

 - For CaffeSimple to run, you need a pre-trained LeNet on MNIST caffe model and the weight file.
Follow the instructions in [Training LeNet on MNIST with Caffe](http://caffe.berkeleyvision.org/gathered/examples/mnist.html) to train your LeNet Model on MNIST. Then copy the model file `caffe/examples/mnist/lenet.prototxt` and the trained weight file `caffe/examples/mnist/lenet_iter_10000.caffemodel` to the SD card root of your Android mobile phone. Check the batch size setting in net.prototxt, set it to `1` if needed.

```
$ adb push $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     /sdcard/weight.caffemodel
$ adb push $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.protobin \
     /sdcard/net.protobin
$ adb push $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/test_image.jpg \
     /sdcard/test_image.jpg
```

 - If you want to use your self-defined caffe network, use `tools/prototxt2protobin.py net.prototxt` to convert your prototxt to protobin. Then push `net.protobin` to your sdcard root directory.

 - Load the Android studio project inside the `$CAFFE_MOBILE/examples/android/CaffeSimple/` folder, and press Command-R to build and run it on your connected device.

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
$ ./tools/prototxt2bin.py $CAFFE/examples/mnist/lenet.prototxt
$ cp $CAFFE/examples/mnist/lenet.protobin \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/net.protobin
$ cp $CAFFE/examples/mnist/lenet_iter_10000.caffemodel \
     $CAFFE_MOBILE/examples/ios/simple/CaffeSimple/data/weight.caffemodel
```

 - Check the batch size setting in net.prototxt, set it to `1` if needed.

 - Load the Xcode project inside the `$CAFFE_MOBILE/examples/ios/simple/` folder, change build target to one of your "iOS Simulators" , and press Command-R to build and run it on the simulator.

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
