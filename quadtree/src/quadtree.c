#include "quadtree.h"

// even the quad tree root is encapsulated
nodeaddr_t root = INVALIDADDR;
long numpoints = 0;

static double euclidean_dist(double x1, double y1, double x2, double y2);
static int cmpknn(const void* a, const void* b);
static void subdivide(nodeaddr_t ad);
static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr);
static nodeaddr_t quadtree_search_rec(nodekey_t key, nodeaddr_t curr);
static double min_distance_to_boundary(double x, double y, Boundary* boundary) ;
static bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist);
static void quadtree_k_nearest_rec(nodeaddr_t curr, long x, long y, long k, Heap* heap);


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
	numpoints++;
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

/*
nodeaddr_t quadtree_nearest(long x, long y)
{
	Stack* stack = stack_initialize(numpoints);
	stack_push(stack, root);
	
	QuadTreeNode aux;
	node_get(root, &aux);
	
	double min_dist = euclidean_dist(aux.boundary.x_min, aux.boundary.y_min, aux.boundary.x_max, aux.boundary.y_max);
	
	nodeaddr_t nearest = root;
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
			if (aux.key.x == -1) {
				continue;
			}
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
*/

static double min_distance_to_boundary(double x, double y, Boundary* boundary) {
    double dx = fmax(fmax(boundary->x_min - x, 0), x - boundary->x_max);
    double dy = fmax(fmax(boundary->y_min - y, 0), y - boundary->y_max);
    return sqrt(dx * dx + dy * dy);
}

static bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist) {
    double min_dist = min_distance_to_boundary(x, y, boundary);
    return min_dist < max_dist;
}

static void quadtree_k_nearest_rec(nodeaddr_t curr, long x, long y, long k, Heap* heap)
{	
	if (curr == INVALIDADDR) {
		return;
	}

	QuadTreeNode curr_node;
	node_get(curr, &curr_node);

	if (curr_node.key.x == -1) {
		return;
	}
	
	double dist = euclidean_dist(x, y, curr_node.key.x, curr_node.key.y);
	
	if (heap->size < k) {
		heap_push(heap, (Neighbor) {curr, dist});
	}
	else if (dist < heap->neighbors[0].dist) {
		heap_pop(heap);
		heap_push(heap, (Neighbor) {curr, dist});
	}

	QuadTreeNode aux;
	if (curr_node.nw != INVALIDADDR) {
		node_get(curr_node.nw, &aux);
		if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
			quadtree_k_nearest_rec(curr_node.nw, x, y, k, heap);
		}
	}
	if (curr_node.ne != INVALIDADDR) {
		node_get(curr_node.ne, &aux);
		if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
			quadtree_k_nearest_rec(curr_node.ne, x, y, k, heap);
		}
	}
	if (curr_node.sw != INVALIDADDR) {
		node_get(curr_node.sw, &aux);
		if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
			quadtree_k_nearest_rec(curr_node.sw, x, y, k, heap);
		}
	}
	if (curr_node.se != INVALIDADDR) {
		node_get(curr_node.se, &aux);
		if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
			quadtree_k_nearest_rec(curr_node.se, x, y, k, heap);
		}
	}
}

static int cmpknn(const void* a, const void* b) {
	Neighbor* k1 = (Neighbor*) a;
	Neighbor* k2 = (Neighbor*) b;
	if (k1->dist > k2->dist) return 1;
	else if (k1->dist < k2->dist) return -1;
	else return 0;
}

void quadtree_k_nearest(long x, long y, long k, long* result)
{
	// check whether the tree is null
	if (root == INVALIDADDR) {
		fprintf(stderr,"quadtree_search: tree empty\n");
		return;
	}
	Heap* heap = heap_initialize(k);
	// call the recursive function
	quadtree_k_nearest_rec(root, x, y, k, heap);

	qsort(heap->neighbors, k, sizeof(Neighbor), cmpknn);
	
	for (int i = 0; i < k; i++) {
		result[i] = heap->neighbors[i].addr;
	}
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
        fprintf(stderr, "export_quadtree: could not open file for writing\n");
        return;
    }

    // Start exporting from the root
    export_node(root, file);

    fclose(file);
}