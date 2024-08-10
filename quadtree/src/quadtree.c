#include "../include/quadtree.h"

// even the binary tree root is encapsulated
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

// recursive insert function 
nodeaddr_t quadtree_insert_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p) {
	// check whether reached a null node
	if (curr == INVALIDADDR) {
		fprintf(stderr,"quadtree_insert_rec: invalid address\n");
		node_reset(p);
		return INVALIDADDR;
	}
	// retrieve curr node
	// Retrieve current node
    QuadTreeNode aux;
    node_get(curr, &aux);

    // Check if the node is subdivided
    if (!aux.subdivided) {
        // Calculate midpoints
        float mid_x = aux.boundary.x_min + (aux.boundary.x_max - aux.boundary.x_min) / 2;
        float mid_y = aux.boundary.y_min + (aux.boundary.y_max - aux.boundary.y_min) / 2;

        // Subdivide the node and set boundaries for each child
        aux.nw = node_create(p);
        node_get(aux.nw, p);
        p->boundary = (Boundary){aux.boundary.x_min, mid_x, mid_y, aux.boundary.y_max};
        node_put(aux.nw, p);

        aux.ne = node_create(p);
        node_get(aux.ne, p);
        p->boundary = (Boundary){mid_x, aux.boundary.x_max, mid_y, aux.boundary.y_max};
        node_put(aux.ne, p);

        aux.sw = node_create(p);
        node_get(aux.sw, p);
        p->boundary = (Boundary){aux.boundary.x_min, mid_x, aux.boundary.y_min, mid_y};
        node_put(aux.sw, p);

        aux.se = node_create(p);
        node_get(aux.se, p);
        p->boundary = (Boundary){mid_x, aux.boundary.x_max, aux.boundary.y_min, mid_y};
        node_put(aux.se, p);

        aux.subdivided = true;
        node_put(curr, &aux);
    }

    // Determine the quadrant
    if (k.x < aux.boundary.x_min + (aux.boundary.x_max - aux.boundary.x_min) / 2) {
        if (k.y < aux.boundary.y_min + (aux.boundary.y_max - aux.boundary.y_min) / 2) {
            // Insert into SW quadrant
            return quadtree_insert_rec(k, aux.sw, p);
        } else {
            // Insert into NW quadrant
            return quadtree_insert_rec(k, aux.nw, p);
        }
    } else {
        if (k.y < aux.boundary.y_min + (aux.boundary.y_max - aux.boundary.y_min) / 2) {
            // Insert into SE quadrant
            return quadtree_insert_rec(k, aux.se, p);
        } else {
            // Insert into NE quadrant
            return quadtree_insert_rec(k, aux.ne, p);
        }
    }
}

// initial insertion function
nodeaddr_t quadtree_insert(nodekey_t k, QuadTreeNode* pn) {
	// check whether pn is null
	if (pn == NULL) {
		fprintf(stderr,"quadtree_insert: pn NULL\n");
		return INVALIDADDR;
	}
	// reset and initialize node
	node_reset(pn);
	pn->key = k;
	// check whether we are inserting the root
	if (root == INVALIDADDR) {
		root = node_create(pn);    
		node_put(root,pn);
		return root;
	} else {
		// proceed recursively
		return quadtree_insert_rec(k, root, pn);
	}
}

