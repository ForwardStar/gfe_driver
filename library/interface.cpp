/**
 * Copyright (C) 2019 Dean De Leo, email: dleo[at]cwi.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "interface.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>

#include "common/error.hpp"
#include "common/quantity.hpp"
#include "common/system.hpp"
#include "common/timer.hpp"

#include "baseline/adjacency_list.hpp"
#include "baseline/csr.hpp"
#include "baseline/dummy.hpp"

#include "../configuration.hpp"

#include "reader/reader.hpp"
#if defined(HAVE_STINGER)
#include "stinger/stinger.hpp"
#include "stinger-dv/stinger-dv.hpp" // dense domain of vertices
#endif
#if defined(HAVE_GRAPHONE)
#include "graphone/graphone.hpp"
#endif
#if defined(HAVE_LIVEGRAPH)
#include "livegraph/livegraph_driver.hpp"
#endif
#if defined(HAVE_TESEO)
#include "teseo/teseo_driver.hpp"
#include "teseo/teseo_real_vtx.hpp"
#endif

#if defined(HAVE_SORTLEDTON)
#include "sortledton/sortledton_driver.hpp"
#endif

#if defined(HAVE_SORTLEDTONV2)
#include "sortledton_v2/sortledton_driver_v2.hpp"
#endif

#if defined(HAVE_BVGT)
#include "bvgt/bvgt_driver.h"
#endif

#if defined(HAVE_GTX)
#include "gtx/gtx_driver.hpp"
#endif

#if defined(HAVE_RG)
#include "radixgraph/radixgraph_driver.h"
#endif

using namespace std;

/*****************************************************************************
 *                                                                           *
 *  Debug                                                                    *
 *                                                                           *
 *****************************************************************************/
//#define DEBUG
namespace gfe { extern mutex _log_mutex [[maybe_unused]]; }
#define COUT_DEBUG_FORCE(msg) { std::scoped_lock<std::mutex> lock{::gfe::_log_mutex}; std::cout << "[Interface::" << __FUNCTION__ << "] " << msg << std::endl; }
#if defined(DEBUG)
    #define COUT_DEBUG(msg) COUT_DEBUG_FORCE(msg)
#else
    #define COUT_DEBUG(msg)
#endif


