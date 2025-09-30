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

#include "insert_only.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

#include "common/database.hpp"
#include "common/system.hpp"
#include "common/timer.hpp"
#include "details/build_thread.hpp"
#include "configuration.hpp"
#include "library/interface.hpp"
//#include "../../third-party/libcommon/src/system_introspection.hpp"

using namespace common;
using namespace gfe::experiment::details;
using namespace std;

namespace gfe::experiment {

InsertOnly::InsertOnly(std::shared_ptr<gfe::library::UpdateInterface> interface, std::shared_ptr<gfe::graph::WeightedEdgeStream> graph, int64_t num_threads) :
        m_interface(interface), m_stream(graph), m_num_threads(num_threads) {
    if(m_num_threads == 0) ERROR("Invalid number of threads: " << m_num_threads);
}

void InsertOnly::set_scheduler_granularity(uint64_t granularity) {
    if(granularity == 0) INVALID_ARGUMENT("The granularity of a chunk size cannot be zero");
    m_scheduler_granularity = granularity;
}

void InsertOnly::set_build_frequency(std::chrono::milliseconds millisecs){
    m_build_frequency = millisecs;
}

// Execute an update at the time
static void run_sequential_insert_edge(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        auto edge = graph->get(pos);
        [[maybe_unused]] bool result = interface->add_edge_v2(edge);
    }
}

static void run_sequential_insert_vertex(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        auto edge = graph->get(pos);
        [[maybe_unused]] bool result = interface->add_vertex(edge.m_source);
        #if defined(HAVE_BVGT)
            // BVGT does not support only inserting a vertex
            result = interface->add_edge(edge);
        #endif
    }
}

static void run_sequential_query_vertex(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        auto edge = graph->get(pos);
        [[maybe_unused]] bool result = interface->has_vertex(edge.m_source);
        #if defined(HAVE_BVGT)
            // BVGT does not support only querying a vertex
            result = interface->get_neighbours(edge.m_source);
        #endif
    }
}

static void run_sequential_get_neighbors(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        [[maybe_unused]] bool result = interface->get_neighbors(pos);
    }
}

static void run_sequential_get_two_hop_neighbors(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        [[maybe_unused]] bool result = interface->get_two_hop_neighbors(pos);
    }
}

static void run_sequential_delete_edge(library::UpdateInterface* interface, graph::WeightedEdgeStream* graph, uint64_t start, uint64_t end){
    for(uint64_t pos = start; pos < end; pos++){
        auto edge = graph->get(pos);
        [[maybe_unused]] bool result = interface->remove_edge(edge);
    }
}

void InsertOnly::execute_round_robin(){
    vector<thread> threads;
    #if HAVE_GTX
        m_interface.get()->set_worker_thread_num(m_num_threads);
    #endif
    atomic<uint64_t> start_chunk_next = 0;
    atomic<int> tempp = m_stream.get()->num_edges()/100;
    #if defined(HAVE_BVGT)
        m_interface.get()->add_vertex(m_stream.get()->max_vertex_id() + 1);
    #endif
    #if defined(HAVE_RG)
        m_interface.get()->set_max_vertex_id(m_stream.get()->max_vertex_id());
        if (configuration().is_insert_vertex_only()) {
            m_interface.get()->add_vertex(m_stream.get()->max_vertex_id());
        }
    #endif
    for(int64_t i = 0; i < m_num_threads; i++){
        threads.emplace_back([this, &start_chunk_next, &tempp](int thread_id){
            concurrency::set_thread_name("Worker #" + to_string(thread_id));
            int64_t memllb = common::get_memory_footprint();
            auto interface = m_interface.get();
            auto graph = m_stream.get();
            uint64_t start;
            const uint64_t size = graph->num_edges();

            interface->on_thread_init(thread_id);
            while( (start = start_chunk_next.fetch_add(m_scheduler_granularity)) < size ){
                uint64_t end = std::min<uint64_t>(start + m_scheduler_granularity, size);
                if (configuration().is_insert_vertex_only()) {
                    run_sequential_insert_vertex(interface, graph, start, end);
                }
                else {
                    run_sequential_insert_edge(interface, graph, start, end);
                }
            }

            interface->on_thread_destroy(thread_id);

        }, static_cast<int>(i));
    }

    // wait for all threads to complete
    for(auto& t : threads) t.join();
}

void InsertOnly::execute_round_robin_get_two_hop_neighbors(){
    vector<thread> threads;
    #if HAVE_GTX
        m_interface.get()->set_worker_thread_num(m_num_threads);
    #endif
    atomic<uint64_t> start_chunk_next = 0;


    for(int64_t i = 0; i < m_num_threads; i++){
        threads.emplace_back([this, &start_chunk_next](int thread_id){
            concurrency::set_thread_name("Worker #" + to_string(thread_id));

            auto interface = m_interface.get();
            auto graph = m_stream.get();
            uint64_t start;
            const uint64_t size = graph->max_vertex_id() + 1;

            interface->on_thread_init(thread_id);

            while( (start = start_chunk_next.fetch_add(m_scheduler_granularity)) < size ){
                uint64_t end = std::min<uint64_t>(start + m_scheduler_granularity, size);
                run_sequential_get_two_hop_neighbors(interface, graph, start, end);
            }

            interface->on_thread_destroy(thread_id);

        }, static_cast<int>(i));
    }

    // wait for all threads to complete
    for(auto& t : threads) t.join();
}

