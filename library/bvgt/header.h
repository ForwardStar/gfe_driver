//
// Created by sjf on 3/14/2022.
//

#ifndef GRAPHINDEX_HEADER_H
#define GRAPHINDEX_HEADER_H

#include <time.h>
#include <stdlib.h>
#include <malloc.h>
//#include <jemalloc/jemalloc.h>
//#include <google/tcmalloc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <omp.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <random>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/lockable_concepts.hpp>
#include <tbb/concurrent_vector.h>
//#include <tbb/compat/thread>
#include <tbb/tbb_allocator.h> // zero_allocator defined here
#include <junction/ConcurrentMap_Linear.h>
//#include <tbb/atomic.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define NUM_THREADS 56 // only effective in main-deletel and build_tree





#endif //GRAPHINDEX_HEADER_H