namespace gfe::library {
/*****************************************************************************
 *                                                                           *
 *  Factory                                                                  *
 *                                                                           *
 *****************************************************************************/
ImplementationManifest::ImplementationManifest(const string& name, const string& description, unique_ptr<Interface> (*factory)(bool)) :
    m_name(name), m_description(description), m_factory(factory){ }

std::unique_ptr<Interface> generate_baseline_adjlist(bool directed_graph){ // directed or undirected graph
    return unique_ptr<Interface>{ new AdjacencyList(directed_graph) };
}

std::unique_ptr<Interface> generate_baseline_adjlist_no_ts(bool directed_graph){
    return unique_ptr<Interface>{ new AdjacencyList(directed_graph, /* thread safe ? */ false) };
}

std::unique_ptr<Interface> generate_csr(bool directed_graph){
    return unique_ptr<Interface>{ new CSR(directed_graph, /* numa interleaved ? */ false) };
}
std::unique_ptr<Interface> generate_csr_lcc(bool directed_graph){
    return unique_ptr<Interface>{ new CSR_LCC(directed_graph, /* numa interleaved ? */ false) };
}
std::unique_ptr<Interface> generate_csr_numa(bool directed_graph){
    return unique_ptr<Interface>{ new CSR(directed_graph, /* numa interleaved ? */ true) };
}
std::unique_ptr<Interface> generate_csr_lcc_numa(bool directed_graph){
    return unique_ptr<Interface>{ new CSR_LCC(directed_graph, /* numa interleaved ? */ true) };
}

std::unique_ptr<Interface> generate_dummy(bool directed_graph){
    return unique_ptr<Interface>{ new Dummy(directed_graph) };
}

#if defined(HAVE_STINGER)
std::unique_ptr<Interface> generate_stinger(bool directed_graph){
    return unique_ptr<Interface>{ new Stinger(directed_graph) };
}
std::unique_ptr<Interface> generate_stinger_dv(bool directed_graph){
    return unique_ptr<Interface>{ new StingerDV(directed_graph) };
}
std::unique_ptr<Interface> generate_stinger_ref(bool directed_graph){
    return unique_ptr<Interface>{ new StingerRef(directed_graph) };
}
#endif

#if defined(HAVE_GRAPHONE)
// Heuristics to set the max number of vertices that the implementation can sustain.
// We wish to create up to 4G vertices, but unfortunately this limit is too high for usage in a workstation machines,
// so we decrease the value depending on the amount of available memory
static uint64_t graphone_max_num_vertices(){
    uint64_t ram = common::get_total_ram(); // in bytes
    // Ensure that the vertex array does not use more than 20% of the available RAM
    uint64_t num_vertices = std::min<uint64_t>(4ull<<30, ram/(5 * 8)); // 8 bytes per vertex, 1/5 of the total ram

    { // critical section, for the output
        std::scoped_lock<std::mutex> lock{_log_mutex};
        cout << "GraphOne: capacity of the vertex array implicitly set to: " << common::ComputerQuantity(num_vertices) << " vertices " << endl;
    }

    return num_vertices;
}

std::unique_ptr<Interface> generate_graphone_cons_sp(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ true, /* blind writes ? */ false, /* ignore build ? */ false, /* ref impl ? */ false, N) };
}
std::unique_ptr<Interface> generate_graphone_cons_dv(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ false, /* blind writes ? */ false, /* ignore build ? */ false, /* ref impl ? */ false, N) };
}
std::unique_ptr<Interface> generate_graphone_bw_sp(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ true, /* blind writes ? */ true, /* ignore build ? */ false, /* ref impl ? */ false,  N) };
}
std::unique_ptr<Interface> generate_graphone_bw_dv(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ false, /* blind writes ? */ true, /* ignore build ? */ false, /* ref impl ? */ false, N) };
}
std::unique_ptr<Interface> generate_graphone_bw_sp_ignore_build(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ true, /* blind writes ? */ true, /* ignore build ? */ true, /* ref impl ? */ false, N) };
}
std::unique_ptr<Interface> generate_graphone_bw_dv_ignore_build(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ false, /* blind writes ? */ true, /* ignore build ? */ true, /* ref impl ? */ false, N) };
}
std::unique_ptr<Interface> generate_graphone_ref(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ true, /* blind writes ? */ true, /* ignore build ? */ false, /* ref impl ? */ true, N) };
}
std::unique_ptr<Interface> generate_graphone_ref_ignore_build(bool directed_graph){
    uint64_t N = graphone_max_num_vertices();
    return unique_ptr<Interface>{ new GraphOne(directed_graph, /* vtx dict ? */ true, /* blind writes ? */ true, /* ignore build ? */ true, /* ref impl ? */ true, N) };
}
#endif

#if defined(HAVE_LIVEGRAPH)
std::unique_ptr<Interface> generate_livegraph_ro(bool directed_graph){ // read only transactions for Graphalytics
    return unique_ptr<Interface>( new LiveGraphDriver(directed_graph, /*read_only ? */ true));
}
std::unique_ptr<Interface> generate_livegraph_rw(bool directed_graph){ // read-write transactions for Graphalytics
    return unique_ptr<Interface>( new LiveGraphDriver(directed_graph, /*read_only ? */ false));
}
#endif

#if defined(HAVE_TESEO)
std::unique_ptr<Interface> generate_teseo(bool directed_graph){
    return unique_ptr<Interface>{ new TeseoDriver(directed_graph) };
}
std::unique_ptr<Interface> generate_teseo_rw(bool directed_graph){
    return unique_ptr<Interface>{ new TeseoDriver(directed_graph, /* read only tx ? */ false ) };
}
std::unique_ptr<Interface> generate_teseo_lcc(bool directed_graph){ // LCC custom algorithm
    return unique_ptr<Interface>{ new TeseoDriverLCC(directed_graph) };
}
std::unique_ptr<Interface> generate_teseo_real_vtx(bool directed_graph){
    return unique_ptr<Interface>{ new TeseoRealVertices(directed_graph) };
}
std::unique_ptr<Interface> generate_teseo_real_vtx_lcc(bool directed_graph){
    return unique_ptr<Interface>{ new TeseoRealVerticesLCC(directed_graph) };
}
#endif

