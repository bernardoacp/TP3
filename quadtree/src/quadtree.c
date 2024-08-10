#include "quadtree.h"

// even the quad tree root is encapsulated
nodeaddr_t root = INVALIDADDR;

static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr);
static nodeaddr_t quadtree_search_rec(nodekey_t key, nodeaddr_t curr);

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

	//nodekey_t key = curr.key;
	//curr.key = INVALIDKEY;
	curr.subdivided = true;
	node_put(ad, &curr);

	//quadtree_insert_rec(key, ad);
}

static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr)
{
	QuadTreeNode curr_node;
	node_get(curr, &curr_node);

	if (!boundary_contains(&curr_node.boundary, key.x, key.y)) {
		return;
	}

	if (curr_node.key.x == -1 && !curr_node.subdivided) {
		curr_node.key = key;
		node_put(curr, &curr_node);
		return;
	}

	if (!curr_node.subdivided) {
		subdivide(curr);
		node_get(curr, &curr_node);
	}

	quadtree_insert_rec(key, curr_node.nw);
	quadtree_insert_rec(key, curr_node.ne);
	quadtree_insert_rec(key, curr_node.sw);
	quadtree_insert_rec(key, curr_node.se);
}

void quadtree_insert(nodekey_t key)
{
	QuadTreeNode aux;
	node_reset(&aux);
	aux.key = key;
	if (root == INVALIDADDR) {
		root = node_create(&aux);
		node_put(root, &aux);
		return;
	}
	quadtree_insert_rec(key, root);
}

static nodeaddr_t quadtree_search_rec(nodekey_t key, nodeaddr_t curr)
{
	QuadTreeNode curr_node;
	node_get(curr, &curr_node);

	if (curr_node.key.x == key.x && curr_node.key.y == key.y) {
		return curr;
	}

	if (!curr_node.subdivided) {
		return -1;
	}

	QuadTreeNode aux;
	node_get(curr_node.nw, &aux);
	if (boundary_contains(&aux.boundary, key.x, key.y)) {
		return quadtree_search_rec(key, curr_node.nw);
	}

	node_get(curr_node.ne, &aux);
	if (boundary_contains(&aux.boundary, key.x, key.y)) {
		return quadtree_search_rec(key, curr_node.ne);
	}

	node_get(curr_node.sw, &aux);
	if (boundary_contains(&aux.boundary, key.x, key.y)) {
		return quadtree_search_rec(key, curr_node.sw);
	}

	node_get(curr_node.se, &aux);
	if (boundary_contains(&aux.boundary, key.x, key.y)) {
		return quadtree_search_rec(key, curr_node.se);
	}
}

nodeaddr_t quadtree_search(nodekey_t key)
{
	// check whether the tree is null
	if (root == INVALIDADDR) {
		fprintf(stderr,"quadtree_search: tree empty\n");
		return INVALIDADDR;
	}
	// call the recursive function
	return quadtree_search_rec(key, root);
}

// Function to recursively export the QuadTree nodes
void export_node(nodeaddr_t addr, FILE* file) {
    if (addr == INVALIDADDR) return;

    QuadTreeNode node;
    node_get(addr, &node);

    // Write the boundary of the current node
    fprintf(file, "%f %f %f %f\n", node.boundary.x_min, node.boundary.x_max, node.boundary.y_min, node.boundary.y_max);

    // Recursively export child nodes
    export_node(node.nw, file);
    export_node(node.ne, file);
    export_node(node.sw, file);
    export_node(node.se, file);
}

// Function to export the QuadTree data to a file
void export_quadtree(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Could not open file for writing\n");
        return;
    }

    // Start exporting from the root
    export_node(root, file);

    fclose(file);
}