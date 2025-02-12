//
// Created by sjf on 9/7/2022.
//

#ifndef GRAPHINDEX_SSSP_H
#define GRAPHINDEX_SSSP_H

#include "benchmark.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "timer.h"
#include "../Spruce/src/index_algorithms.h"
#include "../Spruce/src/spruce_transaction.h"

inline
void RelaxEdges(SpruceTransVer& g, NodeID u, WeightT delta,
                pvector<WeightT> &dist, std::vector <std::vector<NodeID>> &local_bins) {
    std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
    SpruceTransVer::get_neighbours(g, u, neighbours);
    for (auto v : neighbours) {
        WeightT old_dist = dist[v.des];
        WeightT new_dist = dist[u] + v.weight;     //set 1 for no weighted graph
        while (new_dist < old_dist) {
            if (compare_and_swap(dist[v.des], old_dist, new_dist)) {
                size_t dest_bin = new_dist/delta;
                if (dest_bin >= local_bins.size())
                    local_bins.resize(dest_bin+1);
                local_bins[dest_bin].push_back(v.des);
                break;
            }
            old_dist = dist[v.des];      // swap failed, recheck dist update & retry
        }
    }
}

pvector<WeightT> DeltaStep(SpruceTransVer& g, NodeID source, WeightT delta, uint32_t num_nodes, long num_edges);

#endif //GRAPHINDEX_SSSP_H
