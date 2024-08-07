// bintree.c
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
// 
// Vectorized Binary Tree Implementation

#include "bintree.h"

// even the binary tree root is encapsulated
nodeaddr_t root = INVALIDADDR;

// private functions
nodeaddr_t bintree_pred_rec(nodeaddr_t curr, ptr_node_t pn, 
                            ptr_node_t ppn, nodeaddr_t * ppa);
nodeaddr_t bintree_pred(nodeaddr_t curr, ptr_node_t pn, 
                        ptr_node_t ppn, nodeaddr_t * ppa);
nodeaddr_t bintree_search_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p);
nodeaddr_t bintree_insert_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p);
int bintree_remove_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p, 
                       nodeaddr_t * pna);

// Create a binary tree with at most numnodes nodes
void bintree_create(long numnodes){
  // we should create and initialize the vector that will contain the tree
  node_initialize(numnodes);
}

// Destroy a binary tree
void bintree_destroy(){
  // we should first dealocate the vector that contains the tree
  node_destroy();
  // reset the tree root
  root = INVALIDADDR;
}

// Recursive function that finds the predecessor node
nodeaddr_t bintree_pred_rec(nodeaddr_t curr, ptr_node_t p,
                            ptr_node_t ppn, nodeaddr_t * ppa){
  // we reached a null pointer to a node
  if (p == NULL){
    fprintf(stderr,"bintree_search_rec: node NULL\n");
    return INVALIDADDR;
  }
  // get the node at position p
  node_get(curr,p);
  // if it has a right child, keep checking
  if (p->right != INVALIDADDR){
    // if there is right child, continue there
    // update ppn and ppa
    *ppa = curr;
    // save the current node
    node_copy(ppn,p);
    return bintree_pred_rec(p->right,p,ppn,ppa);
  } else {
    // otherwise, this is the predecessor node
    return curr;
  }
}

// initial function to determine the predecessor
nodeaddr_t bintree_pred(nodeaddr_t curr, ptr_node_t pn, 
                        ptr_node_t ppn, nodeaddr_t * ppa){
  // initialize node and respective pointer that will contain the predecessor
  node_reset(ppn);
  *ppa = INVALIDADDR;
  // check whether current position is null
  if (pn == NULL){
    fprintf(stderr,"bintree_pred: pn NULL\n");
    return INVALIDADDR;
  }
  // check whether the tree is empty
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_pred: tree empty\n");
    return INVALIDADDR;
  }
  // retrieve current node from vector
  node_get(curr,pn);
  if (pn->left != INVALIDADDR){
    // the first call goes to the left sub tree
    return bintree_pred_rec(pn->left, pn, ppn, ppa);
  } else {
    // there is no predecessor
    node_reset(pn);
    return INVALIDADDR;
  }
}

// recursive search function
nodeaddr_t bintree_search_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p){
  // reached a null node 
  if (p == NULL){
    fprintf(stderr,"bintree_search_rec: node NULL\n");
    return INVALIDADDR;
  }
  // retrieve the current node
  node_get(curr,p);
  // check the key to define the traversal direction
  if (k < p->key){
    // k should be on the left
    if (p->left == INVALIDADDR){
      // no left child, k is absent
      fprintf(stderr,"bintree_search_rec: node absent\n");
      node_reset(p);
      return INVALIDADDR;
    } else {
      // search recursively
      return bintree_search_rec(k,p->left,p);
    }
  } 
  if (k > p->key){
    // k should be on the right
    if (p->right == INVALIDADDR){
      // no right child, k is absent
      fprintf(stderr,"bintree_search_rec: node absent\n");
      node_reset(p);
      return INVALIDADDR;
    } else {
      // search recursively
      return bintree_search_rec(k,p->right,p);
    }
  } 
  // found node, p contains k
  return curr; 
}

// initial search function
nodeaddr_t bintree_search(nodekey_t k, ptr_node_t pn){
  // check whether the return node is null
  if (pn == NULL){
    fprintf(stderr,"bintree_search: pn NULL\n");
    return INVALIDADDR;
  }
  // check whether the tree is null
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_search: tree empty\n");
    return INVALIDADDR;
  }
  // call the recursive function
  return bintree_search_rec(k, root, pn);
}

// recursive insert function 
nodeaddr_t bintree_insert_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p){
  // check whether reached a null node
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_insert_rec: invalid address\n");
    node_reset(p);
    return INVALIDADDR;
  }
  // retrieve curr node
  node_t aux;
  node_get(curr,&aux);
  // check whether it should be inserted on the left
  if (k < aux.key){
    // there is left child, proceed recursively
    if (aux.left != INVALIDADDR)
      return bintree_insert_rec(k,aux.left,p);
    else{
      // found the insertion location
      aux.left = node_create(p);
      // store the created node
      node_put(curr,&aux);
      return aux.left;
    }
  } 
  // check whether it should be inserted on the left
  if (k > aux.key){
    // there is right child proceed recursively
    if (aux.right != INVALIDADDR)
      return bintree_insert_rec(k,aux.right,p);
    else{
      // found the insertion location
      aux.right = node_create(p);
      // store the created node
      node_put(curr,&aux);
      return aux.right;
    }
  } 
  // found node, p already contains k, just issue a warning
  fprintf(stderr,"bintree_insert_rec: node already in tree\n");
  return curr; 
}

