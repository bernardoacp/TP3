#include "qnode.h"

// encapsulated variables keeping the vector of nodes
QuadTreeNode* nodevet = NULL;
Boundary boundary = INVALIDBOUNDARY;
long nodevetsz = 0;
long nodesallocated = 0;
long firstavail = INVALIDADDR;

#define INVALIDNODE {boundary, INVALIDKEY, INVALIDADDR, INVALIDADDR, INVALIDADDR, INVALIDADDR}

static bool is_invalid_node(QuadTreeNode* node) {
    QuadTreeNode invalid_node = INVALIDNODE;
    return memcmp(node, &invalid_node, sizeof(QuadTreeNode)) == 0;
}

// reset node, removing any previous usage information
void node_reset(QuadTreeNode* pn) {
	pn->boundary = boundary;
	pn->key = INVALIDKEY;
	pn->ne = INVALIDADDR;
    pn->nw = INVALIDADDR;
    pn->se = INVALIDADDR;
    pn->sw = INVALIDADDR;
}

// copy contents of node src to node dst
void node_copy(QuadTreeNode* dst, QuadTreeNode* src) {
	dst->key = src->key;
    dst->boundary = src->boundary;
    dst->nw = src->nw;
    dst->ne = src->ne;
    dst->sw = src->sw;
    dst->se = src->se;
}

// initialize a vectorized binary tree that will contain at most numnodes
long node_initialize(long numnodes, Boundary qt_boundary) {
	// allocate node vet
	nodevet = (QuadTreeNode*) malloc(numnodes * sizeof(QuadTreeNode));
	if (nodevet == NULL) {
		fprintf(stderr,"node_initialize: could not allocate nodevet\n");
		return 0;
	}
	// initialize boundary
	boundary = qt_boundary;
	// initialize nodevetsz;
	nodevetsz = numnodes;
	// create the chain of available nodes as a linked list
	for (long i = 0; i < nodevetsz; i++) {
		node_reset(&(nodevet[i]));
		nodevet[i].nw = (nodeaddr_t) i + 1;
	}
	// last node in the chain
	nodevet[nodevetsz - 1].nw = INVALIDADDR;
	// node 0 is the first avail
	firstavail = (nodeaddr_t) 0;
	return numnodes;
}
/*
void reallocate_node_array() {
    int new_size = nodevetsz * 2;
    QuadTreeNode* new_nodevet = realloc(nodevet, new_size * sizeof(QuadTreeNode));
    if (new_nodevet == NULL) {
        fprintf(stderr, "reallocate_node_array: could not reallocate node array\n");
        exit(EXIT_FAILURE);
    }
    nodevet = new_nodevet;

    // Initialize the new nodes and add them to the available list
    for (int i = nodevetsz; i < new_size; i++) {
        node_reset(&(nodevet[i]));
		nodevet[i].nw = (nodeaddr_t) i + 1;
    }
	nodevet[new_size - 1].nw = INVALIDADDR;
	// node 0 is the first avail
	firstavail = (nodeaddr_t) nodevetsz;
    nodevetsz = new_size;
}
*/

// create node from pn
nodeaddr_t node_create(QuadTreeNode* pn) {
	nodeaddr_t ret;
	// check whether node vector is full
	//if (nodesallocated == nodevetsz) {
	//	reallocate_node_array();
	//}
	// remove a node from the avail chain, update the controls and copy pn to it
	ret = firstavail;
	firstavail = nodevet[ret].nw;
	nodesallocated++;
	node_copy(&(nodevet[ret]), pn);
	return ret;
}

// delete virtually node ad, making it available for futher creation
void node_delete(nodeaddr_t ad) {
	// check whether ad is a valid address
	if (ad < 0 || ad >= nodevetsz) {
		fprintf(stderr,"node_delete: address out of range\n");
		return;
	}
	if (is_invalid_node(&(nodevet[ad]))) {
		fprintf(stderr,"node_delete: node already deleted\n");
	}
	// just reset and add to the front of available list 
	node_reset(&(nodevet[ad]));
	nodevet[ad].nw = firstavail;
	firstavail = ad;
	nodesallocated--;
}

// retrieve a node from the vector address ad and copy it to pn
void node_get(nodeaddr_t ad, QuadTreeNode* pn) {
	// check whether ad is valid
	if (ad < 0 || ad >= nodevetsz) {
		fprintf(stderr,"node_get: address out of range\n");
		pn->key = INVALIDKEY;
		return;
	}
	if (is_invalid_node(&(nodevet[ad]))) {
		fprintf(stderr,"node_get: node is invalid\n");
	}
	node_copy(pn,&(nodevet[ad]));
}

// store a node in the vector address ad and copy pn to it
void node_put(nodeaddr_t ad, QuadTreeNode* pn) {
	// check whether ad is valid
	if (ad < 0 || ad >= nodevetsz) {
		fprintf(stderr,"node_put: address out of range\n");
		return;
	}
	node_copy(&(nodevet[ad]),pn);
}

// print nodevet for debugging purposes
void node_dump(int ad, int level) {
	// check whether ad is valid
	if (ad < 0 || ad >= nodevetsz) {
		fprintf(stderr,"node_dump: address out of range\n");
		return;
	}
	// insert level spaces for sake of indentation
	for (int i = 0; i < level; i++) printf(" "); 
	// print the node info
	printf("(%f %f) key %ld nw %ld ne %ld sw %ld se\n",
            nodevet[ad].key.x, nodevet[ad].key.y,
            nodevet[ad].nw, nodevet[ad].ne, nodevet[ad].sw, nodevet[ad].se);
}

// dump the whole vector
void node_dumpvet() {
	// general information
	printf("sz %ld alloc %ld 1stavail %ld\n",
				nodevetsz, nodesallocated, firstavail);
	// print each vector entry
	for (int i = 0; i<nodevetsz; i++) {
		printf("(%f %f) key %ld nw %ld ne %ld sw %ld se\n",
            nodevet[i].key.x, nodevet[i].key.y,
            nodevet[i].nw, nodevet[i].ne, nodevet[i].sw, nodevet[i].se);
	}
}

void node_destroy() {
	free(nodevet);
	nodevet=NULL;
	nodevetsz=0;
	nodesallocated=0;
	firstavail=INVALIDADDR;
}