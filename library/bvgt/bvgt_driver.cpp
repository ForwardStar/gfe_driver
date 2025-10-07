//
// Created by sjf on 9/15/2022.
//

#include "bvgt_driver.h"

namespace gfe::library {

/*****************************************************************************
 *                                                                           *
 *  Init                                                                     *
 *                                                                           *
 *****************************************************************************/
    BVGTDriver::BVGTDriver(bool is_directed) : top_block(new SpruceTransVer::TopBlock),
                                               m_is_directed(is_directed), vertex_num(1),
    edge_num(0){
//        if (is_directed == true) { throw std::invalid_argument("Graph is directed"); }
//        else {
////            throw std::invalid_argument("Graph is undirected");
//        }
    }

    BVGTDriver::~BVGTDriver() {

    }

    void BVGTDriver::on_thread_init(int thread_id) {
//        throw NotImplemented();
    }

    void BVGTDriver::on_thread_destroy(int thread_id) {

    }

    void* BVGTDriver::handle_impl() {
        return top_block;
    }

/*****************************************************************************
 *                                                                           *
 *  Updates & point look ups                                                 *
 *                                                                           *
 *****************************************************************************/

    uint64_t BVGTDriver::num_edges() const {
        return edge_num;
    }

    uint64_t BVGTDriver::num_vertices() const {
        return vertex_num;
    }

    bool BVGTDriver::has_vertex(uint64_t vertex_id) const {
        return true;
    }

    double BVGTDriver::get_weight(uint64_t source, uint64_t destination) const {
        return 0.0;
    }

    bool BVGTDriver::is_directed() const {
        return m_is_directed;
    }

    void BVGTDriver::set_timeout(uint64_t seconds) {
//        m_timeout = chrono::seconds{seconds};
    }

    void BVGTDriver::set_thread_affinity(bool value) {

    }

    bool BVGTDriver::has_thread_affinity() const {
        return true;
    }

    bool BVGTDriver::has_read_only_transactions() const {
        return true;
    }

    bool BVGTDriver::add_vertex(uint64_t vertex_id) {
        // using add_vertex to change vertex_num
        vertex_num = vertex_id;
        return true;
    }

    bool BVGTDriver::remove_vertex(uint64_t vertex_id) {
//        COUT_DEBUG("remove vertex: " << vertex_id);
        return true;
    }

    bool BVGTDriver::get_neighbors(uint64_t vertex_id) {
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbors;
        SpruceTransVer::get_neighbours((SpruceTransVer::TopBlock*)top_block, (uint32_t)vertex_id, neighbors);
        return true;
    }

    bool BVGTDriver::get_two_hop_neighbors(uint64_t vertex_id) {
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbors;
        SpruceTransVer::get_neighbours((SpruceTransVer::TopBlock*)top_block, (uint32_t)vertex_id, neighbors);
        for (auto e : neighbors) {
            std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbors2;
            SpruceTransVer::get_neighbours((SpruceTransVer::TopBlock*)top_block, (uint32_t)e.des, neighbors2);
        }
        return true;
    }

    bool BVGTDriver::add_edge(gfe::graph::WeightedEdge e) {
        edge_num++;
        return SpruceTransVer::InsertEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_source, (uint32_t)e.m_destination, (float)e.m_weight});
    }

    bool BVGTDriver::add_edge_v2(gfe::graph::WeightedEdge e) {
        edge_num++;
        // for graph analysis
       if (e.m_destination >= vertex_num) vertex_num = e.m_destination + 1;
       if (e.m_source >= vertex_num) vertex_num = e.m_source + 1;

//       if(!is_directed()){
        SpruceTransVer::InsertEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_source, (uint32_t)e.m_destination, (float)e.m_weight});
        SpruceTransVer::InsertEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_destination, (uint32_t)e.m_source, (float)e.m_weight});
