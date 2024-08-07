// bintreetest.c
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
// 
// Test program for the vectorized binary tree ADT 

#include "bintree.h"
#include "memlog.h"
#include "msgassert.h"

int main(int argc, char * argv[]){
   node_t aux;  
   nodekey_t k;
   nodeaddr_t na;

   // inicia registro de acessos a memoria e nome do arquivo
   iniciaMemLog("/tmp/memlog.out");

   int regmem = 1;
   if (regmem){
     ativaMemLog();
   } else {
     desativaMemLog();
   }

   // create the binary tree with at most 25 nodes
   defineFaseMemLog(0);
   bintree_create(25);
   bintree_dump();

   defineFaseMemLog(1);
   // insert keys
   k=50; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=26; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=38; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=72; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=47; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   k=24; na=bintree_insert(k,&aux);
   printf("inserted %ld in %ld\n",k,na); bintree_dump();
   
   defineFaseMemLog(2);
   // search for the inserted keys
   na=bintree_search(50,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(72,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(38,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(26,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(24,&aux); printf("key %ld in %ld\n",aux.key,na);
   na=bintree_search(47,&aux); printf("key %ld in %ld\n",aux.key,na);

   defineFaseMemLog(3);
   // remove some keys
   k=38; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=72; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=26; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=50; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=24; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();
   k=47; bintree_remove(k,&aux); printf("removed %ld\n",k); bintree_dump();

   // destroy the tree
   bintree_destroy();
   return finalizaMemLog();
}
