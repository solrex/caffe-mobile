#!/bin/bash

if [ ! -d "$NDK_HOME" ]; then
    echo "$(tput setaf 2)"
    echo "###########################################################"
    echo " ERROR: Invalid NDK_HOME=\"$NDK_HOME\" env variable, exit. "
    echo "###########################################################"
    echo "$(tput sgr0)"
    exit 1
fi

ANDROID_ABIs=("armeabi" "armeabi-v7a with NEON" "arm64-v8a")

function build-abi {
    cd third_party
    ./build-protobuf-3.1.0.sh Android || exit 1
    #exit 1
    ./build-openblas.sh || exit 1
    #exit 1
    mkdir ../build_${ANDROID_ABI%% *}
    cd ../build_${ANDROID_ABI%% *} || exit 1
    rm -rf *
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../third_party/android-cmake/android.toolchain.cmake \
        -DANDROID_NDK=$NDK_HOME \
        -DANDROID_ABI="$ANDROID_ABI" \
        -DANDROID_NATIVE_API_LEVEL=$ANDROID_NATIVE_API_LEVEL \
        -G "Unix Makefiles" || exit 1
    make -j 4 || exit 1
    cd ../examples/android/CaffeSimple/app/
    mkdir -p libs/${ANDROID_ABI%% *}
    ln -sf ../../../../../../build_${ANDROID_ABI%% *}/lib/libcaffe-jni.so libs/${ANDROID_ABI%% *}/libcaffe-jni.so
    cd ../../../..
}

IFS=""
for abi in ${ANDROID_ABIs[@]}; do
    export ANDROID_ABI="$abi"
    if [ "$ANDROID_ABI" = "arm64-v8a" ]; then
        export ANDROID_NATIVE_API_LEVEL=21
    else
        export ANDROID_NATIVE_API_LEVEL=16
    fi
    echo $ANDROID_ABI
    echo $ANDROID_NATIVE_API_LEVEL
    build-abi
done