//        }
//       else {
//           SpruceTransVer::InsertEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_source, (uint32_t)e.m_destination, (float)e.m_weight});
//       }
        return true;
    }

    bool BVGTDriver::update_edge(gfe::graph::WeightedEdge e) {
//        std::cout << "Updateing Edge..." << std::endl;

//        {
//            //only for mixed workloads
//            if (e.m_destination >= vertex_num) {vertex_num = e.m_destination + 1; std::cout <<vertex_num << std::endl;}
//            if (e.m_source >= vertex_num) {vertex_num = e.m_source + 1; std::cout <<vertex_num << std::endl; }
//        }
        SpruceTransVer::UpdateEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_destination, (uint32_t)e.m_source, (float)e.m_weight});
        return SpruceTransVer::UpdateEdge((SpruceTransVer::TopBlock*)top_block, SpruceTransVer::WeightedEdge{(uint32_t)e.m_source, (uint32_t)e.m_destination, (float)e.m_weight});
//        return true;
    }

    bool BVGTDriver::remove_edge(gfe::graph::Edge e) {
        edge_num--;
        SpruceTransVer::DeleteEdge((SpruceTransVer::TopBlock*)top_block, (uint32_t)e.m_source, (uint32_t)e.m_destination);
        SpruceTransVer::DeleteEdge((SpruceTransVer::TopBlock*)top_block, (uint32_t)e.m_destination, (uint32_t)e.m_source);
        return true;
    }

/*****************************************************************************
 *                                                                           *
 *  Dump                                                                     *
 *                                                                           *
 *****************************************************************************/

    void BVGTDriver::dump_ostream(std::ostream &out) const {

    }


/*****************************************************************************
 *                                                                           *
 *  Helpers                                                                  *
 *                                                                           *
 *****************************************************************************/
// Translate the logical into real vertices IDs. Materialization step at the end of a Graphalytics algorithm


//    template<typename T, bool negative_scores = true>
//    void BVGTDriver::save_results(std::vector<std::pair<uint64_t, T>> &result, const char* dump2file) {
//
//    }

/*****************************************************************************
 *                                                                           *
 *  BFS                                                                      *
 *                                                                           *
 *****************************************************************************/
//    namespace { // anonymous

/*
GAP Benchmark Suite
Kernel: Breadth-First Search (BFS)
Author: Scott Beamer
Will return parent array for a BFS traversal from a source vertex
This BFS implementation makes use of the Direction-Optimizing approach [1].
It uses the alpha and beta parameters to determine whether to switch search
directions. For representing the frontier, it uses a SlidingQueue for the
top-down approach and a Bitmap for the bottom-up approach. To reduce
false-sharing for the top-down approach, thread-local QueueBuffer's are used.
To save time computing the number of edges exiting the frontier, this
implementation precomputes the degrees in bulk at the beginning by storing
them in parent array as negative numbers. Thus the encoding of parent is:
  parent[x] < 0 implies x is unvisited and parent[x] = -out_degree(x)
  parent[x] >= 0 implies x been visited
[1] Scott Beamer, Krste Asanović, and David Patterson. "Direction-Optimizing
    Breadth-First Search." International Conference on High Performance
    Computing, Networking, Storage and Analysis (SC), Salt Lake City, Utah,
    November 2012.
*/


        void BVGTDriver::bfs(uint64_t source_vertex_id, const char* dump2file) {
        std::vector<uint32_t> neighbours;
        SpruceTransVer::get_neighbours_only((SpruceTransVer::TopBlock*)top_block, source_vertex_id, neighbours);
        DOBFS((SpruceTransVer::TopBlock*)top_block, (uint32_t)source_vertex_id, vertex_num, edge_num, neighbours.size());
        }

/*****************************************************************************
 *                                                                           *
 *  PageRank                                                                 *
 *                                                                           *
 *****************************************************************************/

// Implementation based on the reference PageRank for the GAP Benchmark Suite
// https://github.com/sbeamer/gapbs
// The reference implementation has been written by Scott Beamer
//
// Copyright (c) 2015, The Regents of the University of California (Regents)
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the Regents nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL REGENTS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*
GAP Benchmark Suite
Kernel: PageRank (PR)
Author: Scott Beamer

Will return pagerank scores for all vertices once total change < epsilon

This PR implementation uses the traditional iterative approach. This is done
to ease comparisons to other implementations (often use same algorithm), but
it is not necesarily the fastest way to implement it. It does perform the
updates in the pull direction to remove the need for atomics.
*/




        void BVGTDriver::pagerank(uint64_t num_iterations, double damping_factor, const char* dump2file) {
            PageRankPull((SpruceTransVer::TopBlock*)top_block, num_iterations, vertex_num);
        }

