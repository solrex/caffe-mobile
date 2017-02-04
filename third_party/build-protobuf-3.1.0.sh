#!/bin/bash -x
# https://gist.github.com/e814d23046772a98ae437270e8aaaf08.git

echo "$(tput setaf 2)"
echo Building Google Protobuf for Mac OS X / iOS.
echo Use 'tail -f build.log' to monitor progress.
echo "$(tput sgr0)"

# Controls which architectures are build/included in the
# universal binaries and libraries this script produces.
# Set each to '1' to include, '0' to exclude.
BUILD_X86_64_MAC=1
BUILD_X86_64_IOS_SIM=0
BUILD_I386_IOS_SIM=1
BUILD_ARMV7_IPHONE=1
BUILD_ARMV7S_IPHONE=1
BUILD_ARM64_IPHONE=1

PB_VERSION=3.1.0

# Set this to the replacement name for the 'google' namespace.
# This is being done to avoid a conflict with the private
# framework build of Google Protobuf that Apple ships with their
# OpenGL ES framework.
GOOGLE_NAMESPACE=google

# Set this to the minimum iOS SDK version you wish to support.
IOS_MIN_SDK=7.0

(

PREFIX=`pwd`/protobuf
mkdir -p ${PREFIX}/platform

EXTRA_MAKE_FLAGS="-j4"

XCODEDIR=`xcode-select --print-path`

OSX_SDK=$(xcodebuild -showsdks | grep macosx | sort | head -n 1 | awk '{print $NF}')
IOS_SDK=$(xcodebuild -showsdks | grep iphoneos | sort | head -n 1 | awk '{print $NF}')
SIM_SDK=$(xcodebuild -showsdks | grep iphonesimulator | sort | head -n 1 | awk '{print $NF}')


MACOSX_PLATFORM=${XCODEDIR}/Platforms/MacOSX.platform
MACOSX_SYSROOT=${MACOSX_PLATFORM}/Developer/${OSX_SDK}.sdk

IPHONEOS_PLATFORM=${XCODEDIR}/Platforms/iPhoneOS.platform
IPHONEOS_SYSROOT=${IPHONEOS_PLATFORM}/Developer/SDKs/${IOS_SDK}.sdk

IPHONESIMULATOR_PLATFORM=${XCODEDIR}/Platforms/iPhoneSimulator.platform
IPHONESIMULATOR_SYSROOT=${IPHONESIMULATOR_PLATFORM}/Developer/SDKs/${SIM_SDK}.sdk

CC=clang
CFLAGS="-DNDEBUG -g -O0 -pipe -fPIC -fcxx-exceptions"
CXX=clang
CXXFLAGS="${CFLAGS} -std=c++11 -stdlib=libc++"
LDFLAGS="-stdlib=libc++"
LIBS="-lc++ -lc++abi"

echo "$(tput setaf 2)"
echo "####################################"
echo " Cleanup any earlier build attempts"
echo "####################################"
echo "$(tput sgr0)"

(
    cd /tmp
    if [ -d ${PREFIX} ]
    then
        rm -rf ${PREFIX}
    fi
    mkdir ${PREFIX}
    mkdir ${PREFIX}/platform
)

echo "$(tput setaf 2)"
echo "##########################################"
echo " Fetch Google Protobuf $PB_VERSION from source."
echo "##########################################"
echo "$(tput sgr0)"

(
    cd /tmp

    curl -L https://github.com/google/protobuf/archive/v${PB_VERSION}.tar.gz --output /tmp/protobuf-${PB_VERSION}.tar.gz

    if [ -d /tmp/protobuf-${PB_VERSION} ]
    then
        rm -rf /tmp/protobuf-${PB_VERSION}
    fi
    
    tar xvf /tmp/protobuf-${PB_VERSION}.tar.gz
)

echo "$(tput setaf 2)"
echo "###############################################################"
echo " Replace 'namespace google' with 'namespace google_public'"
echo " in all source/header files.  This is to address a"
echo " namespace collision issue when building for recent"
echo " versions of iOS.  Apple is using the protobuf library"
echo " internally, and embeds it as a private framework."
echo "###############################################################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION/src/google/protobuf
    sed -i '' -e "s/namespace\ google /namespace\ ${GOOGLE_NAMESPACE} /g" $(find . -name \*.h -type f)
    sed -i '' -e "s/namespace\ google /namespace\ ${GOOGLE_NAMESPACE} /g" $(find . -name \*.cc -type f)
    sed -i '' -e "s/namespace\ google /namespace\ ${GOOGLE_NAMESPACE} /g" $(find . -name \*.proto -type f)
    sed -i '' -e "s/google::protobuf/${GOOGLE_NAMESPACE}::protobuf/g" $(find . -name \*.h -type f)
    sed -i '' -e "s/google::protobuf/${GOOGLE_NAMESPACE}::protobuf/g" $(find . -name \*.cc -type f)
    sed -i '' -e "s/google::protobuf/${GOOGLE_NAMESPACE}::protobuf/g" $(find . -name \*.proto -type f)
)

