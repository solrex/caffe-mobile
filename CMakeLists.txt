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

add_definitions(-DDISABLE_BOOST)
add_definitions(-DDISABLE_GLOG)
add_definitions(-DDISABLE_HDF5)
add_definitions(-DFORWARD_ONLY)
add_definitions(-DCAFFE_COMPACT)

include(./cmake/Utils.cmake)
include(./cmake/Misc.cmake)
include(./cmake/Targets.cmake)
include(./cmake/ProtoBuf.cmake)

if(APPLE)
    include(./cmake/Modules/FindvecLib.cmake)
    if(VECLIB_FOUND)
        include_directories(${vecLib_INCLUDE_DIR})
        list(APPEND Caffe_LINKER_LIBS ${vecLib_LINKER_LIBS})
    endif()
endif()

if(NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x -Wno-sign-compare")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x -Wno-sign-compare")
endif()

caffe_set_caffe_link()

add_subdirectory(src/caffe)

if(NOT UNIX)
    add_subdirectory(tools)
endif()

