cmake_minimum_required(VERSION 2.8.12)
project(janus)

set (CMAKE_CXX_STANDARD 14)

# run this after clone in the build_cmake directory:
# conan install --build=missing ..

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build_cmake/conanbuildinfo.cmake) #Clion,
#    with conanbuildinfo.cmake in build folder
    include(${CMAKE_CURRENT_SOURCE_DIR}/build_cmake/conanbuildinfo.cmake)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Not CLion
endif()


conan_basic_setup()

if(APPLE)
    set(RT "")
else()
    set(RT "rt")
endif()

# for c build

add_library(
        EXTERN_C
        extern_interface/scheduler.c
        extern_interface/scheduler.h
)

# for cxx build

set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCHECK_ISO")
# this makes gcc>=5 support conan.io libraries, e.g. yaml-cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")

find_package(Threads)
find_package(PkgConfig REQUIRED)
#pkg_check_modules(APR REQUIRED apr-1)
#pkg_check_modules(APRUTIL REQUIRED apr-util-1)
pkg_check_modules(PYTHON2 REQUIRED python2)
#pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
#pkg_check_modules(PROFILER REQUIRED libprofiler)

#FIND_PACKAGE(Boost COMPONENTS system filesystem coroutine REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG_CODE")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOG_LEVEL_AS_INFO")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DLOG_LEVEL_AS_DEBUG")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_PROFILE")

#FIND_LIBRARY (DMALLOC_LIBRARY dmalloc /usr/lib /sw/lib)
#FIND_PATH (DMALLOC_INCLUDE_DIR dmalloc.h PATHS /usr/include /sw/include)
#IF(DMALLOC_LIBRARY)
#  ADD_DEFINITIONS(-D_USE_DMALLOC)
#  SET(LINK_TO_DMALLOC 1)
#ENDIF(DMALLOC_LIBRARY)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/deptran/mdcc/mdcc_rpc.h
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/rpcgen --cpp deptran/mdcc/mdcc_rpc.rpc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/deptran/mdcc/mdcc_rpc.rpc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/deptran/rcc_rpc.h
           ${CMAKE_CURRENT_SOURCE_DIR}/deptran/rcc_rpc.py
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/rpcgen --cpp --python deptran/rcc_rpc.rpc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/deptran/rcc_rpc.rpc
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target (gen-rpc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/deptran/mdcc/mdcc_rpc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/deptran/rcc_rpc.h
            ${CMAKE_CURRENT_SOURCE_DIR}/deptran/rcc_rpc.py)

include_directories(
    ${PYTHON2_INCLUDE_DIRS}
#    ${APR_INCLUDE_DIRS}
#    ${YAML_CPP_INCLUDE_DIRS}
#    ${PROFILER_INCLUDE_DIRS}
    .
    rrr
    memdb
    deptran
)

link_directories(
    ${PYTHON2_LIBRARY_DIRS}
#    ${APR_LIBRARY_DIRS}
#    ${YAML_CPP_LIBRARY_DIRS}
#    ${PROFILER_LIBRARY_DIRS}
)


#set (CMAKE_SHARED_LINKER_FLAGS "-Wl, -lyaml-cpp")


file(GLOB RRR_SRC
    "rrr/base/*.hpp"
    "rrr/base/*.cpp"
    "rrr/misc/*.hpp"
    "rrr/misc/*.cpp"
    "rrr/coroutine/*.h"
    "rrr/coroutine/*.cc"
#    "rrr/mq/*.cpp"
#    "rrr/mq/*.h"
    "rrr/pylib/simplerpc/_pyrpc.cpp"
    "rrr/rpc/*.cpp"
    "rrr/rpc/*.hpp"
    "rrr/utils/*.h"
    "rrr/rrr.h"
    "rrr/rrr.hpp"
)

file(GLOB MEMDB_SRC
    "memdb/*.h"
    "memdb/*.cc")

file(GLOB BENCH_SRC
    "bench/*/*.h"
    "bench/*/*.cc"
)

file(GLOB DEPTRAN_SRC

    "deptran/*.h"
    "deptran/*.cc"
    "deptran/*/*.cc"
    "deptran/*/*.h"
)

add_library(RRR
    ${RRR_SRC})

add_library(MEMDB
    ${MEMDB_SRC})

add_library(JANUS
        ${DEPTRAN_SRC}
        ${BENCH_SRC})

#add_executable(JANUS_PROCESS
#        ${DEPTRAN_SRC}
#        ${BENCH_SRC)


add_dependencies(JANUS gen-rpc)

add_executable(JANUS_PROCESS
        "deptran/empty.cc")

target_link_libraries(
        MEMDB
        RRR
        ${CONAN_LIBS}
        ${RT})

target_link_libraries(
        JANUS
        RRR
        MEMDB
        EXTERN_C
        ${RT}
        ${CONAN_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        ${PYTHON2_LIBRARIES}
#    ${APR_LIBRARIES}
#    ${APRUTIL_LIBRARIES}
#    ${PROFILER_LIBRARIES}
)

target_link_libraries(
        JANUS_PROCESS
        JANUS
)

MESSAGE(${CONAN_LIBS})
