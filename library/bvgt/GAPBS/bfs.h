//
// Created by sjf on 8/22/2022.
//

#ifndef GRAPHINDEX_BFS_H
#define GRAPHINDEX_BFS_H

#include "benchmark.h"
#include "bitmap.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "sliding_queue.h"
#include "timer.h"
#include "../Spruce/src/index_algorithms.h"
#include "../Spruce/src/spruce_transaction.h"

extern int64_t BUStep(SpruceTransVer& g, pvector<NodeID> &parent, Bitmap &front,
               Bitmap &next, int vertex_num);
extern int64_t TDStep(SpruceTransVer& g, pvector<NodeID> &parent,
               SlidingQueue<NodeID> &queue);
extern void QueueToBitmap(const SlidingQueue<NodeID> &queue, Bitmap &bm);
extern void BitmapToQueue(SpruceTransVer& g, const Bitmap &bm,
                  SlidingQueue<NodeID> &queue, int vertex_num);
extern pvector<NodeID> InitParent(int vertex_num);
extern pvector<NodeID> DOBFS(SpruceTransVer& g/*const Graph &g*/, NodeID source, int vertex_num, int edge_num, int src_out_degree, int alpha = 15,
                      int beta = 18);

#endif //GRAPHINDEX_BFS_H