/*

// recursive search function
nodeaddr_t quadtree_search_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p) {
	// reached a null node 
	if (p == NULL) {
		fprintf(stderr,"quadtree_search_rec: node NULL\n");
		return INVALIDADDR;
	}
	// retrieve the current node
	node_get(curr,p);
	// check the key to define the traversal direction
	if (k < p->key){
		// k should be on the left
		if (p->left == INVALIDADDR) {
			// no left child, k is absent
			fprintf(stderr,"quadtree_search_rec: node absent\n");
			node_reset(p);
			return INVALIDADDR;
		} else {
			// search recursively
			return quadtree_search_rec(k,p->left,p);
		}
	} 
	if (k > p->key) {
		// k should be on the right
		if (p->right == INVALIDADDR) {
			// no right child, k is absent
			fprintf(stderr,"quadtree_search_rec: node absent\n");
			node_reset(p);
			return INVALIDADDR;
		} else {
			// search recursively
			return quadtree_search_rec(k,p->right,p);
		}
	} 
	// found node, p contains k
	return curr; 
}

// initial search function
nodeaddr_t quadtree_search(nodekey_t k, QuadTreeNode* pn) {
	// check whether the return node is null
	if (pn == NULL) {
		fprintf(stderr,"quadtree_search: pn NULL\n");
		return INVALIDADDR;
	}
	// check whether the tree is null
	if (root == INVALIDADDR) {
		fprintf(stderr,"quadtree_search: tree empty\n");
		return INVALIDADDR;
	}
	// call the recursive function
	return quadtree_search_rec(k, root, pn);
}

// recursive dump function
void quadtree_dump_rec(nodeaddr_t curr, int level) { 
	// reached an invalid node
	if (curr == INVALIDADDR) {
		fprintf(stderr,"quadtree_dump_rec: invalid address\n");
		return;
	}
	// retrieve current node
	node_t aux;
	node_get(curr,&aux);
	// dump node with level indentation
	node_dump(curr,level);
	// proceed recursively in a BFS fashion
	if (aux.left != INVALIDADDR)
		quadtree_dump_rec(aux.left,level+1);
	if (aux.right != INVALIDADDR)
		quadtree_dump_rec(aux.right,level+1);
	return; 
}

// initial dump function
void quadtree_dump() {
	// check whether the tree contains any node
	if (root == INVALIDADDR) {
		fprintf(stderr,"quadtree_dump: empty tree\n");
		//dump the node vector to be sure
		node_dumpvet();
		return;
	} else {
		// proceed recursively
		printf("quadtree_dump: root %ld\n",root);
		quadtree_dump_rec(root, 0);
	}
	// print the node vector for debugging purposes
	node_dumpvet();
	return;
}

// Recursive function that finds the predecessor node
nodeaddr_t quadtree_pred_rec(nodeaddr_t curr, QuadTreeNode* p, QuadTreeNode* ppn, nodeaddr_t * ppa) {
	// we reached a null pointer to a node
	if (p == NULL) {
		fprintf(stderr,"quadtree_search_rec: node NULL\n");
		return INVALIDADDR;
	}
	// get the node at position p
	node_get(curr,p);
	// if it has a right child, keep checking
	if (p->right != INVALIDADDR) {
		// if there is right child, continue there
		// update ppn and ppa
		*ppa = curr;
		// save the current node
		node_copy(ppn,p);
		return quadtree_pred_rec(p->right,p,ppn,ppa);
	} else {
		// otherwise, this is the predecessor node
		return curr;
	}
}

// initial function to determine the predecessor
nodeaddr_t quadtree_pred(nodeaddr_t curr, QuadTreeNode* pn, QuadTreeNode* ppn, nodeaddr_t * ppa) {
	// initialize node and respective pointer that will contain the predecessor
	node_reset(ppn);
	*ppa = INVALIDADDR;
	// check whether current position is null
	if (pn == NULL) {
		fprintf(stderr,"quadtree_pred: pn NULL\n");
		return INVALIDADDR;
	}
	// check whether the tree is empty
	if (curr == INVALIDADDR) {
		fprintf(stderr,"quadtree_pred: tree empty\n");
		return INVALIDADDR;
	}
	// retrieve current node from vector
	node_get(curr,pn);
	if (pn->left != INVALIDADDR) {
		// the first call goes to the left sub tree
		return quadtree_pred_rec(pn->left, pn, ppn, ppa);
	} else {
		// there is no predecessor
		node_reset(pn);
		return INVALIDADDR;
	}
}

// recursive remove function
int quadtree_remove_rec(nodekey_t k, nodeaddr_t curr, QuadTreeNode* p, nodeaddr_t* pna) {
	// reached an invalid node
	if (curr == INVALIDADDR) {
		fprintf(stderr,"quadtree_remove_rec: invalid address\n");
		node_reset(p);
		return 0;
	}
	// retrieve current node
	node_t aux;
	node_get(curr,&aux);
	// check whether k should be on the left
	if (k < aux.key) {
		// check whether there is left child
		if (aux.left != INVALIDADDR) {
			nodeaddr_t na;
			// proceed recursively
			int ret = quadtree_remove_rec(k,aux.left,p,&na);
			if (ret) {
				// we removed na from the tree, adjust aux.left
				aux.left = na;
				// save the node
				node_put(curr,&aux);
			}
		} 
		else {
			fprintf(stderr,"quadtree_remove_rec: key not in tree\n");
		}
		return 0; //nothing to change up
	} 
	// check whether k should be on the right
	if (k > aux.key) {
		// check whether there is right child
		if (aux.right != INVALIDADDR) {
			nodeaddr_t na;
			// proceed recursively
			int ret = quadtree_remove_rec(k,aux.right,p,&na);
			if (ret) {
				// we removed na from the tree, adjust aux.right
				aux.right = na;
				// save the node
				node_put(curr,&aux);
			}
		} 
		else {
			fprintf(stderr,"quadtree_remove_rec: key not in tree\n");
		}
		return 0; //nothing to change up
	} 
	// found the node to be removed, aux contains it
	// should handle children, if any

	// if there is not right child, then the left child becomes the removed node
	// we should return its address and remove curr
	// printf("will remove %ld\n",curr);
	if (aux.right == INVALIDADDR) {
		printf("quadtree_remove_rec: there is no right child\n");
		node_delete(curr); // removing curr from the tree
		node_copy(p,&aux); // returning the removed node
		*pna = aux.left;
		return 1;
	}

	// there is a left child
	if (aux.left != INVALIDADDR) {
		// first find the predecessor
		node_t auxpred, auxppred;
		nodeaddr_t ppred;
		nodeaddr_t pred = quadtree_pred(curr, &auxpred, &auxppred, &ppred);

		// there are two possible cases
		// predecessor is the left child of curr, ppred == INVALIDADDR
		// update links and remove pred
		if (ppred == INVALIDADDR) {
			printf("quadtree_remove_rec: pred is the left child\n");
			node_copy(p,&aux); // returning the removed node
			// update aux
			aux.key = auxpred.key;
			aux.left = auxpred.left;
			node_put(curr,&aux); // storing aux 
			node_delete(pred); // removing predecessor
			return 0; // no up pointers to fix
		} else {
			// predecessor is a right grandchild of curr
			printf("quadtree_remove_rec: pred right child aux %ld auxpred %ld auxppred %ld\n", aux.key, auxpred.key, auxppred.key);
			node_copy(p,&aux); // returning the removed node
			// update auxpred and aux
			auxppred.right = auxpred.left;
			aux.key = auxpred.key;
			node_delete(pred); 
			// storing aux and auxpred
			node_put(curr,&aux);
			node_put(ppred,&auxppred);
			return 0;
		}
	}
	// there maybe a right child
	printf("quadtree_remove_rec: there may be a right child\n");
	node_copy(p,&aux); // returning the removed node
	node_delete(curr);
	*pna = aux.right;
	// we should update pointer up
	return 1; 
}

// initial remove function
void quadtree_remove(nodekey_t k, QuadTreeNode* pn) {
	// check whether on is not null
	if (pn == NULL) {
		fprintf(stderr,"quadtree_remove: pn NULL\n");
		return;
	}
	// check whether there is any node in the tree
	if (root == INVALIDADDR) {
		fprintf(stderr,"quadtree_remove: tree empty\n");
		return;
	}
	// initialize pn
	node_reset(pn);
	nodeaddr_t na;
	// proceed recursively
	int ret = quadtree_remove_rec(k,root,pn,&na);
	// check whether it is necessary to update the root
	if (ret) {
		root = na;
	}
	return;
}

*/