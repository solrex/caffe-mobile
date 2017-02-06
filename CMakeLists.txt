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

#add_definitions(-DUSE_EIGEN)
add_definitions(-DCPU_ONLY)

include(./cmake/Utils.cmake)
include(./cmake/Misc.cmake)
include(./cmake/Targets.cmake)

# Set CXX_FLAGS for different platform
if(NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x -Wno-sign-compare")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x -Wno-sign-compare")
    if(IOS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fembed-bitcode")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fembed-bitcode")
    endif()
endif()

# Set search path for cross compile
if(THIRD_PARTY)
    set(SAVE_PATH ${CMAKE_FIND_ROOT_PATH})
    set(CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/third_party/protobuf ${CMAKE_SOURCE_DIR}/third_party/OpenBLAS)
    include(./cmake/ProtoBuf.cmake)
    include(./cmake/Modules/FindOpenBLAS.cmake)
    set(CMAKE_FIND_ROOT_PATH ${SAVE_PATH})
else()
    include(./cmake/ProtoBuf.cmake)
    include(./cmake/Modules/FindOpenBLAS.cmake)
endif()
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

if(TOOLS)
    add_subdirectory(tools)
endif()