#if defined(HAVE_BVGT)
std::unique_ptr<Interface> generate_bvgt(bool directed_graph){
    return unique_ptr<Interface>{ new BVGTDriver(directed_graph) };
}
//std::unique_ptr<Interface> generate_bvgt_weighted(bool directed_graph){
//    return unique_ptr<Interface>{ new BVGTDriverWeighted(directed_graph)};
//}
//std::unique_ptr<Interface> generate_bvgt_64(bool directed_graph){
//    return unique_ptr<Interface>{ new BVGTDriver(directed_graph) };
//}
//std::unique_ptr<Interface> generate_bvgt_weighted_64(bool directed_graph){
//    return unique_ptr<Interface>{ new BVGTDriverWeighted(directed_graph)};
//}
#endif

#if defined(HAVE_GTX)
std::unique_ptr<Interface> generate_gtx(bool directed_graph){
    return unique_ptr<Interface>{ new GTXDriver(directed_graph, false) };
}
#endif

#if defined(HAVE_RG)
std::unique_ptr<Interface> generate_radixgraph(bool directed_graph){
    return unique_ptr<Interface>{ new RadixGraphDriver(directed_graph) };
}
#endif

#if defined(HAVE_SORTLEDTON)
std::unique_ptr<Interface> generate_sortledton(bool directed_graph) {
    auto& config = configuration();
    cout << "Running Sortledton with block size: " << config.block_size() << endl;
    return unique_ptr<Interface>{ new SortledtonDriver(directed_graph, 8, config.block_size()) };
}
#endif

#if defined(HAVE_SORTLEDTONV2)
std::unique_ptr<Interface> generate_sortledton_v2(bool directed_graph) {
    auto& config = configuration();
    cout << "Running Sortledton V2 with block size: " << config.block_size() << endl;
    return unique_ptr<Interface>{ new SortledtonDriverV2(directed_graph, config.block_size()) };
}
#endif

