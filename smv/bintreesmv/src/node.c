#include "node.h"

// internal structure keeping the vector of nodes


nodevet_t nv;

void node_reset(ptr_node_t pn){
  pn->key = INVALIDKEY; 
  pn->left = INVALIDADDR;
  pn->right = INVALIDADDR;
}

void node_copy(ptr_node_t dst, ptr_node_t src){
  dst->key = src->key;
  dst->left = src->left;
  dst->right = src->right;
}

long node_initialize(long numnodes){

  nv.vet = NULL;
  nv.nodevetsz = 0;
  nv.nodesallocated = 0;
  nv.firstavail = INVALIDADDR;
  nv.vet = (ptr_node_t) init_page();
  if (nv.vet == NULL){
    fprintf(stderr,"node_initialize: could not allocate nv.vet\n");
    return 0;
  }
  nv.nodevetsz = numnodes;
  // create the chain of available nodes as a linked list
  node_t aux;
  for (long i=0;i<nv.nodevetsz; i++){
    node_reset(&aux);
    aux.left = (nodeaddr_t) i+1;
    node_put((nodeaddr_t) i, &aux); 
  }
  node_reset(&aux);
  node_put((nodeaddr_t) (nv.nodevetsz-1), &aux); 
  nv.firstavail = (nodeaddr_t) 0;

  return numnodes;
}

nodeaddr_t node_create(ptr_node_t pn){
  nodeaddr_t ret;
  node_t aux;
  if (nv.nodesallocated == nv.nodevetsz){
    fprintf(stderr,"node_create: could not create node\n");
    node_reset(pn);
    return INVALIDADDR;
  }
  ret = nv.firstavail;
  node_get(ret, &aux);
  nv.firstavail = aux.left;
  nv.nodesallocated++;
  node_put(ret,pn);
  return ret;
}

void node_delete(nodeaddr_t ad){
  if (ad<0 || ad>=nv.nodevetsz){
    fprintf(stderr,"node_delete: address out of range\n");
    return;
  }
  node_t aux;
  node_get(ad,&aux);
  if (aux.key == INVALIDKEY){
    fprintf(stderr,"node_delete: node already deleted\n");
  }
  node_reset(&aux);
  aux.left = nv.firstavail;
  node_put(ad,&aux);
  nv.firstavail = ad;
  nv.nodesallocated--;
}

void node_get(nodeaddr_t ad, ptr_node_t pn){
  if (ad<0 || ad>=nv.nodevetsz){
    fprintf(stderr,"node_get: address out of range\n");
    pn->key = INVALIDKEY;
    return;
  }
  if (nv.vet[ad].key == INVALIDKEY){
    fprintf(stderr,"node_get: node is invalid\n");
  }
  node_copy(pn,&(nv.vet[ad]));
}

void node_put(nodeaddr_t ad, ptr_node_t pn){
  if (ad<0 || ad>=nv.nodevetsz){
    fprintf(stderr,"node_put: address out of range\n");
    return;
  }
  node_copy(&(nv.vet[ad]),pn);
}

// print nodevet for debugging purposes
void node_dump(int ad, int level){
  if (ad<0 || ad>=nv.nodevetsz){
    fprintf(stderr,"node_dump: address out of range\n");
    return;
  }
  node_t aux;
  node_get(ad,&aux);
  for (int i=0; i<level; i++) printf(" "); 
  printf("(%d) key %ld left %ld right %ld\n",
         ad,aux.key, aux.left, aux.right);
}

void node_dumpvet(){
  printf("sz %d alloc %d 1stavail %ld\n",
         nv.nodevetsz, nv.nodesallocated, nv.firstavail);
  for (int i = 0; i<nv.nodevetsz; i++){
    node_dump(i,0);
  }
}


void node_destroy(){
  end_page();
  nv.vet = NULL;
  nv.nodevetsz=0;
  nv.nodesallocated=0;
  nv.firstavail=INVALIDADDR;
}


