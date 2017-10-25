#!/bin/bash

# check host system
if [ "$(uname)" = "Darwin" ]; then
    HOST_OS=MacOSX
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    HOST_OS=Linux
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
    HOST_OS=Windows
else
    echo "Unknown OS"
    exit 1
fi

# TARGET: Windows, Linux, Android, iPhoneOS, iPhoneSimulator, MacOSX
if [ "$1" = "" ]; then
    TARGET=$HOST_OS
else
    TARGET=$1
fi

# Options for All
PB_VERSION=3.1.0
MAKE_FLAGS="$MAKE_FLAGS -j 4"
BUILD_DIR=".cbuild"

# Options for Android
if [ "$ANDROID_ABI" = "" ]; then
  # Caffe-Mobile Tested ANDROID_ABI: arm64-v8a, armeabi, armeabi-v7a with NEON
  ANDROID_ABI="arm64-v8a"
fi
#    ANDROID_ABI=armeabi-v7a - specifies the target Application Binary
#      Interface (ABI). This option nearly matches to the APP_ABI variable
#      used by ndk-build tool from Android NDK.
#
#      Possible targets are:
#        "armeabi" - ARMv5TE based CPU with software floating point operations
#        "armeabi-v7a" - ARMv7 based devices with hardware FPU instructions
#            this ABI target is used by default
#        "armeabi-v7a-hard with NEON" - ARMv7 based devices with hardware FPU instructions and hardfp
#        "armeabi-v7a with NEON" - same as armeabi-v7a, but
#            sets NEON as floating-point unit
#        "armeabi-v7a with VFPV3" - same as armeabi-v7a, but
#            sets VFPV3 as floating-point unit (has 32 registers instead of 16)
#        "armeabi-v6 with VFP" - tuned for ARMv6 processors having VFP
#        "x86" - IA-32 instruction set
#        "mips" - MIPS32 instruction set
#
#      64-bit ABIs for NDK r10 and newer:
#        "arm64-v8a" - ARMv8 AArch64 instruction set
#        "x86_64" - Intel64 instruction set (r1)
#        "mips64" - MIPS64 instruction set (r6)
if [ "$ANDROID_NATIVE_API_LEVEL" = "" ]; then
  ANDROID_NATIVE_API_LEVEL=21
fi

if [ $ANDROID_NATIVE_API_LEVEL -lt 21 -a "$ANDROID_ABI" = "arm64-v8a" ]; then
    echo "ERROR: This ANDROID_ABI($ANDROID_ABI) requires ANDROID_NATIVE_API_LEVEL($ANDROID_NATIVE_API_LEVEL) >= 21"
    exit 1
fi
BUILD_PROTOC=OFF

echo "$(tput setaf 2)"
echo Building Google Protobuf for $TARGET
echo "$(tput sgr0)"

RUN_DIR=$PWD

function fetch-protobuf {
    echo "$(tput setaf 2)"
    echo "##########################################"
    echo " Fetch Google Protobuf $PB_VERSION from source."
    echo "##########################################"
    echo "$(tput sgr0)"

    if [ ! -f protobuf-${PB_VERSION}.tar.gz ]; then
        curl -L https://github.com/google/protobuf/archive/v${PB_VERSION}.tar.gz --output protobuf-${PB_VERSION}.tar.gz
    fi
    if [ -d protobuf-${PB_VERSION} ]; then
        rm -rf protobuf-${PB_VERSION}
    fi
    tar -xzf protobuf-${PB_VERSION}.tar.gz
}

function build-Windows {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    if [ ! -s ${TARGET}-protobuf/lib/libprotobuf.a ]; then
        cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../${TARGET}-protobuf \
            -Dprotobuf_BUILD_TESTS=OFF \
            -Dprotobuf_BUILD_SHARED_LIBS=OFF \
            -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations" \
            -Dprotobuf_WITH_ZLIB=OFF \
            -G "Unix Makefiles"
        make ${MAKE_FLAGS}
        make install
    fi
    cd ../..
    rm -f protobuf
    ln -s ${TARGET}-protobuf protobuf
}

