cmake_minimum_required(VERSION 3.16)
project(GraphIndex)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(CMAKE_CXX_STANDARD 20)

#file(GLOB GAPBS "third_party/gapbs/src/*")
add_library(BVGT STATIC GAPBS/benchmark.cpp
            GAPBS/bfs.cc
            GAPBS/bitmap.h
            GAPBS/cc_sv.cc
            GAPBS/platform_atomics.cpp
            GAPBS/pr_spmv.cc
            GAPBS/pvector.h
            GAPBS/sliding_queue.h
            GAPBS/sssp.cc
            GAPBS/timer.h
            Spruce/src/index_algorithms.cpp 
            Spruce/src/memory_analysis.cpp
            Spruce/src/succinct_algorithms.cpp
            Spruce/src/spruce_transaction.h
            Spruce/src/spruce_transaction.cpp)