if [ $BUILD_X86_64_MAC -eq 1 ]
then

echo "$(tput setaf 2)"
echo "#####################"
echo " x86_64 for Mac OS X"
echo " and python bindings"
echo "#####################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/x86_64 "CC=${CC}" "CFLAGS=${CFLAGS} -arch x86_64" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch x86_64" "LDFLAGS=${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS}
    make ${EXTRA_MAKE_FLAGS} test
    make ${EXTRA_MAKE_FLAGS} install
    cd python
    python setup.py build
    python setup.py install --user
)
X86_64_MAC_PROTOBUF=x86_64/lib/libprotobuf.a
X86_64_MAC_PROTOBUF_LITE=x86_64/lib/libprotobuf-lite.a

else

X86_64_MAC_PROTOBUF=
X86_64_MAC_PROTOBUF_LITE=

fi

if [ $BUILD_X86_64_IOS_SIM -eq 1 ]
then

echo "$(tput setaf 2)"
echo "###########################"
echo " x86_64 for iPhone Simulator"
echo "###########################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --build=x86_64-apple-darwin13.0.0 --host=x86_64-apple-darwin13.0.0 --with-protoc=${PREFIX}/platform/x86_64/bin/protoc --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/x86_64_ios "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${IOS_MIN_SDK} -arch x86_64 -isysroot ${IPHONESIMULATOR_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch x86_64 -isysroot ${IPHONESIMULATOR_SYSROOT} -miphoneos-version-min=${IOS_MIN_SDK}" LDFLAGS="-arch x86_64 -miphoneos-version-min=${IOS_MIN_SDK} ${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS}
    make ${EXTRA_MAKE_FLAGS} install
)

X86_64_IOS_SIM_PROTOBUF=x86_64_ios/lib/libprotobuf.a
X86_64_IOS_SIM_PROTOBUF_LITE=x86_64_ios/lib/libprotobuf-lite.a

else

X86_64_IOS_SIM_PROTOBUF=
X86_64_IOS_SIM_PROTOBUF_LITE=

fi

if [ $BUILD_I386_IOS_SIM -eq 1 ]
then

echo "$(tput setaf 2)"
echo "###########################"
echo " i386 for iPhone Simulator"
echo "###########################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --build=x86_64-apple-darwin13.0.0 --host=i386-apple-darwin13.0.0 --with-protoc=${PREFIX}/platform/x86_64/bin/protoc --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/i386 "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${IOS_MIN_SDK} -arch i386 -isysroot ${IPHONESIMULATOR_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch i386 -isysroot ${IPHONESIMULATOR_SYSROOT} -miphoneos-version-min=${IOS_MIN_SDK}" LDFLAGS="-arch i386 -miphoneos-version-min=${IOS_MIN_SDK} ${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS}
    make ${EXTRA_MAKE_FLAGS} install
)

I386_IOS_SIM_PROTOBUF=i386/lib/libprotobuf.a
I386_IOS_SIM_PROTOBUF_LITE=i386/lib/libprotobuf-lite.a

else

I386_IOS_SIM_PROTOBUF=
I386_IOS_SIM_PROTOBUF_LITE=

fi

if [ $BUILD_ARMV7_IPHONE -eq 1 ]
then

echo "$(tput setaf 2)"
echo "##################"
echo " armv7 for iPhone"
echo "##################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --build=x86_64-apple-darwin13.0.0 --host=armv7-apple-darwin13.0.0 --with-protoc=${PREFIX}/platform/x86_64/bin/protoc --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/armv7 "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${IOS_MIN_SDK} -arch armv7 -isysroot ${IPHONEOS_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch armv7 -isysroot ${IPHONEOS_SYSROOT}" LDFLAGS="-arch armv7 -miphoneos-version-min=${IOS_MIN_SDK} ${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS}
    make ${EXTRA_MAKE_FLAGS} install
)

ARMV7_IPHONE_PROTOBUF=armv7/lib/libprotobuf.a 
ARMV7_IPHONE_PROTOBUF_LITE=armv7/lib/libprotobuf-lite.a 

else

ARMV7_IPHONE_PROTOBUF=
ARMV7_IPHONE_PROTOBUF_LITE=

fi

if [ $BUILD_ARMV7S_IPHONE -eq 1 ]
then

echo "$(tput setaf 2)"
echo "###################"
echo " armv7s for iPhone"
echo "###################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --build=x86_64-apple-darwin13.0.0 --host=armv7s-apple-darwin13.0.0 --with-protoc=${PREFIX}/platform/x86_64/bin/protoc --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/armv7s "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${IOS_MIN_SDK} -arch armv7s -isysroot ${IPHONEOS_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch armv7s -isysroot ${IPHONEOS_SYSROOT}" LDFLAGS="-arch armv7s -miphoneos-version-min=${IOS_MIN_SDK} ${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS} 
    make ${EXTRA_MAKE_FLAGS} install

)

