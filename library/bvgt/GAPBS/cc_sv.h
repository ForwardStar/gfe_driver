//
// Created by sjf on 9/7/2022.
//
// Weakly connected components

#ifndef GRAPHINDEX_CC_SV_H
#define GRAPHINDEX_CC_SV_H

#include "benchmark.h"
#include "bitmap.h"
#include "pvector.h"
#include "timer.h"
#include "../Spruce/src/index_algorithms.h"
#include "../Spruce/src/spruce_transaction.h"

pvector<NodeID> ShiloachVishkin(SpruceTransVer& g, uint32_t num_nodes);

#endif //GRAPHINDEX_CC_SV_H