function build-Linux {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    if [ ! -s ${TARGET}-protobuf/lib/libprotobuf.a ]; then
        cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../${TARGET}-protobuf \
            -Dprotobuf_BUILD_TESTS=OFF \
            -Dprotobuf_BUILD_SHARED_LIBS=OFF \
            -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations" \
            -Dprotobuf_WITH_ZLIB=OFF
        make ${MAKE_FLAGS}
        make install
    fi
    cd ../..
    rm -f protobuf
    ln -s ${TARGET}-protobuf protobuf
}

function build-MacOSX {
    build-Linux
}

function build-Android {
    TARGET="${ANDROID_ABI%% *}-$ANDROID_NATIVE_API_LEVEL"
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
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

    if [ ! -s ${TARGET}-protobuf/lib/libprotobuf.a ]; then
        mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
        rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
        cd protobuf-$PB_VERSION/$BUILD_DIR
        cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../${TARGET}-protobuf \
            -DCMAKE_TOOLCHAIN_FILE="../../android-cmake/android.toolchain.cmake" \
            -DANDROID_NDK="$NDK_HOME" \
            -DANDROID_ABI="$ANDROID_ABI" \
            -DANDROID_NATIVE_API_LEVEL="$ANDROID_NATIVE_API_LEVEL" \
            -Dprotobuf_BUILD_TESTS=OFF \
            -Dprotobuf_BUILD_SHARED_LIBS=OFF \
            -Dprotobuf_WITH_ZLIB=OFF \
            -G "Unix Makefiles"
        make ${MAKE_FLAGS}
        make install
        cd ../..
    fi
    cd ${TARGET}-protobuf/bin
    PROTOC=protoc
    if [ $HOST_OS = "Windows" ]; then
        PROTOC=protoc.exe
    fi
    ln -sf ../../$HOST_OS-protobuf/bin/$PROTOC $PROTOC
    cd ../..
}

function build-iPhoneSimulator {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    if [ ! -s ${TARGET}-protobuf/lib/libprotobuf.a ]; then
        mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
        rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
        cd protobuf-$PB_VERSION/$BUILD_DIR
        cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../${TARGET}-protobuf\
            -DCMAKE_TOOLCHAIN_FILE="../../ios-cmake/toolchain/iOS.cmake" \
            -DIOS_PLATFORM=SIMULATOR \
            -Dprotobuf_BUILD_TESTS=OFF \
            -Dprotobuf_BUILD_SHARED_LIBS=OFF \
            -Dprotobuf_WITH_ZLIB=OFF
        make ${MAKE_FLAGS}
        make install
        cd ../..
    fi
    cd ${TARGET}-protobuf/bin
    PROTOC=protoc
    ln -sf ../../$HOST_OS-protobuf/bin/$PROTOC $PROTOC
    cd ../..
}

function build-iPhoneOS {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    if [ ! -s ${TARGET}-protobuf/lib/libprotobuf.a ]; then
        mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
        rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
        cd protobuf-$PB_VERSION/$BUILD_DIR
        cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../${TARGET}-protobuf\
            -DCMAKE_TOOLCHAIN_FILE="../../ios-cmake/toolchain/iOS.cmake" \
            -DIOS_PLATFORM=OS \
            -DCMAKE_CXX_FLAGS="-fembed-bitcode -Wno-deprecated-declarations" \
            -Dprotobuf_BUILD_TESTS=OFF \
            -Dprotobuf_BUILD_SHARED_LIBS=OFF \
            -Dprotobuf_WITH_ZLIB=OFF
        make ${MAKE_FLAGS}
        make install
        cd ../..
    fi
    cd ${TARGET}-protobuf/bin
    PROTOC=protoc
    ln -sf ../../$HOST_OS-protobuf/bin/$PROTOC $PROTOC
    cd ../..
}

fetch-protobuf
if [ "$TARGET" != "Linux" -a "$TARGET" != "MacOSX" -a "$TARGET" != "Windows" ]; then
    PROTOC_VERSION=$(./$HOST_OS-protobuf/bin/protoc --version)
    if [ "$PROTOC_VERSION" != "libprotoc 3.1.0" ]; then
        TARGET_SAVE=$TARGET
        TARGET=$HOST_OS
        build-$TARGET
        TARGET=$TARGET_SAVE
    fi
fi
build-$TARGET