ARMV7S_IPHONE_PROTOBUF=armv7s/lib/libprotobuf.a 
ARMV7S_IPHONE_PROTOBUF_LITE=armv7s/lib/libprotobuf-lite.a 

else

ARMV7S_IPHONE_PROTOBUF=
ARMV7S_IPHONE_PROTOBUF_LITE=

fi

if [ $BUILD_ARM64_IPHONE -eq 1 ]
then

echo "$(tput setaf 2)"
echo "########################################"
echo " Patch Protobuf $PB_VERSION for 64bit support"
echo "########################################"
echo "$(tput sgr0)"

#(
    #cd /tmp/protobuf-$PB_VERSION
    # make ${EXTRA_MAKE_FLAGS} distclean
    # curl https://gist.github.com/BennettSmith/7111094/raw/171695f70b102de2301f5b45d9e9ab3167b4a0e8/0001-Add-generic-GCC-support-for-atomic-operations.patch --location --output /tmp/0001-Add-generic-GCC-support-for-atomic-operations.patch
    # curl https://gist.github.com/BennettSmith/7111094/raw/a4e85ffc82af00ae7984020300db51a62110db48/0001-Add-generic-gcc-header-to-Makefile.am.patch --location --output /tmp/0001-Add-generic-gcc-header-to-Makefile.am.patch
    # patch -p1 < /tmp/0001-Add-generic-GCC-support-for-atomic-operations.patch
    # patch -p1 < /tmp/0001-Add-generic-gcc-header-to-Makefile.am.patch
    # rm /tmp/0001-Add-generic-GCC-support-for-atomic-operations.patch
    # rm /tmp/0001-Add-generic-gcc-header-to-Makefile.am.patch
#)

echo "$(tput setaf 2)"
echo "##################"
echo " arm64 for iPhone"
echo "##################"
echo "$(tput sgr0)"

(
    cd /tmp/protobuf-$PB_VERSION
    ./autogen.sh
    make distclean
    ./configure --build=x86_64-apple-darwin13.0.0 --host=arm --with-protoc=${PREFIX}/platform/x86_64/bin/protoc --disable-shared --prefix=${PREFIX} --exec-prefix=${PREFIX}/platform/arm64 "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${IOS_MIN_SDK} -arch arm64 -isysroot ${IPHONEOS_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch arm64 -isysroot ${IPHONEOS_SYSROOT}" LDFLAGS="-arch arm64 -miphoneos-version-min=${IOS_MIN_SDK} ${LDFLAGS}" "LIBS=${LIBS}"
    make ${EXTRA_MAKE_FLAGS}
    make ${EXTRA_MAKE_FLAGS} install
)

ARM64_IPHONE_PROTOBUF=arm64/lib/libprotobuf.a 
ARM64_IPHONE_PROTOBUF_LITE=arm64/lib/libprotobuf-lite.a 

else

ARM64_IPHONE_PROTOBUF=
ARM64_IPHONE_PROTOBUF_LITE=

fi

echo "$(tput setaf 2)"
echo "############################"
echo " Create Universal Libraries"
echo "############################"
echo "$(tput sgr0)"

(
    cd ${PREFIX}/platform
    mkdir universal

    lipo ${ARM64_IPHONE_PROTOBUF} ${ARMV7S_IPHONE_PROTOBUF} ${ARMV7_IPHONE_PROTOBUF} ${I386_IOS_SIM_PROTOBUF} ${X86_64_MAC_PROTOBUF} -create -output universal/libprotobuf.a

    lipo ${ARM64_IPHONE_PROTOBUF_LITE} ${ARMV7S_IPHONE_PROTOBUF_LITE} ${ARMV7_IPHONE_PROTOBUF_LITE} ${I386_IOS_SIM_PROTOBUF_LITE} ${X86_64_MAC_PROTOBUF_LITE} -create -output universal/libprotobuf-lite.a
)

echo "$(tput setaf 2)"
echo "########################"
echo " Finalize the packaging"
echo "########################"
echo "$(tput sgr0)"

(
    cd ${PREFIX}
    mkdir bin
    mkdir lib
    cp -r platform/x86_64/bin/protoc bin
#    cp -r platform/x86_64/lib/* lib
    cp -r platform/universal/* lib
#    rm -rf platform

    file lib/libprotobuf.a
    file lib/libprotobuf-lite.a
    file lib/libprotoc.a
)

) 2>&1
#) >build.log 2>&1

echo "$(tput setaf 2)"
echo Done!
echo "$(tput sgr0)"
