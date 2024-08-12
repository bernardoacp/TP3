#include "quadtree.h"

// even the quad tree root is encapsulated
nodeaddr_t root = INVALIDADDR;
long numnodes = 0;

static void subdivide(nodeaddr_t ad);
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

static void subdivide(nodeaddr_t ad)
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

	// Push down the point from the parent node to one of the children
    //nodekey_t key = curr.key;
    //curr.key = INVALIDKEY;
    node_put(ad, &curr);

    // Debugging statements to check the state of the nodes
    // printf("Subdivided node at address %ld\n", ad);
    // printf("NW child at address %ld\n", curr.nw);
    // printf("NE child at address %ld\n", curr.ne);
    // printf("SW child at address %ld\n", curr.sw);
    // printf("SE child at address %ld\n", curr.se);

    // Reinsert the point into the appropriate child node
    //quadtree_insert_rec(key, ad);
}

static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr)
{
	QuadTreeNode curr_node;
	node_get(curr, &curr_node);

	if (!boundary_contains(&curr_node.boundary, key.x, key.y)) {
		return;
	}

	if (curr_node.key.x == -1 && curr_node.nw == INVALIDADDR) {
		curr_node.key = key;
		node_put(curr, &curr_node);
		return;
	}

	if (curr_node.nw == INVALIDADDR) {
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

	if (curr_node.nw == INVALIDADDR) {
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

// calculates Euclidean distance between (x1,y1) and (x2,y2)
static double euclidean_dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0); 
}

static bool check_overlap(nodeaddr_t addr, Boundary* bd)
{
	QuadTreeNode node;
	node_get(addr, &node);

	if (node.boundary.x_min > bd->x_max || node.boundary.x_max < bd->x_min || node.boundary.y_min > bd->y_max || node.boundary.y_max < bd->y_min) {
		return false;
	}
	return true;
}

// Function to recursively search for the nearest neighbor
void quadtree_nearest(nodeaddr_t* knearest, long size, long x, long y)
{
	Stack* stack = stack_initialize(1000);
	stack_push(stack, root);
	
	QuadTreeNode aux;
	node_get(root, &aux);
	
	double min_dist = euclidean_dist(aux.boundary.x_min, aux.boundary.y_min, aux.boundary.x_max, aux.boundary.y_max);
	
	nodeaddr_t nearest = INVALIDADDR;
	while (stack->size) {
		nodeaddr_t curr = stack_pop(stack);
		node_get(curr, &aux);
		if (aux.nw != INVALIDADDR && check_overlap(aux.nw, &(Boundary) {x - min_dist, x + min_dist, y - min_dist, y + min_dist})) {
			stack_push(stack, aux.nw);
		}
		if (aux.ne != INVALIDADDR && check_overlap(aux.ne, &(Boundary) {x - min_dist, x + min_dist, y - min_dist, y + min_dist})) {
			stack_push(stack, aux.ne);
		}
		if (aux.sw != INVALIDADDR && check_overlap(aux.sw, &(Boundary) {x - min_dist, x + min_dist, y - min_dist, y + min_dist})) {
			stack_push(stack, aux.sw);
		}
		if (aux.se != INVALIDADDR && check_overlap(aux.se, &(Boundary) {x - min_dist, x + min_dist, y - min_dist, y + min_dist})) {
			stack_push(stack, aux.se);
		}
		if (curr != INVALIDADDR) {
			double dist = euclidean_dist(x, y, aux.key.x, aux.key.y);
			if (dist < min_dist) {
				min_dist = dist;
				nearest = curr;
			}
		}
	}
	stack_destroy(stack);
	return nearest;
}

nodeaddr_t* quadtree_knearest(long x, long y, long k)
{
	// to be implemented
	nodeaddr_t* knearest = (nodeaddr_t*) malloc(k * sizeof(nodeaddr_t));
	if (knearest == NULL) {
		fprintf(stderr, "quadtree_knearest: could not allocate nearest\n");
		return NULL;
	}
	long size = 0;
	while (size < k) {
		quadtree_nearest(knearest, size, x, y);
		size++;
	}
	return NULL;
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