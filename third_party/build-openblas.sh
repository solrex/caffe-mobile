#!/bin/bash

PLATFORM=Android

# Options for All
OPENBLAS_VERSION=0.2.19
MAKE_FLAGS="$MAKE_FLAGS -j 4"
BUILD_DIR=".cbuild"

# Options for Android
ANDROID_NATIVE_API_LEVEL=21
# Options: "arm64-v8a" "armeabi-v7a with NEON"
ANDROID_ABI="armeabi-v7a with NEON"

# Build Environment
if [ "$(uname)" = "Darwin" ]; then
    OS=darwin
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    OS=linux
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ||
       "$(expr substr $(uname -s) 1 9)" = "CYGWIN_NT" ]; then
    OS=windows
else
    echo "Unknown OS"
    exit 1
fi

if [ "$(uname -m)" = "x86_64"  ]; then
    BIT=x86_64
else
    BIT=x86
fi

echo "$(tput setaf 2)"
echo Building Openblas for $PLATFORM
echo "$(tput sgr0)"

RUN_DIR=$PWD

function fetch-OpenBLAS {
    echo "$(tput setaf 2)"
    echo "##########################################"
    echo " Fetch Openblas $OPENBLAS_VERSION from source."
    echo "##########################################"
    echo "$(tput sgr0)"

    if [ ! -f OpenBLAS-${OPENBLAS_VERSION}.tar.gz ]; then
        curl -L https://github.com/xianyi/OpenBLAS/archive/v${OPENBLAS_VERSION}.tar.gz --output OpenBLAS-${OPENBLAS_VERSION}.tar.gz
    fi
    if [ -d OpenBLAS-${OPENBLAS_VERSION} ]; then
        rm -rf OpenBLAS-${OPENBLAS_VERSION}
    fi
    tar -xzf OpenBLAS-${OPENBLAS_VERSION}.tar.gz
}

function build-Android {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building OpenBLAS for $PLATFORM"
    echo "#####################"
    echo "$(tput sgr0)"

    # Test ENV NDK_HOME
    if [ ! -d "$NDK_HOME" ]; then
        echo "$(tput setaf 2)"
        echo "###########################################################"
        echo " ERROR: Invalid NDK_HOME=\"$NDK_HOME\" env variable, exit. "
        echo "###########################################################"
        echo "$(tput sgr0)"
        exit 1
    fi

    if [ "${ANDROID_ABI}" = "armeabi-v7a with NEON" ]; then
        CROSS_SUFFIX=$NDK_HOME/toolchains/arm-linux-androideabi-4.9/prebuilt/${OS}-${BIT}/bin/arm-linux-androideabi-
        SYSROOT=$NDK_HOME/platforms/android-$ANDROID_NATIVE_API_LEVEL/arch-arm
        TARGET=ARMV7
        BINARY=32
    elif [ "${ANDROID_ABI}" = "arm64-v8a" ]; then
        CROSS_SUFFIX=$NDK_HOME/toolchains/aarch64-linux-android-4.9/prebuilt/${OS}-${BIT}/bin/aarch64-linux-android-
        SYSROOT=$NDK_HOME/platforms/android-$ANDROID_NATIVE_API_LEVEL/arch-arm64
        TARGET=ARMV8
        BINARY=64
    else
        echo "Error: not support OpenBLAS for ABI: ${ANDROID_ABI}"
        exit 1
    fi

    mkdir -p OpenBLAS-$TARGET
    cd OpenBLAS-$OPENBLAS_VERSION
    make ${MAKE_FLAGS} \
        NOFORTRAN=1 \
        NO_NOLAPACKE=1 \
        OSNAME=Android \
        SMP=1 \
        USE_THREAD=1 \
        NUM_THREAD=4 \
        CROSS_SUFFIX="$CROSS_SUFFIX" \
        CC="${CROSS_SUFFIX}gcc --sysroot=$SYSROOT" \
        HOSTCC=gcc \
        TARGET=$TARGET \
        CFLAGS="-mhard-float" \
        BINARY=$BINARY \
        PREFIX="../OpenBLAS-$TARGET"
    make ${MAKE_FLAGS} \
        NOFORTRAN=1 \
        NO_NOLAPACKE=1 \
        OSNAME=Android \
        SMP=1 \
        USE_THREAD=1 \
        NUM_THREAD=4 \
        CROSS_SUFFIX="$CROSS_SUFFIX" \
        CC="${CROSS_SUFFIX}gcc --sysroot=$SYSROOT" \
        HOSTCC=gcc \
        TARGET=$TARGET \
        CFLAGS="-mhard-float" \
        BINARY=$BINARY \
        PREFIX="../OpenBLAS-$TARGET" \
        install
    cd ..
    ln -s OpenBLAS-$OPENBLAS_VERSION OpenBLAS
}

fetch-OpenBLAS
build-$PLATFORM