vector<ImplementationManifest> implementations() {
    vector<ImplementationManifest> result;

    // v2 25/06/2020: Updates, implicitly create a vertex referred in a new edge upon first reference with the method add_edge_v2
    // v3 14/04/2021: Fix the predicate in the TimeoutService
    result.emplace_back("baseline_v3", "Sequential baseline, based on adjacency list", &generate_baseline_adjlist);
    result.emplace_back("baseline_v3_seq", "Sequential baseline, non thread safe", &generate_baseline_adjlist_no_ts);

    // v2 14/04/2021: Fix the predicate in the TimeoutService
    // v3 19/04/2021: Materialization with a vector
    result.emplace_back("csr3", "CSR baseline", &generate_csr);
    result.emplace_back("csr3-lcc", "CSR baseline, sort-merge impl for the LCC kernel", &generate_csr_lcc);
    result.emplace_back("csr3-numa", "CSR baseline, allocate the internal arrays using all NUMA nodes", &generate_csr_numa);
    result.emplace_back("csr3-lcc-numa", "CSR baseline, allocate the internal arrays using all NUMA nodes, sort-merge impl for the LCC kernel", &generate_csr_lcc_numa);

    // Temporary, we run csr3-lcc on a single NUMA node to pin down if NUMA effects are resposible for SortedVectorAL being faster some times
    result.emplace_back("single-numa-node-csr3-lcc", "CSR baseline, sort-merge impl for the LCC kernel", &generate_csr_lcc);

    // v2 25/06/2020: Updates, implicitly create a vertex referred in a new edge upon first reference with the method add_edge_v2
    // v3 14/04/2021: Fix the predicate in the TimeoutService
    result.emplace_back("dummy_v3", "Dummy implementation of the interface, all operations are nop", &generate_dummy);

#if defined(HAVE_STINGER)
    // v2 12/06/2020: OMP dynamic scheduling in the Graphalytics kernels
    // v3 29/06/2020: add_edge_v2
    // v4 24/09/2020: do not use OpenMP in updates
    // v5 26/09/2020: completely disable vertex deletions
    // v6 14/04/2021: Fix the predicate in the TimeoutService
    // v7 19/04/2021: Materialization step with a vector
    result.emplace_back("stinger7", "Stinger library", &generate_stinger);
    result.emplace_back("stinger7-dv", "Stinger with dense vertices", &generate_stinger_dv);
    result.emplace_back("stinger7-ref", "Stinger with the GAPBS ref impl.", &generate_stinger_ref);
#endif

#if defined(HAVE_GRAPHONE)
    // v2 11/06/2020: Bug fix for the properties on the static views + OMP dynamic scheduling. Repeat all experiments for Graphalytics.
    // v3 25/06/2020: Updates, implicitly create a vertex referred in a new edge upon first reference with the method add_edge_v2
    // v4 27/06/2020: Fix the number of vertices. It only affects the variations with dense vertices.
    // v5 14/04/2021: Fix the predicate in the TimeoutService
    // v6 23/04/2021: Materialization step with a vector
    result.emplace_back("g1_v6-cons-sp", "GraphOne, consistency for updates, sparse vertices (vertex dictionary)", &generate_graphone_cons_sp);
    result.emplace_back("g1_v6-cons-dv", "GraphOne, consistency for updates, dense vertices", &generate_graphone_cons_dv);
    result.emplace_back("g1_v6-bw-sp", "GraphOne, blind writes, sparse vertices (vertex dictionary)", &generate_graphone_bw_sp);
    result.emplace_back("g1_v6-bw-dv", "GraphOne, blind writes, dense vertices", &generate_graphone_bw_dv);
    result.emplace_back("g1_v6-bw-sp-ignore-build", "GraphOne, blind writes, sparse vertices (vertex dictionary), new deltas/levels cannot be explicitly created", &generate_graphone_bw_sp_ignore_build);
    result.emplace_back("g1_v6-bw-dv-ignore-build", "GraphOne, blind writes, dense vertices, new deltas/levels cannot be explicitly created", &generate_graphone_bw_dv_ignore_build);
    result.emplace_back("g1_v6-ref", "GraphOne, reference GAP BS for the Graphalytics algorithms", &generate_graphone_ref);
    result.emplace_back("g1_v6-ref-ignore-build", "GraphOne, reference GAP BS for the Graphalytics algorithms", &generate_graphone_ref_ignore_build);
#endif

#if defined(HAVE_LIVEGRAPH)
    // v2 14/04/2021: Fix the predicate in the TimeoutService
    // v3 25/04/2021: Materialization step with a vector
    result.emplace_back("livegraph3_ro", "LiveGraph, use read-only transactions for the Graphalytics kernels", &generate_livegraph_ro);
    result.emplace_back("livegraph3_rw", "LiveGraph, use read-write transactions for the Graphalytics kernels", &generate_livegraph_rw);
#endif

#if defined(HAVE_TESEO)
    // v1 05/04/2020: initial version for evaluation
    // v2 28/04/2020: big rewrite: dense file, delayed rebalances, new leaf layout, new rebalancer logic. All experiments should be repeated, that is, ignore v1.
    // v3 27/05/2020: scan enhancements: AUX view, prefetching, NUMA, segment's pivot, direct pointers in the AUX view. All graphalytics & bm experiments should be repeated.
    // v4 15/06/2020: cursor state + support for R/W iterators. It only affects scans (Graphalytics & bm)
    // v5 26/06/2020: updates, implicitly create a vertex referred in a new edge upon first reference with the method add_edge_v2
    // v6 18/07/2020: vertex table
    // v7 08/11/2020: vertical partitioning
    // v8 23/11/2020: variable length leaves
    // v9 07/01/2021: bug fixes
    // v10 08/01/2021: set the thread affinity by default
    // v11 14/04/2021: Fix the predicate in the TimeoutService
    // v12 19/04/2021: Materialization with a vector (12b, bogus translation IDs in TeseoRealVertices)
    // v13 08/12/2021: Vertex table: uses pseudorandom hash function to avoid clustering on en-wiki dataset
    result.emplace_back("teseo.13", "Teseo", &generate_teseo);
    result.emplace_back("teseo-rw.13", "Teseo. Use read-write transactions for graphalytics, to measure their overhead", &generate_teseo_rw);
    result.emplace_back("teseo-lcc.13", "Teseo with a tuned implementation of the LCC kernel", &generate_teseo_lcc);
    result.emplace_back("teseo-dv.13b", "Teseo, dense vertices", &generate_teseo_real_vtx);
    result.emplace_back("teseo-lcc-dv.13b", "Teseo, dense vertices and sort-merge implementation of the LCC kernel", &generate_teseo_real_vtx_lcc);
#endif

#if defined(HAVE_BVGT)
    result.emplace_back("bvgt", "Spruce", &generate_bvgt);
//    result.emplace_back("bvgt-weighted", "BVGT for weighted graph", &generate_bvgt_weighted);
//    result.emplace_back("bvgt-64", "BVGT for 8B edges", &generate_bvgt_64);
//   result.emplace_back("bvgt-weighted", "BVGT for weighted graph", &generate_bvgt_weighted_64);
#endif

#if defined(HAVE_RG)
    result.emplace_back("radixgraph", "RadixGraph", &generate_radixgraph);
#endif

#if defined(HAVE_GTX)
    result.emplace_back("gtx", "GTX", &generate_gtx);
#endif

#if defined(HAVE_SORTLEDTON)
//    result.emplace_back("sortledton", "Sortledton", &generate_sortledton);
// v2: 11.07.2021 Bugfixex for size and version drawing.
//    result.emplace_back("sortledton.1", "Sortledton", &generate_sortledton);
    /**
     * v3: 25.07.2021
     * Carries over optimization applied in the microbenchmarks
     *   - BFS
     *     - does not use edge_count anymore but sums up while populating the distance array
     *     - removes unecessary double translation
     *   - LCC
     *     - uses now shared locks
     *     - uses the GFE driver implementation which is based on pthreads and not OpenMP like mine
     *   - SSSP
     *     - now use blocked iterators
     *   - Size versioning
     *     - I reuse old version records for neighbourhood size versioning
     *   - Skiplists
     *     - uses the correct probability function for deciding the height of new nodes
     **/
//    result.emplace_back("sortledton.2", "Sortledton", &generate_sortledton);

    /**
     * Temporary version.
     * - Iterators in Sorltedton do no acquire locks
     * - BFS is not run as first analytics algorithms. Therefore, GC is not running as the first run.
     * - PageRank is run first.
     */
//    result.emplace_back("sortledton.lock-free-analytics", "Sortledton", &generate_sortledton);

    /**
     * Temporary version.
     *  - uses raw memory access for the BFS bottom up step and small sets. No locks acquired.
     *  - do not use ANY of these results, I ran many experiments with this version.
     */
//    result.emplace_back("sortledton.raw-access-bfs", "Sortledton", &generate_sortledton);

    /**
     * v4: 26.07.2021
     * - Fixes bug in BFS which slowed down init_distances
     * - Fixes running debug code in release version
     * - compiles with O3 -march native and -mtune native
     **/
//    result.emplace_back("sortledton.3", "Sortledton", &generate_sortledton);
      /**
       * Test version used to try an incorrect system.
       */
//    result.emplace_back("sortledton-no-size-versioning", "Sortledton", &generate_sortledton);
      /**
       * Sortledton with steam gc implementation for adjacency set sizes.
       */
//    result.emplace_back("sortledton-steam-gc", "Sortledton", &generate_sortledton);
      /**
       * Sorltedton with steam gc implementation for adjacency set sizes and edge version chains.
       *
       * Measurements contained some debug code for the isolated workloads. We measure these again.
       */
//    result.emplace_back("sortledton-steam-gc-1", "Sortledton", &generate_sortledton);
//    result.emplace_back("sortledton-steam-gc-2", "Sortledton", &generate_sortledton);
  /**
   * Clean measurements with steam GC for both sizes and edges.
   */
    result.emplace_back("sortledton.4", "Sortledton", &generate_sortledton);
    /**
     * Tests version chain garbage collection for edges during analytics.
     * This did not win analytics latency.
     */
//    result.emplace_back("sortledton-steam-gc-3", "Sortledton", &generate_sortledton);
#endif

#if defined(HAVE_SORTLEDTONV2)
    result.emplace_back("sortledton.v2.1", "Sortledton V2", &generate_sortledton_v2);
#endif

    return result;
}

