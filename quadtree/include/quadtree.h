#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "boundary.h"
#include "qnode.h"

void quadtree_create(long numnodes, Boundary boundary);
nodeaddr_t quadtree_search(nodekey_t k, QuadTreeNode* pn);
void quadtree_insert(nodekey_t k);
void quadtree_subdivide(nodeaddr_t ad);
void quadtree_remove(nodekey_t k, QuadTreeNode* pn);
void quadtree_dump();
void quadtree_destroy();
void export_quadtree(const char* filename);
void export_node(nodeaddr_t addr, FILE* file);

#endif