/*****************************************************************************
 *                                                                           *
 *  WCC                                                                      *
 *                                                                           *
 *****************************************************************************/
// Implementation based on the reference WCC for the GAP Benchmark Suite
// https://github.com/sbeamer/gapbs
// The reference implementation has been written by Scott Beamer
//
// Copyright (c) 2015, The Regents of the University of California (Regents)
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the Regents nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL REGENTS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


//#define DEBUG_WCC
/*
GAP Benchmark Suite
Kernel: Connected Components (CC)
Author: Scott Beamer

Will return comp array labelling each vertex with a connected component ID

This CC implementation makes use of the Shiloach-Vishkin [2] algorithm with
implementation optimizations from Bader et al. [1]. Michael Sutton contributed
a fix for directed graphs using the min-max swap from [3], and it also produces
more consistent performance for undirected graphs.

[1] David A Bader, Guojing Cong, and John Feo. "On the architectural
    requirements for efficient execution of graph algorithms." International
    Conference on Parallel Processing, Jul 2005.

[2] Yossi Shiloach and Uzi Vishkin. "An o(logn) parallel connectivity algorithm"
    Journal of Algorithms, 3(1):57–67, 1982.

[3] Kishore Kothapalli, Jyothish Soman, and P. J. Narayanan. "Fast GPU
    algorithms for graph connectivity." Workshop on Large Scale Parallel
    Processing, 2010.
*/

// The hooking condition (comp_u < comp_v) may not coincide with the edge's
// direction, so we use a min-max swap such that lower component IDs propagate
// independent of the edge's direction.

        void BVGTDriver::wcc(const char* dump2file) {
            ShiloachVishkin((SpruceTransVer::TopBlock*)top_block, vertex_num);
        }

/*****************************************************************************
 *                                                                           *
 *  CDLP                                                                     *
 *                                                                           *
 *****************************************************************************/
// same impl~ as the one done for llama



        void BVGTDriver::cdlp(uint64_t max_iterations, const char* dump2file) {
            cdlp_m((SpruceTransVer::TopBlock*)top_block, (uint32_t)max_iterations, (uint32_t)vertex_num);
        }

/*****************************************************************************
 *                                                                           *
 *  LCC                                                                      *
 *                                                                           *
 *****************************************************************************/
//#define DEBUG_LCC
#if defined(DEBUG_LCC)
#define COUT_DEBUG_LCC(msg) COUT_DEBUG(msg)
#else
#define COUT_DEBUG_LCC(msg)
#endif

// loosely based on the impl~ made for Stinger



        void BVGTDriver::lcc(const char* dump2file) {
            OrderedCount((SpruceTransVer::TopBlock*)top_block, vertex_num);
        }

/*****************************************************************************
 *                                                                           *
 *  SSSP                                                                     *
 *                                                                           *
 *****************************************************************************/
// Implementation based on the reference SSSP for the GAP Benchmark Suite
// https://github.com/sbeamer/gapbs
// The reference implementation has been written by Scott Beamer
//
// Copyright (c) 2015, The Regents of the University of California (Regents)
// All Rights Reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the Regents nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL REGENTS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


        void BVGTDriver::sssp(uint64_t source_vertex_id, const char* dump2file) {
            DeltaStep((SpruceTransVer::TopBlock*)top_block, source_vertex_id, 2.0, vertex_num, edge_num);
        }