void InsertOnly::execute_round_robin_get_neighbors(){
    vector<thread> threads;
    #if HAVE_GTX
        m_interface.get()->set_worker_thread_num(m_num_threads);
    #endif
    atomic<uint64_t> start_chunk_next = 0;


    for(int64_t i = 0; i < m_num_threads; i++){
        threads.emplace_back([this, &start_chunk_next](int thread_id){
            concurrency::set_thread_name("Worker #" + to_string(thread_id));

            auto interface = m_interface.get();
            auto graph = m_stream.get();
            uint64_t start;
            const uint64_t size = graph->max_vertex_id() + 1;

            interface->on_thread_init(thread_id);

            while( (start = start_chunk_next.fetch_add(m_scheduler_granularity)) < size ){
                uint64_t end = std::min<uint64_t>(start + m_scheduler_granularity, size);
                run_sequential_get_neighbors(interface, graph, start, end);
            }

            interface->on_thread_destroy(thread_id);

        }, static_cast<int>(i));
    }

    // wait for all threads to complete
    for(auto& t : threads) t.join();
}

void InsertOnly::execute_round_robin_query(){
    vector<thread> threads;
    #if HAVE_GTX
        m_interface.get()->set_worker_thread_num(m_num_threads);
    #endif
    atomic<uint64_t> start_chunk_next = 0;
    atomic<int> tempp = m_stream.get()->num_edges()/100;
    for(int64_t i = 0; i < m_num_threads; i++){
        threads.emplace_back([this, &start_chunk_next, &tempp](int thread_id){
            concurrency::set_thread_name("Worker #" + to_string(thread_id));
            int64_t memllb = common::get_memory_footprint();
            auto interface = m_interface.get();
            auto graph = m_stream.get();
            uint64_t start;
            const uint64_t size = graph->num_edges();

            interface->on_thread_init(thread_id);
            while( (start = start_chunk_next.fetch_add(m_scheduler_granularity)) < size ){
                uint64_t end = std::min<uint64_t>(start + m_scheduler_granularity, size);
                run_sequential_query_vertex(interface, graph, start, end);
            }

            interface->on_thread_destroy(thread_id);

        }, static_cast<int>(i));
    }

    // wait for all threads to complete
    for(auto& t : threads) t.join();
}

void InsertOnly::execute_round_robin_delete(){
    vector<thread> threads;
    #if HAVE_GTX
        m_interface.get()->set_worker_thread_num(m_num_threads);
    #endif
    atomic<uint64_t> start_chunk_next = 0;


    for(int64_t i = 0; i < m_num_threads; i++){
        threads.emplace_back([this, &start_chunk_next](int thread_id){
            concurrency::set_thread_name("Worker #" + to_string(thread_id));

            auto interface = m_interface.get();
            auto graph = m_stream.get();
            uint64_t start;
            const uint64_t size = graph->num_edges();

            interface->on_thread_init(thread_id);

            while( (start = start_chunk_next.fetch_add(m_scheduler_granularity)) < size ){
                uint64_t end = std::min<uint64_t>(start + m_scheduler_granularity, size);
                run_sequential_delete_edge(interface, graph, start, end);
            }

            interface->on_thread_destroy(thread_id);

        }, static_cast<int>(i));
    }

    // wait for all threads to complete
    for(auto& t : threads) t.join();
}

