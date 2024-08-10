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

void subdivide(nodeaddr_t ad)
{
	QuadTreeNode curr;
	node_get(ad, &curr);

	double x_min = curr.boundary.x_min;
	double x_max = curr.boundary.x_max;
	double y_min = curr.boundary.y_min;
	double y_max = curr.boundary.y_max;

	Boundary nw = (Boundary) {x_min, (x_min+x_max)/2, (y_min+y_max)/2, y_max};
	Boundary ne = (Boundary) {(x_min+x_max)/2, x_max, (y_min+y_max)/2, y_max};
	Boundary sw = (Boundary) {x_min, (x_min+x_max)/2, y_min, (y_min+y_max)/2};
	Boundary se = (Boundary) {(x_min+x_max)/2, x_max, y_min, (y_min+y_max)/2};

	QuadTreeNode aux;
	node_reset(&aux);
	aux.boundary = nw;
	curr.nw = node_create(&aux);

	node_reset(&aux);
	aux.boundary = ne;
	curr.ne = node_create(&aux);

	node_reset(&aux);
	aux.boundary = sw;
	curr.sw = node_create(&aux);

	node_reset(&aux);
	aux.boundary = se;
	curr.se = node_create(&aux);

	node_put(ad, &curr);
}