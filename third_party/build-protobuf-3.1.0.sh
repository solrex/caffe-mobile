#!/bin/bash

# TARGET: Linux, Android, iPhoneOS, iPhoneSimulator, MacOSX
if [ "$1" = "" ]; then
    TARGET=Linux
else
    TARGET=$1
fi

# Options for All
PB_VERSION=3.1.0
MAKE_FLAGS="$MAKE_FLAGS -j 4"
BUILD_DIR=".cbuild"

# Options for Android
ANDROID_ABI="arm64-v8a"
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
ANDROID_NATIVE_API_LEVEL=21
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

function build-Linux {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../protobuf-$TARGET \
        -Dprotobuf_BUILD_TESTS=OFF \
        -Dprotobuf_BUILD_SHARED_LIBS=OFF \
        -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations" \
        -Dprotobuf_WITH_ZLIB=OFF
    make ${MAKE_FLAGS}
    make install
    mkdir -p ../../protobuf
    cd ../../protobuf
    rm -f lib include bin
    ln -s ../protobuf-$TARGET/lib lib
    ln -s ../protobuf-$TARGET/include include
    ln -s ../protobuf-$TARGET/bin bin
    cd ..
}

function build-MacOSX {
    build-Linux
}

function build-Android {
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

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    # if [ "$BUILD_PROTOC" = "OFF" ]; then
    #     # Do not cross build protoc
    #     sed -i "s/include(libprotoc.cmake)/#include(libprotoc.cmake)/" ../cmake/CMakeLists.txt
    #     sed -i "s/include(protoc.cmake)/#include(protoc.cmake)/" ../cmake/CMakeLists.txt
    # fi
    cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../protobuf-$TARGET\
        -DCMAKE_TOOLCHAIN_FILE="../../android-cmake/android.toolchain.cmake" \
        -DANDROID_NDK="$NDK_HOME" \
        -DANDROID_ABI="$ANDROID_ABI" \
        -DANDROID_NATIVE_API_LEVEL="$ANDROID_NATIVE_API_LEVEL" \
        -Dprotobuf_BUILD_TESTS=OFF \
        -Dprotobuf_BUILD_SHARED_LIBS=OFF \
        -Dprotobuf_WITH_ZLIB=OFF
    make ${MAKE_FLAGS}
    make install
    mkdir -p ../../protobuf
    cd ../../protobuf
    rm -f lib include
    ln -s ../protobuf-$TARGET/lib lib
    ln -s ../protobuf-$TARGET/include include
    cd ..
}

function build-iPhoneSimulator {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    # if [ "$BUILD_PROTOC" = "OFF" ]; then
    #     # Do not cross build protoc
    #     sed -i "s/include(libprotoc.cmake)/#include(libprotoc.cmake)/" ../cmake/CMakeLists.txt
    #     sed -i "s/include(protoc.cmake)/#include(protoc.cmake)/" ../cmake/CMakeLists.txt
    # fi
    cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../protobuf-$TARGET\
        -DCMAKE_TOOLCHAIN_FILE="../../ios-cmake/toolchain/iOS.cmake" \
        -DIOS_PLATFORM=SIMULATOR \
        -Dprotobuf_BUILD_TESTS=OFF \
        -Dprotobuf_BUILD_SHARED_LIBS=OFF \
        -Dprotobuf_WITH_ZLIB=OFF
    make ${MAKE_FLAGS}
    make install
    mkdir -p ../../protobuf
    cd ../../protobuf
    rm -f lib include
    ln -s ../protobuf-$TARGET/lib lib
    ln -s ../protobuf-$TARGET/include include
    cd ..
}

function build-iPhoneOS {
    echo "$(tput setaf 2)"
    echo "#####################"
    echo " Building protobuf for $TARGET"
    echo "#####################"
    echo "$(tput sgr0)"

    mkdir -p protobuf-$PB_VERSION/$BUILD_DIR
    rm -rf protobuf-$PB_VERSION/$BUILD_DIR/*
    cd protobuf-$PB_VERSION/$BUILD_DIR
    # if [ "$BUILD_PROTOC" = "OFF" ]; then
    #     # Do not cross build protoc
    #     sed -i "s/include(libprotoc.cmake)/#include(libprotoc.cmake)/" ../cmake/CMakeLists.txt
    #     sed -i "s/include(protoc.cmake)/#include(protoc.cmake)/" ../cmake/CMakeLists.txt
    # fi
    cmake ../cmake -DCMAKE_INSTALL_PREFIX=../../protobuf-$TARGET\
        -DCMAKE_TOOLCHAIN_FILE="../../ios-cmake/toolchain/iOS.cmake" \
        -DIOS_PLATFORM=OS \
        -DCMAKE_CXX_FLAGS="-fembed-bitcode -Wno-deprecated-declarations" \
        -Dprotobuf_BUILD_TESTS=OFF \
        -Dprotobuf_BUILD_SHARED_LIBS=OFF \
        -Dprotobuf_WITH_ZLIB=OFF
    make ${MAKE_FLAGS}
    make install
    mkdir -p ../../protobuf
    cd ../../protobuf
    rm -f lib include
    ln -s ../protobuf-$TARGET/lib lib
    ln -s ../protobuf-$TARGET/include include
    cd ..
}

fetch-protobuf
if [ "$TARGET" != "Linux" -a "$TARGET" != "MacOSX" ]; then
    PROTOC_VERSION=$(./protobuf/bin/protoc --version)
    if [ "$PROTOC_VERSION" != "libprotoc 3.1.0" ]; then
        TARGET_SAVE=$TARGET
        TARGET=Linux
        build-$TARGET
        TARGET=$TARGET_SAVE
    fi
fi
build-$TARGET
