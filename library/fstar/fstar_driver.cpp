/*
    Author: Haoxuan Xie
*/

#include "fstar_driver.h"

namespace gfe::library {

    /*****************************************************************************
     *                                                                           *
     *  Init                                                                     *
     *                                                                           *
     *****************************************************************************/
    ForwardStarDriver::ForwardStarDriver(bool is_directed) : m_is_directed(is_directed), vertex_num(1), edge_num(0) {
        std::ifstream fin("settings");
        int d;
        fin >> d;
        std::vector<int> a(d);
        for (auto& i : a) fin >> i;
        bool enable_query = 0;
        fin >> enable_query;
        G = new ForwardStar(d, a, enable_query);
    }

    ForwardStarDriver::~ForwardStarDriver() {
        delete G;
    }

    void ForwardStarDriver::on_thread_init(int thread_id) {
        // Not implemented
    }

    void ForwardStarDriver::on_thread_destroy(int thread_id) {
        // Not implemented
    }

    void* ForwardStarDriver::handle_impl() {
        return nullptr;
    }

    /*****************************************************************************
     *                                                                           *
     *  Updates & point look ups                                                 *
     *                                                                           *
     *****************************************************************************/
    uint64_t ForwardStarDriver::num_edges() const {
        return edge_num;
    }

    uint64_t ForwardStarDriver::num_vertices() const {
        return vertex_num;
    }

    bool ForwardStarDriver::has_vertex(uint64_t vertex_id) const {
        auto u = G->vertex_index->RetrieveVertex(vertex_id);
        return u != nullptr;
    }

    double ForwardStarDriver::get_weight(uint64_t source, uint64_t destination) const {
        // Not supported
        return 0.0;
    }

    bool ForwardStarDriver::is_directed() const {
        return m_is_directed;
    }

    void ForwardStarDriver::set_timeout(uint64_t seconds) {
        // m_timeout = std::chrono::seconds{ seconds };
    }

    void ForwardStarDriver::set_thread_affinity(bool value) {
        // Not implemented
    }

    bool ForwardStarDriver::has_thread_affinity() const {
        return true;
    }

    bool ForwardStarDriver::has_read_only_transactions() const {
        return true;
    }

    bool ForwardStarDriver::add_vertex(uint64_t vertex_id) {
        // Use add_edge function instead
        // vertex_num++;
        // auto u = G->vertex_index->RetrieveVertex(vertex_id, true);
        return true;
    }

    bool ForwardStarDriver::remove_vertex(uint64_t vertex_id) {
        return G->vertex_index->DeleteVertex(vertex_id);
    }

    bool ForwardStarDriver::add_edge(gfe::graph::WeightedEdge e) {
        edge_num++;
        G->InsertEdge(e.m_source, e.m_destination, e.m_weight);
        return true;
    }

    bool ForwardStarDriver::add_edge_v2(gfe::graph::WeightedEdge e) {
        edge_num++;
        G->InsertEdge(e.m_source, e.m_destination, e.m_weight);
        if (!m_is_directed) G->InsertEdge(e.m_destination, e.m_source, e.m_weight);
        return true;
    }

    bool ForwardStarDriver::update_edge(gfe::graph::WeightedEdge e) {
        return G->UpdateEdge(e.m_source, e.m_destination, e.m_weight);
    }

    bool ForwardStarDriver::remove_edge(gfe::graph::Edge e) {
        edge_num--;
        G->DeleteEdge(e.m_source, e.m_destination);
        if (!m_is_directed) G->DeleteEdge(e.m_destination, e.m_source);
        return true;
    }

    /*****************************************************************************
     *                                                                           *
     *  Dump                                                                     *
     *                                                                           *
     *****************************************************************************/
    void ForwardStarDriver::dump_ostream(std::ostream &out) const {
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
    void ForwardStarDriver::bfs(uint64_t source_vertex_id, const char* dump2file) {
        vertex_num = G->vertex_index->cnt;
        auto p = DOBFS(G, source_vertex_id, vertex_num, edge_num, -1);
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
    void ForwardStarDriver::pagerank(uint64_t num_iterations, double damping_factor, const char* dump2file) {
        vertex_num = G->vertex_index->cnt;
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
    void ForwardStarDriver::wcc(const char* dump2file) {
        vertex_num = G->vertex_index->cnt;
        ShiloachVishkin(G, vertex_num);
    }

    /*****************************************************************************
     *                                                                           *
     *  CDLP                                                                     *
     *                                                                           *
     *****************************************************************************/
    // same impl~ as the one done for llama
    void ForwardStarDriver::cdlp(uint64_t max_iterations, const char* dump2file) {
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
    void ForwardStarDriver::lcc(const char* dump2file) {
        vertex_num = G->vertex_index->cnt;
        OrderedCount(G, vertex_num);
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
    void ForwardStarDriver::sssp(uint64_t source_vertex_id, const char* dump2file) {
        vertex_num = G->vertex_index->cnt;
        DeltaStep(G, source_vertex_id, 2.0, vertex_num, edge_num);
    }
}