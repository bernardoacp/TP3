// node.c
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
// 
// Vectorized Binary Tree Node Implementation

#include "node.h"
#include "memlog.h"
#include "msgassert.h"

// encapsulated variables keeping the vector of nodes
ptr_node_t nodevet=NULL;
long nodevetsz=0;
long nodesallocated=0;
nodeaddr_t firstavail=INVALIDADDR;

// reset node, removing any previous usage information
void node_reset(ptr_node_t pn){
  pn->key = INVALIDKEY; 
  pn->left = INVALIDADDR;
  pn->right = INVALIDADDR;
}

// copy contents of node src to node dst
void node_copy(ptr_node_t dst, ptr_node_t src){
  dst->key = src->key;
  dst->left = src->left;
  dst->right = src->right;
}

// initialize a vectorized binary tree that will contain at most numnodes
long node_initialize(long numnodes){
  // allocate node vet
  nodevet = (ptr_node_t) malloc(numnodes*sizeof(node_t));
  if (nodevet == NULL){
    fprintf(stderr,"node_initialize: could not allocate nodevet\n");
    return 0;
  }
  // initialize nodevetsz;
  nodevetsz = numnodes;
  // create the chain of available nodes as a linked list
  for (long i=0;i<nodevetsz; i++){
    node_reset(&(nodevet[i]));
    nodevet[i].left = (nodeaddr_t) i+1;
    ESCREVEMEMLOG((long int)(&(nodevet[i])),sizeof(node_t),0);
  }
  // last node in the chain
  nodevet[nodevetsz-1].left = INVALIDADDR;
  // node 0 is the first avail
  firstavail = (nodeaddr_t) 0;
  return numnodes;
}

// create node from pn
nodeaddr_t node_create(ptr_node_t pn){
  nodeaddr_t ret;
  // check whether node vector is full
  if (nodesallocated == nodevetsz){
    fprintf(stderr,"node_create: could not create node\n");
    node_reset(pn);
    return INVALIDADDR;
  }
  // remove a node from the avail chain, update the controls and copy pn to it
  ret = firstavail;
  firstavail = nodevet[ret].left;
  LEMEMLOG((long int)(&(nodevet[ret])),sizeof(node_t),0);
  nodesallocated++;
  node_copy(&(nodevet[ret]),pn);
  ESCREVEMEMLOG((long int)(&(nodevet[ret])),sizeof(node_t),0);
  return ret;
}

// delete virtually node ad, making it available for futher creation
void node_delete(nodeaddr_t ad){
  // check whether ad is a valid address
  if (ad<0 || ad>=nodevetsz){
    fprintf(stderr,"node_delete: address out of range\n");
    return;
  }
  if (nodevet[ad].key == INVALIDKEY){
    fprintf(stderr,"node_delete: node already deleted\n");
  }
  // just reset and add to the front of available list
  node_reset(&(nodevet[ad]));
  ESCREVEMEMLOG((long int)(&(nodevet[ad])),sizeof(node_t),0);
  nodevet[ad].left = firstavail;
  firstavail = ad;
  nodesallocated--;
}

// retrieve a node from the vector address ad and copy it to pn
void node_get(nodeaddr_t ad, ptr_node_t pn){
  // check whether ad is valid
  if (ad<0 || ad>=nodevetsz){
    fprintf(stderr,"node_get: address out of range\n");
    pn->key = INVALIDKEY;
    return;
  }
  if (nodevet[ad].key == INVALIDKEY){
    fprintf(stderr,"node_get: node is invalid\n");
  }
  node_copy(pn,&(nodevet[ad]));
  LEMEMLOG((long int)(&(nodevet[ad])),sizeof(node_t),0);
}

// store a node in the vector address ad and copy pn to it
void node_put(nodeaddr_t ad, ptr_node_t pn){
  // check whether ad is valid
  if (ad<0 || ad>=nodevetsz){
    fprintf(stderr,"node_put: address out of range\n");
    return;
  }
  node_copy(&(nodevet[ad]),pn);
  ESCREVEMEMLOG((long int)(&(nodevet[ad])),sizeof(node_t),0);
}

// print nodevet for debugging purposes
void node_dump(int ad, int level){
  // check whether ad is valid
  if (ad<0 || ad>=nodevetsz){
    fprintf(stderr,"node_dump: address out of range\n");
    return;
  }
  // insert level spaces for sake of indentation
  for (int i=0; i<level; i++) printf(" "); 
  // print the node info
  printf("(%d) key %ld left %ld right %ld\n",
         ad,nodevet[ad].key, nodevet[ad].left, nodevet[ad].right);
  ESCREVEMEMLOG((long int)(&(nodevet[ad])),sizeof(node_t),0);
}

// dump the whole vector
void node_dumpvet(){
  // general information
  printf("sz %ld alloc %ld 1stavail %ld\n",
         nodevetsz, nodesallocated, firstavail);
  // print each vector entry
  for (int i = 0; i<nodevetsz; i++){
    printf("(%d) key %ld left %ld right %ld\n",
            i,nodevet[i].key,nodevet[i].left,nodevet[i].right);
    ESCREVEMEMLOG((long int)(&(nodevet[i])),sizeof(node_t),0);
  }
}


void node_destroy(){
  free(nodevet);
  nodevet=NULL;
  nodevetsz=0;
  nodesallocated=0;
  firstavail=INVALIDADDR;
}


