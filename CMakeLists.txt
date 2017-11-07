cmake_minimum_required(VERSION 2.8.7)
if(POLICY CMP0046)
    cmake_policy(SET CMP0046 NEW)
endif()
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif()

set(CAFFE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CAFFE_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_definitions(-DCPU_ONLY)
add_definitions(-DUSE_NEON_MATH)

include(./cmake/Utils.cmake)
include(./cmake/Misc.cmake)
include(./cmake/Targets.cmake)

# Set CXX_FLAGS for different platform
if(NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-sign-compare")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-sign-compare")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x")
    if(IOS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fembed-bitcode")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fembed-bitcode")
    endif()
endif()

# Set search path for cross compile
if(ANDROID)
    STRING(REGEX MATCH "[^ ]+" ANDROID_ABI_PREFIX ${ANDROID_ABI})
    set(THIRD_PREFIX ${ANDROID_ABI_PREFIX}-${ANDROID_NATIVE_API_LEVEL}-)
    list(INSERT CMAKE_FIND_ROOT_PATH 0 ${CMAKE_SOURCE_DIR}/third_party/${THIRD_PREFIX}protobuf ${CMAKE_SOURCE_DIR}/third_party/${THIRD_PREFIX}OpenBLAS)
elseif(IOS)
    list(INSERT CMAKE_FIND_ROOT_PATH 0 ${CMAKE_SOURCE_DIR}/third_party/ios-protobuf)
endif()
#message(STATUS "Change CMAKE_FIND_ROOT_PATH to: ${CMAKE_FIND_ROOT_PATH}")

include(./cmake/ProtoBuf.cmake)
include(./cmake/Modules/FindOpenBLAS.cmake)
include(./cmake/Modules/FindvecLib.cmake)
include(./cmake/Modules/FindAtlas.cmake)
# Select blas lib for different platform
# APPLE only, 1st
if(VECLIB_FOUND)
    include_directories(${vecLib_INCLUDE_DIR})
    list(APPEND Caffe_LINKER_LIBS ${vecLib_LINKER_LIBS})
elseif(OpenBLAS_FOUND)
    include_directories(${OpenBLAS_INCLUDE_DIR})
    list(APPEND Caffe_LINKER_LIBS ${OpenBLAS_LIB})
elseif(ATLAS_FOUND)
    include_directories(${Atlas_CBLAS_INCLUDE_DIR})
    list(APPEND Caffe_LINKER_LIBS ${Atlas_CBLAS_LIBRARY})
else()
    MESSAGE(FATAL_ERROR "BLAS (VecLib/OpenBLAS/Atlas) library not found.")
endif()

caffe_set_caffe_link()

add_subdirectory(src/caffe)

if(ANDROID)
    add_subdirectory(src/jni)
endif()

if(TOOLS)
    add_subdirectory(tools)
endif()

