// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "bfs.h"
#include "time.h"

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

int64_t BUStep(SpruceTransVer& g, pvector<NodeID> &parent, Bitmap &front,
               Bitmap &next, int vertex_num) {
    int64_t awake_count = 0;
    next.reset();
#pragma omp parallel for reduction(+ : awake_count) schedule(dynamic, 1024)
    for (NodeID u=0; u < vertex_num; u++) {
        if (parent[u] == -1) {
            std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
            SpruceTransVer::get_neighbours(g, u, neighbours);
            for (auto e : neighbours) {
                auto v = (NodeID)e.des;
                if (front.get_bit(v)) {
                    parent[u] = v;
                    awake_count++;
                    next.set_bit(u);
                    break;
                }
            }
        }
    }
    return awake_count;
}


int64_t TDStep(SpruceTransVer& g, pvector<NodeID> &parent,
               SlidingQueue<NodeID> &queue) {
    int64_t scout_count = 0;
    double time_out_start_time, time_out_stop_time, time_thread_start_time, time_thread_stop_time;
    long int thread_times[64] = {0};
#pragma omp parallel
    {
        QueueBuffer<NodeID> lqueue(queue);

        //time analysis

        time_out_start_time = omp_get_wtime();

#pragma omp for reduction(+ : scout_count) nowait schedule(dynamic)
        for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
            // time analysis
            struct timespec start_in, stop_in;
            int thread_id = omp_get_thread_num();
//            time_thread_start_time = omp_get_wtime();
            clock_gettime(CLOCK_MONOTONIC, &start_in);


            NodeID u = *q_iter;
            uint32_t from_node_id = (uint32_t)u;
            std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
            SpruceTransVer::get_neighbours(g, u, neighbours);
            for (auto e : neighbours) {
                auto v = (NodeID)e.des;
                NodeID curr_val = parent[v];
                if (curr_val /*< 0*/ == -1) {
//                    if (v.des > 16777217) {
//                        std::cout << "max vertex num should be " << v.des << std::endl;
//                    }
                    if (compare_and_swap(parent[v], curr_val, u)) {
                        lqueue.push_back(v);
                        scout_count += SpruceTransVer::GetDegree(g, v);
                    }
                }
            }
            // time analysis
            clock_gettime(CLOCK_MONOTONIC, &stop_in);
//            time_thread_stop_time = omp_get_wtime();
            thread_times[thread_id] += (stop_in.tv_sec - start_in.tv_sec)*1000000000 + stop_in.tv_nsec - start_in.tv_nsec;
//            printf("I'm here.");
        }

        lqueue.flush();

        // time analysis

#pragma omp barrier
#pragma omp master
        {
            time_out_stop_time = omp_get_wtime();
//            printf("Running time for the level: %lf s\n", time_out_stop_time - time_out_start_time);
//            for (int i = 0; i < 64; i++) {
//                printf("Running time for thread %ld : %ld ns\n", i, thread_times[i]);
//            }
            // write_array_to_file("bfs_thread_time.txt", thread_times, 56);
        }
    }
    return scout_count;
}


void QueueToBitmap(const SlidingQueue<NodeID> &queue, Bitmap &bm) {
#pragma omp parallel for
    for (auto q_iter = queue.begin(); q_iter < queue.end(); q_iter++) {
        NodeID u = *q_iter;
        bm.set_bit_atomic(u);
    }
}

void BitmapToQueue(SpruceTransVer& g, const Bitmap &bm,
                   SlidingQueue<NodeID> &queue, int vertex_num) {
#pragma omp parallel
    {
        QueueBuffer<NodeID> lqueue(queue);
#pragma omp for nowait
        for (NodeID n=0; n < vertex_num; n++)
            if (bm.get_bit(n))
                lqueue.push_back(n);
        lqueue.flush();
    }
    queue.slide_window();
}

pvector<NodeID> InitParent(int vertex_num) {
    pvector<NodeID> parent(vertex_num);
#pragma omp parallel for
    for (NodeID n=0; n < vertex_num; n++)
        parent[n] = /*g.out_degree(n) != 0 ? -g.out_degree(n) : -1;*/ -1;
    return parent;
}

pvector<NodeID> DOBFS(SpruceTransVer& g/*const Graph &g*/, NodeID source, int vertex_num, int edge_num, int src_out_degree, int alpha,
                      int beta) {
//    PrintStep("Source", static_cast<int64_t>(source));
    Timer t;
//    t.Start();
    pvector<NodeID> parent = InitParent(vertex_num);
//    t.Stop();
//    PrintStep("i", t.Seconds());
    parent[source] = source;

    SlidingQueue<NodeID> queue(vertex_num);
    queue.push_back(source);
    queue.slide_window();
    Bitmap curr(vertex_num);
    curr.reset();
    Bitmap front(vertex_num);
    front.reset();
    int64_t edges_to_check = edge_num;
    int64_t scout_count = src_out_degree;
    while (!queue.empty()) {
        //edit for only top-down search
        if (/*0*/scout_count > edges_to_check / alpha) {
      int64_t awake_count, old_awake_count;
      TIME_OP(t, QueueToBitmap(queue, front));
    //   PrintStep("e", t.Seconds());
      awake_count = queue.size();
      queue.slide_window();
      do {
        t.Start();
        old_awake_count = awake_count;
        awake_count = BUStep(g, parent, front, curr, vertex_num);
        front.swap(curr);
        t.Stop();
        // PrintStep("bu", t.Seconds(), awake_count);
      } while ((awake_count >= old_awake_count) ||
               (awake_count > vertex_num / beta));
      TIME_OP(t, BitmapToQueue(g, front, queue, vertex_num));
    //   PrintStep("c", t.Seconds());
      scout_count = 1;
        } else {
            t.Start();
            edges_to_check -= scout_count;
            scout_count = TDStep(g, parent, queue);
            queue.slide_window();
            t.Stop();
            // PrintStep("td", t.Seconds(), queue.size());
//            std::cout << queue.size() << std::endl;
        }
    }
//#pragma omp parallel for
//    for (NodeID n = 0; n < vertex_num; n++)
//        if (parent[n] < -1)
//            parent[n] = -1;
    return parent;
}