/*****************************************************************************
 *                                                                           *
 *  Base interface                                                           *
 *                                                                           *
 *****************************************************************************/
Interface::Interface(){}
Interface::~Interface(){}
void Interface::on_main_init(int num_threads){ };
void Interface::on_thread_init(int thread_id){ };
void Interface::on_thread_destroy(int thread_id){ } ;
void Interface::on_main_destroy(){ };
bool Interface::has_edge(uint64_t source, uint64_t destination) const {
    return !isnan(get_weight(source, destination));
}
void Interface::dump() const{
    dump_ostream(std::cout);
}
void Interface::dump(const std::string& path) const {
    fstream handle(path.c_str());
    if(!handle.good()) ERROR("[dump] Cannot open the file: `" << path << "'");
    dump_ostream(handle);
    handle.close();
}

void Interface::dump(const char* c_path) const {
    string path = c_path;
    dump(path);
}

bool Interface::is_undirected() const {
    return !is_directed();
}

void Interface::updates_start() { }

void Interface::updates_stop() { }

bool Interface::can_be_validated() const {
    return true;
}

bool Interface::has_weights() const {
  return true;
}

/*****************************************************************************
 *                                                                           *
 *  Update interface                                                         *
 *                                                                           *
 *****************************************************************************/
bool UpdateInterface::batch(const SingleUpdate* array, size_t array_sz, bool force){
    bool result = true;
    const SingleUpdate* __restrict A = array;
    COUT_DEBUG("batch: " << array_sz << ", force: " << force);

    if(force){

        // now a bit of a hack, we want all updates to succeed. An update may fail if a vertex is still being added
        // by another thread in the meanwhile
        for(uint64_t i = 0; i < array_sz; i++){
            if(A[i].m_weight >= 0){ // insert
                graph::WeightedEdge edge{A[i].m_source, A[i].m_destination, A[i].m_weight};
                if( ! add_edge(edge) ){ // avoid infinite loops/waits
                    auto op = [this](graph::WeightedEdge edge){ return add_edge(edge); };
                    batch_try_again(op, edge);
                }
            } else { // remove
                graph::Edge edge{A[i].m_source, A[i].m_destination};

                if ( ! remove_edge(edge) ){ // avoid infinite loops/waits
                    auto op = [this](graph::Edge edge){ return remove_edge(edge); };
                    batch_try_again(op, edge);
                }
            }
        }
    } else {
        for(uint64_t i = 0; i < array_sz; i++){
            if(A[i].m_weight >= 0){ // insert
                result &= add_edge(graph::WeightedEdge{A[i].m_source, A[i].m_destination, A[i].m_weight});
            } else { // remove
                result &= remove_edge(graph::Edge{A[i].m_source, A[i].m_destination});
            }
        }
    }

    return result;
}

template<typename Action, typename Edge>
void UpdateInterface::batch_try_again(Action action, Edge edge){
    constexpr chrono::seconds timeout = 10min;
    bool result = false;

    auto t_start = chrono::steady_clock::now();
    do {
        result = std::invoke(action, edge);
    } while(!result && chrono::steady_clock::now() - t_start <= timeout);

    if(!result){
        RAISE_EXCEPTION(TimeoutError, "Cannot process the edge " << edge << " after " << timeout.count() << " seconds");
    }
}

void UpdateInterface::load(const string& path) {
    auto reader = reader::Reader::open(path);
    ASSERT(reader->is_directed() == is_directed());
    gfe::graph::WeightedEdge edge;
    while(reader->read(edge)){
        add_vertex(edge.m_source);
        add_vertex(edge.m_destination);
        add_edge(edge);
    }
    build();
}

void UpdateInterface::build(){
    /* nop */
}

uint64_t UpdateInterface::num_levels() const {
    return 0; // by default, we assume that the implementation is not LSM/delta based, and it doesn`t create new levels/deltas/snapshots
}

} // namespace library
