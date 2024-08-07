#ifndef BINTREE_H
#define BINTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "node.h" 

void bintree_create(long numnodes);
nodeaddr_t bintree_search(nodekey_t k, ptr_node_t pn);
nodeaddr_t bintree_insert(nodekey_t k, ptr_node_t pn);
void bintree_remove(nodekey_t k, ptr_node_t pn);
void bintree_dump();
void bintree_destroy();


#endif
