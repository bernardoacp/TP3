#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "qnode.h"

void quadtree_create(long numnodes, Boundary boundary);
nodeaddr_t quadtree_search(nodekey_t k, QuadTreeNode* pn);
nodeaddr_t quadtree_insert(nodekey_t k, QuadTreeNode* pn);
void quadtree_subdivide(nodeaddr_t ad);
void quadtree_remove(nodekey_t k, QuadTreeNode* pn);
void quadtree_dump();
void quadtree_destroy();

#endif