// initial insertion function
nodeaddr_t bintree_insert(nodekey_t k, ptr_node_t pn){
  // check whether pn is null
  if (pn == NULL){
    fprintf(stderr,"bintree_insert: pn NULL\n");
    return INVALIDADDR;
  }
  // reset and initialize node
  node_reset(pn);
  pn->key = k;
  // check whether we are inserting the root
  if (root == INVALIDADDR){
    root = node_create(pn);    
    node_put(root,pn);
    return root;
  } else{
    // proceed recursively
    return bintree_insert_rec(k, root, pn);
  }
}

// recursive remove function
int bintree_remove_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p, 
                       nodeaddr_t * pna){
  // reached an invalid node
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_remove_rec: invalid address\n");
    node_reset(p);
    return 0;
  }
  // retrieve current node
  node_t aux;
  node_get(curr,&aux);
  // check whether k should be on the left
  if (k < aux.key){
    // check whether there is left child
    if (aux.left != INVALIDADDR){
      nodeaddr_t na;
      // proceed recursively
      int ret = bintree_remove_rec(k,aux.left,p,&na);
      if (ret){
	// we removed na from the tree, adjust aux.left
	aux.left = na;
	// save the node
	node_put(curr,&aux);
      }
    } else{
      fprintf(stderr,"bintree_remove_rec: key not in tree\n");
    }
    return 0; //nothing to change up
  } 
  // check whether k should be on the right
  if (k > aux.key){
    // check whether there is right child
    if (aux.right != INVALIDADDR){
      nodeaddr_t na;
      // proceed recursively
      int ret = bintree_remove_rec(k,aux.right,p,&na);
      if (ret){
	// we removed na from the tree, adjust aux.right
	aux.right = na;
	// save the node
	node_put(curr,&aux);
      }
    } else{
      fprintf(stderr,"bintree_remove_rec: key not in tree\n");
    }
    return 0; //nothing to change up
  } 
  // found the node to be removed, aux contains it
  // should handle children, if any

  // if there is not right child, then the left child becomes the removed node
  // we should return its address and remove curr
  // printf("will remove %ld\n",curr);
  if (aux.right == INVALIDADDR){
    printf("bintree_remove_rec: there is no right child\n");
    node_delete(curr); // removing curr from the tree
    node_copy(p,&aux); // returning the removed node
    *pna = aux.left;
    return 1;
  }

  // there is a left child
  if (aux.left != INVALIDADDR){
    // first find the predecessor
    node_t auxpred, auxppred;
    nodeaddr_t ppred;
    nodeaddr_t pred = bintree_pred(curr, &auxpred, &auxppred, &ppred);

    // there are two possible cases
    // predecessor is the left child of curr, ppred == INVALIDADDR
    // update links and remove pred
    if (ppred == INVALIDADDR){
      printf("bintree_remove_rec: pred is the left child\n");
      node_copy(p,&aux); // returning the removed node
      // update aux
      aux.key = auxpred.key;
      aux.left = auxpred.left;
      node_put(curr,&aux); // storing aux 
      node_delete(pred); // removing predecessor
      return 0; // no up pointers to fix
    } else {
      // predecessor is a right grandchild of curr
      printf("bintree_remove_rec: pred right child aux %ld auxpred %ld auxppred %ld\n",
              aux.key, auxpred.key, auxppred.key);
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
  printf("bintree_remove_rec: there may be a right child\n");
  node_copy(p,&aux); // returning the removed node
  node_delete(curr);
  *pna = aux.right;
  // we should update pointer up
  return 1; 
}

// initial remove function
void bintree_remove(nodekey_t k, ptr_node_t pn){
  // check whether on is not null
  if (pn == NULL){
    fprintf(stderr,"bintree_remove: pn NULL\n");
    return;
  }
  // check whether there is any node in the tree
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_remove: tree empty\n");
    return;
  }
  // initialize pn
  node_reset(pn);
  nodeaddr_t na;
  // proceed recursively
  int ret = bintree_remove_rec(k,root,pn,&na);
  // check whether it is necessary to update the root
  if (ret){
    root = na;
  }
  return;
}

// recursive dump function
void bintree_dump_rec(nodeaddr_t curr, int level){
  // reached an invalid node
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_dump_rec: invalid address\n");
    return;
  }
  // retrieve current node
  node_t aux;
  node_get(curr,&aux);
  // dump node with level indentation
  node_dump(curr,level);
  // proceed recursively in a BFS fashion
  if (aux.left != INVALIDADDR)
     bintree_dump_rec(aux.left,level+1);
  if (aux.right != INVALIDADDR)
     bintree_dump_rec(aux.right,level+1);
  return; 
}

// initial dump function
void bintree_dump(){
  // check whether the tree contains any node
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_dump: empty tree\n");
    //dump the node vector to be sure
    node_dumpvet();
    return;
  } else{
    // proceed recursively
    printf("bintree_dump: root %ld\n",root);
    bintree_dump_rec(root, 0);
  }
  // print the node vector for debugging purposes
  node_dumpvet();
  return;
}

