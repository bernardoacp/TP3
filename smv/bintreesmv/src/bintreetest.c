#include "bintree.h"

int main(int argc, char * argv[]){
   node_t aux;  
   nodekey_t k;
   nodeaddr_t na;

   bintree_create(6);
   bintree_dump();

   k=10; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=6; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=8; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=12; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=7; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=4; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   na=bintree_search(10,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(12,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(8,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(6,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(4,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(7,&aux); printf("key %ld in %ld\n",aux.key,na);
   //k=7; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=8; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=12; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=6; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=10; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=4; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=7; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   bintree_destroy();
   return 0;
}