chrono::microseconds InsertOnly::execute() {
    // re-adjust the scheduler granularity if there are too few insertions to perform
    if(m_stream->num_edges() / m_num_threads < m_scheduler_granularity){
        m_scheduler_granularity = m_stream->num_edges() / m_num_threads;
        if(m_scheduler_granularity == 0) m_scheduler_granularity = 1; // corner case
        LOG("InsertOnly: reset the scheduler granularity to " << m_scheduler_granularity << " edge insertions per thread");
    }

    auto before = common::get_memory_footprint();
    LOG("Memory before: " <<  before << "MB");
    // Execute the insertions
    m_interface->on_main_init(m_num_threads /* build thread */ +1);
    m_interface->updates_start();
    Timer timer;
    timer.start();
    BuildThread build_service { m_interface , static_cast<int>(m_num_threads), m_build_frequency };
    execute_round_robin();
    build_service.stop();
    timer.stop();
    auto after = common::get_memory_footprint();
    LOG("Memory consumption: " << after - before << "MB");
    m_interface->updates_stop();
    LOG("Insertions performed with " << m_num_threads << " threads in " << timer);

    #if HAVE_RG
        m_interface->print_stats();
    #endif
    m_time_insert = timer.microseconds();
    m_num_build_invocations = build_service.num_invocations();

    // A final invocation of the method #build()
    m_interface->on_thread_init(0);
    timer.start();
    m_interface->build();
    timer.stop();
    m_time_build = timer.microseconds();
    if(m_time_build > 0){
        LOG("Build time: " << timer);
    }
    m_num_build_invocations++;

    if (configuration().is_insert_vertex_only()) {
        // Query vertex in round robin
        if((m_stream->max_vertex_id() + 1) / m_num_threads < m_scheduler_granularity){
            m_scheduler_granularity = (m_stream->max_vertex_id() + 1) / m_num_threads;
            if(m_scheduler_granularity == 0) m_scheduler_granularity = 1; // corner case
            LOG("InsertOnly: reset the scheduler granularity to " << m_scheduler_granularity << " vertex queries per thread");
        }
        timer.start();
        BuildThread build_service2 { m_interface , static_cast<int>(m_num_threads), m_build_frequency };
        execute_round_robin_query();
        build_service2.stop();
        timer.stop();
        LOG("Vertex queries performed with " << m_num_threads << " threads in " << timer);
    }
    else {
        LOG("Edge stream size: " << m_stream->num_edges() << ", num edges stored in the graph: " << m_interface->num_edges() << ", match: " << (m_stream->num_edges() == m_interface->num_edges() ? "yes" : "no"));
    }

    m_interface->on_thread_destroy(0);
    m_interface->on_main_destroy();

    //////////////////////////////////////
    // Get-neighbor and 2-hop neighbors here (When running graph analytics)
    if (configuration().num_repetitions() != 0) {
        if((m_stream->max_vertex_id() + 1) / m_num_threads < m_scheduler_granularity){
            m_scheduler_granularity = (m_stream->max_vertex_id() + 1) / m_num_threads;
            if(m_scheduler_granularity == 0) m_scheduler_granularity = 1; // corner case
            LOG("InsertOnly: reset the scheduler granularity to " << m_scheduler_granularity << " neighbour queries per thread");
        }

        timer.start();
        BuildThread build_service2 { m_interface , static_cast<int>(m_num_threads), m_build_frequency };
        execute_round_robin_get_neighbors();
        build_service2.stop();
        timer.stop();
        LOG("Get 1-hop neighbors performed with " << m_num_threads << " threads in " << timer);

        timer.start();
        BuildThread build_service3 { m_interface , static_cast<int>(m_num_threads), m_build_frequency };
        execute_round_robin_get_two_hop_neighbors();
        build_service3.stop();
        timer.stop();
        LOG("Get 2-hop neighbors performed with " << m_num_threads << " threads in " << timer);
    }

    //////////////////////////////////////
    // Delete here (When not running graph analytics and vertex insertion only)
    if (configuration().num_repetitions() == 0 && !configuration().is_insert_vertex_only()) {
        if(m_stream->num_edges() / m_num_threads < m_scheduler_granularity){
            m_scheduler_granularity = m_stream->num_edges() / m_num_threads;
            if(m_scheduler_granularity == 0) m_scheduler_granularity = 1; // corner case
            LOG("InsertOnly: reset the scheduler granularity to " << m_scheduler_granularity << " edge insertions per thread");
        }

        m_interface->updates_start();
        timer.start();
        BuildThread build_service2 { m_interface , static_cast<int>(m_num_threads), m_build_frequency };
        execute_round_robin_delete();
        build_service2.stop();
        timer.stop();
        m_interface->updates_stop();
        LOG("Deletions performed with " << m_num_threads << " threads in " << timer);
        m_time_insert = timer.microseconds();
        m_num_build_invocations = build_service2.num_invocations();

        m_interface->on_thread_init(0);
        timer.start();
        m_interface->build();
        timer.stop();
        m_time_build = timer.microseconds();
        if(m_time_build > 0){
            LOG("Build time: " << timer);
        }
        m_num_build_invocations++;

        LOG("Edge stream size: " << m_stream->num_edges() << ", num edges stored in the graph: " << m_interface->num_edges() << ", match: " << (0 == m_interface->num_edges() ? "yes" : "no"));
    
        // Delete finshed
        m_interface->on_thread_destroy(0);
        m_interface->on_main_destroy();
    }

    return chrono::microseconds{ m_time_insert + m_time_build };
}

void InsertOnly::save() {
    assert(configuration().db() != nullptr);
    auto db = configuration().db()->add("insert_only");
    db.add("scheduler", "round_robin"); // backwards compatibility
    db.add("scheduler_granularity", m_scheduler_granularity); // the number of insertions performed by each thread
    db.add("insertion_time", m_time_insert); // microseconds
    db.add("build_time", m_time_build); // microseconds
    db.add("num_edges", m_stream->num_edges());
    db.add("num_snapshots_created", m_interface->num_levels());
    db.add("num_build_invocations", m_num_build_invocations);
    // missing revision: until 25/Nov/2019
    // version 20191125: build thread, build frequency taken into account, scheduler set to round_robin, removed batch updates
    // version 20191210: difference between num_build_invocations (explicit invocations to #build()) and num_snapshots_created (actual number of deltas created by the impl)
    // version 20200625: rely on #add_edge_v2 to implicitly create the vertices. This should alleviate the footprint of the driver for non scalable implementations
    db.add("revision", "20200625");
}

} // namespace
