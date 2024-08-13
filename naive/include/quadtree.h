#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "boundary.h"
#include "qnode.h"
#include "heap.h"

#define INF 0x3f3f3f3f 

void quadtree_create(long numnodes, Boundary boundary);
void quadtree_destroy();
void quadtree_insert(nodekey_t k);
nodeaddr_t quadtree_search(char* idend, double x, double y);
void quadtree_k_nearest(double x, double y, long k, Neighbor* result);
void export_quadtree(const char* filename);
void export_node(nodeaddr_t addr, FILE* file);

#endif