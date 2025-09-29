/*
    Author: Haoxuan Xie
    Year: 2025
*/
#include "radixgraph_driver.h"

namespace gfe::library {

    /*****************************************************************************
     *                                                                           *
     *  Init                                                                     *
     *                                                                           *
     *****************************************************************************/
    RadixGraphDriver::RadixGraphDriver(bool is_directed) : m_is_directed(is_directed), vertex_num(1), edge_num(0) {
        std::ifstream fin("settings");
        int d;
        fin >> d;
        std::vector<int> a(d);
        for (auto& i : a) fin >> i;
        G = new RadixGraph(d, a, _num_threads);
    }

    RadixGraphDriver::~RadixGraphDriver() {
        delete G;
    }

    void RadixGraphDriver::on_thread_init(int thread_id) {
        G->thread_id_local = thread_id;
    }

    void RadixGraphDriver::on_thread_destroy(int thread_id) {
        G->thread_id_local = -1;
    }

    void RadixGraphDriver::on_main_init(int num_threads) {
        _num_threads = num_threads;
        if (G) G->Init(_num_threads);
    }

    void RadixGraphDriver::set_mixed_workloads(bool is_mixed_workloads) {
        G->is_mixed_workloads = is_mixed_workloads;
        G->vertex_index->is_mixed_workloads = is_mixed_workloads;
    }

    void* RadixGraphDriver::handle_impl() {
        return nullptr;
    }

    /*****************************************************************************
     *                                                                           *
     *  Updates & point look ups                                                 *
     *                                                                           *
     *****************************************************************************/
    uint64_t RadixGraphDriver::num_edges() const {
        return edge_num;
    }

    uint64_t RadixGraphDriver::num_vertices() const {
        return vertex_num;
    }

    bool RadixGraphDriver::has_vertex(uint64_t vertex_id) const {
        auto u = G->vertex_index->RetrieveVertex(vertex_id);
        return u != nullptr;
    }

    double RadixGraphDriver::get_weight(uint64_t source, uint64_t destination) const {
        // Not supported
        return 0.0;
    }

    bool RadixGraphDriver::is_directed() const {
        return m_is_directed;
    }

    void RadixGraphDriver::set_timeout(uint64_t seconds) {
        // m_timeout = std::chrono::seconds{ seconds };
    }

    void RadixGraphDriver::set_thread_affinity(bool value) {
        // Not implemented
    }

    bool RadixGraphDriver::has_thread_affinity() const {
        return true;
    }

    bool RadixGraphDriver::has_read_only_transactions() const {
        return true;
    }

    bool RadixGraphDriver::add_vertex(uint64_t vertex_id) {
        // vertex_num = std::max((uint64_t)vertex_num, vertex_id + 1);
        G->vertex_index->InsertSimpleVertex(vertex_id);
        return true;
    }

    bool RadixGraphDriver::remove_vertex(uint64_t vertex_id) {
        return G->vertex_index->DeleteVertex(vertex_id);
    }

    bool RadixGraphDriver::get_neighbors(uint64_t vertex_id) {
        std::vector<WeightedEdge> neighbors;
        G->GetNeighbours(vertex_id, neighbors);
        return true;
    }

    bool RadixGraphDriver::get_two_hop_neighbors(uint64_t vertex_id) {
        std::vector<WeightedEdge> neighbors;
        int t = G->GetGlobalTimestamp(); // Get timestamp for consistency (1-hop and 2-hop neighbors should be in the same graph snapshot)
        G->GetNeighbours(vertex_id, neighbors, false, t);
        for (auto e : neighbors) {
            std::vector<WeightedEdge> neighbors2;
            G->GetNeighboursByOffset(e.idx, neighbors2, false, t);
        }
        return true;
    }

    bool RadixGraphDriver::add_edge(gfe::graph::WeightedEdge e) {
        edge_num++;
        G->InsertEdge(e.m_source, e.m_destination, e.m_weight);
        return true;
    }

    bool RadixGraphDriver::add_edge_v2(gfe::graph::WeightedEdge e) {
        edge_num++;
        G->InsertEdge(e.m_source, e.m_destination, e.m_weight);
        if (!m_is_directed) G->InsertEdge(e.m_destination, e.m_source, e.m_weight);
        return true;
    }

    bool RadixGraphDriver::update_edge(gfe::graph::WeightedEdge e) {
        return G->UpdateEdge(e.m_source, e.m_destination, e.m_weight);
    }

    bool RadixGraphDriver::remove_edge(gfe::graph::Edge e) {
        edge_num--;
        G->DeleteEdge(e.m_source, e.m_destination);
        if (!m_is_directed) G->DeleteEdge(e.m_destination, e.m_source);
        return true;
    }

    void RadixGraphDriver::build() {
        G->CreateSnapshots();
    }

    /*****************************************************************************
     *                                                                           *
     *  Dump                                                                     *
     *                                                                           *
     *****************************************************************************/
    void RadixGraphDriver::dump_ostream(std::ostream &out) const {
        // Not implemented
    }


    /*****************************************************************************
     *                                                                           *
     *  Helpers                                                                  *
     *                                                                           *
     *****************************************************************************/

    /*****************************************************************************
     *                                                                           *
     *  BFS                                                                      *
     *                                                                           *
     *****************************************************************************/

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
    void RadixGraphDriver::bfs(uint64_t source_vertex_id, const char* dump2file) {
        auto p = DOBFS(G, source_vertex_id, G->vertex_index->cnt, edge_num, -1);
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
    void RadixGraphDriver::pagerank(uint64_t num_iterations, double damping_factor, const char* dump2file) {
        PageRankPull(G, num_iterations, vertex_num);
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
    void RadixGraphDriver::wcc(const char* dump2file) {
        ShiloachVishkin(G, vertex_num);
    }

    /*****************************************************************************
     *                                                                           *
     *  CDLP                                                                     *
     *                                                                           *
     *****************************************************************************/
    // same impl~ as the one done for llama
    void RadixGraphDriver::cdlp(uint64_t max_iterations, const char* dump2file) {
        // Not implemented
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
    void RadixGraphDriver::lcc(const char* dump2file) {
        OrderedCount(G, G->vertex_index->cnt);
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
    void RadixGraphDriver::sssp(uint64_t source_vertex_id, const char* dump2file) {
        DeltaStep(G, source_vertex_id, 2.0, G->vertex_index->cnt, edge_num);
    }

    void RadixGraphDriver::print_stats() {
        auto info = G->GetDebugInfo();
        if (info.empty()) {
            return;
        }
        // Sort by degree
        std::sort(info.begin(), info.end(), [](const RadixGraph::DebugInfo& a, const RadixGraph::DebugInfo& b) {
            return a.deg > b.deg;
        });
        std::cout << "Top 10 vertices by degree:\n";
        for (size_t i = 0; i < std::min(info.size(), size_t(10)); ++i) {
            const auto& vertex_info = info[i];
            std::cout << "Node " << vertex_info.node << " Deg " << vertex_info.deg << " Total time " << vertex_info.t_total << " Compaction time " << vertex_info.t_compact << "\n";
        }
    }
}