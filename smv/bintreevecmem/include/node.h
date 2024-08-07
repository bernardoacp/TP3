// node.h
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
// 
// Vectorized Binary Tree Node ADT Header file 

#ifndef NODE_H
#define NODE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define INVALIDKEY -1
#define INVALIDADDR -2

// there are no pointers in a vectorized implementation, just vector indices
typedef long nodekey_t;
typedef long nodeaddr_t;

// classical binary tree
typedef struct {
  nodekey_t key;
  nodeaddr_t left;
  nodeaddr_t right;
} node_t, *ptr_node_t;

long node_initialize(long numnodes);
nodeaddr_t node_create(ptr_node_t pn);
void node_delete(nodeaddr_t ad);
void node_get(nodeaddr_t ad, ptr_node_t pn);
void node_put(nodeaddr_t ad, ptr_node_t pn);
void node_dump(int ad, int level);
void node_dumpvet();
void node_reset(ptr_node_t pn);
void node_copy(ptr_node_t dst, ptr_node_t src);
void node_destroy();

#endif
