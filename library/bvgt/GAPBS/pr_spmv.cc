// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include "pr_spmv.h"

/*
GAP Benchmark Suite
Kernel: PageRank (PR)
Author: Scott Beamer

Will return pagerank scores for all vertices once total change < epsilon

This legacy PR implementation uses the traditional iterative approach. This is
done to ease comparisons to other implementations (often use same algorithm),
but it is not necesarily the fastest way to implement it. It performs each
iteration as a sparse-matrix vector multiply (SpMV), and values are not visible
until the next iteration (like Jacobi-style method).
*/

pvector<ScoreT> PageRankPull(SpruceTransVer& g, int max_iters,
                             uint32_t num_nodes, double epsilon) {

  const ScoreT init_score = 1.0f / /*g.num_nodes()*/num_nodes;
  const ScoreT base_score = (1.0f - kDamp) / /*g.num_nodes()*/num_nodes;
  pvector<ScoreT> scores(/*g.num_nodes()*/num_nodes, init_score);
  pvector<ScoreT> outgoing_contrib(num_nodes, 0.0);
  double middle, start, end; // time analysis
//  start = omp_get_wtime();
  for (int iter=0; iter < max_iters; iter++) {
      double error = 0;
      double dangling_sum = 0.0;


#pragma omp parallel for reduction(+:dangling_sum)
      for (NodeID n = 0; n < num_nodes; n++) {
          uint32_t out_degree = SpruceTransVer::GetDegree(g, n);
          if (out_degree == 0) {
              dangling_sum += scores[n];
          }
          else {
              outgoing_contrib[n] = scores[n] / out_degree;
          }
      }

//      middle = omp_get_wtime();

      dangling_sum /= num_nodes;
    #pragma omp parallel for /*reduction(+ : error)*/ schedule(guided/*dynamic, 64*/)
    for (NodeID u=0; u < num_nodes; u++) {
      ScoreT incoming_total = 0;
        std::vector<SpruceTransVer::WeightedOutEdgeSimple> neighbours;
//        uint32_t f = (uint32_t)u;
        SpruceTransVer::get_neighbours(g, u, neighbours);
      for (auto e : neighbours) {
        auto v = (NodeID)e.des;
        incoming_total += outgoing_contrib[v];
      }
//      ScoreT old_score = scores[u];
      scores[u] = base_score + kDamp * (incoming_total + dangling_sum);
//      error += fabs(scores[u] - old_score);
    }
//    printf(" %2d    %lf\n", iter, error);
//    if (error < epsilon)
//      break;
  }
//  end = omp_get_wtime();
//  printf("Time to get degree: %lf s \n Time total: %lf s \n", middle - start, end - start);
  return scores;
}