#include "bintree.h"

// even the root is encapsulated
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

void bintree_create(long numnodes){
  node_initialize(numnodes);
}

void bintree_destroy(){
  node_destroy();
  root = INVALIDADDR;
}

nodeaddr_t bintree_pred_rec(nodeaddr_t curr, ptr_node_t p,
                            ptr_node_t ppn, nodeaddr_t * ppa){
  if (p == NULL){
    fprintf(stderr,"bintree_search_rec: node NULL\n");
    return INVALIDADDR;
  }
  node_get(curr,p);
  if (p->right != INVALIDADDR){
    // if there is right child, continue there
    // update ppn and ppa
    *ppa = curr;
    node_copy(ppn,p);
    return bintree_pred_rec(p->right,p,ppn,ppa);
  } else {
    // otherwise, this is the predecessor node
    return curr;
  }
}

nodeaddr_t bintree_pred(nodeaddr_t curr, ptr_node_t pn, 
                        ptr_node_t ppn, nodeaddr_t * ppa){
  node_reset(ppn);
  *ppa = INVALIDADDR;
  if (pn == NULL){
    fprintf(stderr,"bintree_pred: pn NULL\n");
    return INVALIDADDR;
  }
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_pred: tree empty\n");
    return INVALIDADDR;
  }
  node_get(curr,pn);
  if (pn->left != INVALIDADDR){
    return bintree_pred_rec(pn->left, pn, ppn, ppa);
  } else {
    // there is no predecessor
    node_reset(pn);
    return INVALIDADDR;
  }
}

nodeaddr_t bintree_search_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p){
  if (p == NULL){
    fprintf(stderr,"bintree_search_rec: node NULL\n");
    return INVALIDADDR;
  }
  node_get(curr,p);
  if (k < p->key){
    if (p->left == INVALIDADDR){
      fprintf(stderr,"bintree_search_rec: node absent\n");
      node_reset(p);
      return INVALIDADDR;
    } else {
      return bintree_search_rec(k,p->left,p);
    }
  } 
  if (k > p->key){
    if (p->right == INVALIDADDR){
      fprintf(stderr,"bintree_search_rec: node absent\n");
      node_reset(p);
      return INVALIDADDR;
    } else {
      return bintree_search_rec(k,p->right,p);
    }
  } 
  // found node, p already contains it
  return curr; 
}

nodeaddr_t bintree_search(nodekey_t k, ptr_node_t pn){
  if (pn == NULL){
    fprintf(stderr,"bintree_search: pn NULL\n");
    return INVALIDADDR;
  }
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_search: tree empty\n");
    return INVALIDADDR;
  }
  return bintree_search_rec(k, root, pn);
}

nodeaddr_t bintree_insert_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p){
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_insert_rec: invalid address\n");
    node_reset(p);
    return INVALIDADDR;
  }
  node_t aux;
  node_get(curr,&aux);
  if (k < aux.key){
    if (aux.left != INVALIDADDR)
      return bintree_insert_rec(k,aux.left,p);
    else{
      aux.left = node_create(p);
      node_put(curr,&aux);
      return aux.left;
    }
  } 
  if (k > aux.key){
    if (aux.right != INVALIDADDR)
      return bintree_insert_rec(k,aux.right,p);
    else{
      aux.right = node_create(p);
      node_put(curr,&aux);
      return aux.right;
    }
  } 
  // found node, p already contains it, just issue a warning
  fprintf(stderr,"bintree_insert_rec: node already in tree\n");
  return curr; 
}

nodeaddr_t bintree_insert(nodekey_t k, ptr_node_t pn){
  if (pn == NULL){
    fprintf(stderr,"bintree_insert: pn NULL\n");
    return INVALIDADDR;
  }
  node_reset(pn);
  pn->key = k;
  if (root == INVALIDADDR){
    root = node_create(pn);    
    node_put(root,pn);
    return root;
  } else{
    return bintree_insert_rec(k, root, pn);
  }
}

int bintree_remove_rec(nodekey_t k, nodeaddr_t curr, ptr_node_t p, 
                       nodeaddr_t * pna){
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_remove_rec: invalid address\n");
    node_reset(p);
    return 0;
  }
  node_t aux;
  node_get(curr,&aux);
  if (k < aux.key){
    if (aux.left != INVALIDADDR){
      nodeaddr_t na;
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
  if (k > aux.key){
    if (aux.right != INVALIDADDR){
      nodeaddr_t na;
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
  // if there is not right child, then the left child becomes the node
  // we should return its address and remove curr
  printf("will remove %ld\n",curr);

  if (aux.right == INVALIDADDR){
    printf("bintree_remove_rec: there is no right child\n");
    node_delete(curr); // removing curr from the tree
    node_copy(p,&aux); // returning the removed node
    *pna = aux.left;
    return 1;
  }

  if (aux.left != INVALIDADDR){
    // find the predecessor
    node_t auxpred, auxppred;
    nodeaddr_t ppred;
    nodeaddr_t pred = bintree_pred(curr, &auxpred, &auxppred, &ppred);

    // we have two cases
    // predecessor is the left child of curr, ppred == INVALIDADDR
    // update links and remove pred
    if (ppred == INVALIDADDR){
      printf("bintree_remove_rec: pred is the left child\n");
      node_copy(p,&aux); // returning the removed node
      aux.key = auxpred.key;
      aux.left = auxpred.left;
      node_put(curr,&aux); // saving node
      node_delete(pred); // removing predecessor
      return 0; // no up pointers to fix
    } else {
      // predecessor is a right grandchild of curr
      printf("bintree_remove_rec: pred right child aux %ld auxpred %ld auxppred %ld\n",
              aux.key, auxpred.key, auxppred.key);
      node_copy(p,&aux); // returning the removed node
      auxppred.right = auxpred.left;
      aux.key = auxpred.key;
      node_delete(pred); 
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
  return 1; 
}

void bintree_remove(nodekey_t k, ptr_node_t pn){
  if (pn == NULL){
    fprintf(stderr,"bintree_remove: pn NULL\n");
    return;
  }
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_remove: tree empty\n");
    return;
  }
  node_reset(pn);
  nodeaddr_t na;
  int ret = bintree_remove_rec(k,root,pn,&na);
  if (ret){
    root = na;
  }
  return;
}


void bintree_dump_rec(nodeaddr_t curr, int level){
  if (curr == INVALIDADDR){
    fprintf(stderr,"bintree_dump_rec: invalid address\n");
    return;
  }
  node_t aux;
  node_get(curr,&aux);
  node_dump(curr,level);
  if (aux.left != INVALIDADDR)
     bintree_dump_rec(aux.left,level+1);
  if (aux.right != INVALIDADDR)
     bintree_dump_rec(aux.right,level+1);
  return; 
}

void bintree_dump(){
  if (root == INVALIDADDR){
    fprintf(stderr,"bintree_dump: empty tree\n");
    node_dumpvet();
    return;
  } else{
    printf("bintree_dump: root %ld\n",root);
    bintree_dump_rec(root, 0);
  }
  node_dumpvet();
  return;
}

