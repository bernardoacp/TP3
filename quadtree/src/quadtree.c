#include "../include/quadtree.h"

// even the quad tree root is encapsulated
nodeaddr_t root = INVALIDADDR;

// private functions
// nodeaddr_t quadtree_pred_rec(nodeaddr_t curr, QuadTreeNode* pn, QuadTreeNode* ppn, nodeaddr_t* ppa);
// nodeaddr_t quadtree_pred(nodeaddr_t curr, QuadTreeNode* pn, QuadTreeNode* ppn, nodeaddr_t* ppa);
nodeaddr_t quadtree_search_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p);
nodeaddr_t quadtree_insert_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p);
// int quadtree_remove_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p, nodeaddr_t* pna);

// Create a binary tree with at most numnodes nodes
void quadtree_create(long numnodes, Boundary qt_boundary) {
	// we should create and initialize the vector that will contain the tree
	node_initialize(numnodes, qt_boundary);
}

// Destroy a binary tree
void quadtree_destroy() {
	// we should first dealocate the vector that contains the tree
	node_destroy();
	// reset the tree root
	root = INVALIDADDR;
}

void subdivide(addr_t ad)
{
	
}