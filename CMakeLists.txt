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
include(./cmake/ProtoBuf.cmake)

if(NOT MSVC)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x -Wno-sign-compare")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x -Wno-sign-compare")
endif()

if(APPLE)
    include(./cmake/Modules/FindvecLib.cmake)
    if(VECLIB_FOUND)
        include_directories(${vecLib_INCLUDE_DIR})
        list(APPEND Caffe_LINKER_LIBS ${vecLib_LINKER_LIBS})
    endif()
    if(IOS)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fembed-bitcode")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fembed-bitcode")
    else()
        if(CMAKE_PREFIX_PATH)
            set(CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH} CACHE string  "user defined search path root")
        endif()
    endif()
endif()

caffe_set_caffe_link()

add_subdirectory(src/caffe)

if(TOOLS)
    add_subdirectory(tools)
endif()