/*
GAP Benchmark Suite
Kernel: Betweenness Centrality (BC)
Author: Scott Beamer

Will return array of approx betweenness centrality scores for each vertex

This BC implementation makes use of the Brandes [1] algorithm with
implementation optimizations from Madduri et al. [2]. It is only approximate
because it does not compute the paths from every start vertex, but only a small
subset of them. Additionally, the scores are normalized to the range [0,1].

As an optimization to save memory, this implementation uses a Bitmap to hold
succ (list of successors) found during the BFS phase that are used in the back-
propagation phase.

[1] Ulrik Brandes. "A faster algorithm for betweenness centrality." Journal of
    Mathematical Sociology, 25(2):163–177, 2001.

[2] Kamesh Madduri, David Ediger, Karl Jiang, David A Bader, and Daniel
    Chavarria-Miranda. "A faster parallel algorithm and efficient multithreaded
    implementations for evaluating betweenness centrality on massive datasets."
    International Symposium on Parallel & Distributed Processing (IPDPS), 2009.
*/


        void BVGTDriver::bc(uint64_t max_iterations, const char* dump2file) {
            pvector<ScoreT> scores(vertex_num, 0);
            pvector<double> path_counts(vertex_num);
            Bitmap succ(vertex_num);
            std::vector<SlidingQueue<NodeID>::iterator> depth_index;
            SlidingQueue<NodeID> queue(vertex_num);
            NodeID sp = 0;
            for (NodeID iter=0; iter < max_iterations; iter++) {
                NodeID source = sp++ % vertex_num;
                path_counts.fill(0);
                depth_index.resize(0);
                queue.reset();
                succ.reset();

                // PBFS
                pvector<NodeID> depths(vertex_num, -1);
                std::vector<std::atomic<bool>> occurred(vertex_num);
                depths[source] = 0;
                path_counts[source] = 1;
                queue.push_back(source);
                depth_index.push_back(queue.begin());
                queue.slide_window();
                #pragma omp parallel
                {
                    NodeID depth = 0;
                    QueueBuffer<NodeID> lqueue(queue);
                    while (!queue.empty()) {
                        depth++;
                        #pragma omp for schedule(dynamic, 64) nowait
                        for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
                            NodeID u = *q_iter;
                            std::vector<SpruceTransVer::WeightedOutEdgeSimple> u_neighbors;
                            SpruceTransVer::get_neighbours((SpruceTransVer::TopBlock*)top_block, (uint32_t)u, u_neighbors);
                            for (auto &e : u_neighbors) {
                            NodeID v = e.des;
                            if ((depths[v] == -1) &&
                                (compare_and_swap(depths[v], static_cast<NodeID>(-1), depth))) {
                                bool first_time = !occurred[v].exchange(true);
                                if (first_time) {
                                    // Yet normally this is unnecessary but I don't know the compare_and_swap sometimes works malfunctionally...
                                    lqueue.push_back(v);
                                }
                            }
                            if (depths[v] == depth) {
                                succ.set_bit_atomic(v);
                                #pragma omp atomic
                                path_counts[v] += path_counts[u];
                            }
                            }
                        }
                        lqueue.flush();
                        #pragma omp barrier
                        #pragma omp single
                        {
                            depth_index.push_back(queue.begin());
                            queue.slide_window();
                        }
                    }
                }
                depth_index.push_back(queue.begin());

                pvector<ScoreT> deltas(vertex_num, 0);
                for (int d=depth_index.size()-2; d >= 0; d--) {
                    #pragma omp parallel for schedule(dynamic, 64)
                    for (auto it = depth_index[d]; it < depth_index[d+1]; it++) {
                        NodeID u = *it;
                        ScoreT delta_u = 0;
                        std::vector<SpruceTransVer::WeightedOutEdgeSimple> g_out_neigh;
                        SpruceTransVer::get_neighbours((SpruceTransVer::TopBlock*)top_block, (uint32_t)u, g_out_neigh);
                        for (auto &e : g_out_neigh) {
                            NodeID v = e.des;
                            if (succ.get_bit(v)) {
                                delta_u += (path_counts[u] / path_counts[v]) * (1 + deltas[v]);
                            }
                        }
                        deltas[u] = delta_u;
                        scores[u] += delta_u;
                    }
                }
            }
            // normalize scores
            ScoreT biggest_score = 0;
            #pragma omp parallel for reduction(max : biggest_score)
            for (NodeID n=0; n < vertex_num; n++)
                biggest_score = std::max(biggest_score, scores[n]);
            #pragma omp parallel for
            for (NodeID n=0; n < vertex_num; n++)
                scores[n] = scores[n] / biggest_score;
        }

/*****************************************************************************
 *                                                                           *
 *  LCC, sort-merge implementation                                           *
 *                                                                           *
 *****************************************************************************/
/**
 * Algorithm parameters
 */

//    } // namespace
}