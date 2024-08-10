#ifndef QNODE_H
#define QNODE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "boundary.h"

// struct that contains the information about the recharging locations
typedef struct {
	double x;
	double y;
} Item;

#define INVALIDKEY (Item) {-1, -1}
#define INVALIDBOUNDARY (Boundary) {0, 0, 0, 0}
#define INVALIDADDR -2

// there are no pointers in a vectorized implementation, just vector indices
typedef Item nodekey_t;
typedef long nodeaddr_t;

typedef struct {
    Boundary boundary;
    nodekey_t key;
    nodeaddr_t nw;
    nodeaddr_t ne;
    nodeaddr_t sw;
    nodeaddr_t se;
    bool subdivided;
} QuadTreeNode;

long node_initialize(long numnodes, Boundary qt_boundary);
nodeaddr_t node_create(QuadTreeNode* pn);
void node_delete(nodeaddr_t ad);
void node_get(nodeaddr_t ad, QuadTreeNode* pn);
void node_put(nodeaddr_t ad, QuadTreeNode* pn);
void node_dump(int ad, int level);
void node_dumpvet();
void node_reset(QuadTreeNode* pn);
void node_copy(QuadTreeNode* dst, QuadTreeNode* src);
void node_destroy();

#endif