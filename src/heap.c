#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

Heap* heap_initialize(long max_size)
{
    Heap* h = (Heap*) malloc(sizeof(Heap));
    h->size = 0;
    h->neighbors = (Neighbor*) malloc(max_size * sizeof(Neighbor));
    return h;
}

void heap_destroy(Heap* h)
{
    if (h == NULL) return;
    
    free(h->neighbors); h->neighbors = NULL;
    free(h); h = NULL;
}

bool empty(Heap* h)
{
    return h->size == 0;
}

static long get_ancestor(long posicao)
{
    return (posicao - 1) / 2;
}

static long get_left_successor(long posicao)
{
    return 2 * posicao + 1;
}

static long get_right_successor(long posicao)
{
    return 2 * posicao + 2;
}

static long get_max_sucessor(Heap* h, long posicao)
{
    long sucessor_esq = get_left_successor(posicao);
    long sucessor_dir = get_right_successor(posicao);

    // Retorna -1 se o elemento não tiver sucessores
    if (sucessor_esq >= h->size && sucessor_dir >= h->size) return -1; 

    // Retorna o sucessor direito se o esquerdo não existir
    if (sucessor_esq >= h->size) return sucessor_dir; 

    // Retorna o sucessor esquerdo se o direito não existir
    if (sucessor_dir >= h->size) return sucessor_esq; 
    
    // Compara os sucessores esquerdo e direito e retorna o de maior valor.
    return (h->neighbors[sucessor_esq].dist > h->neighbors[sucessor_dir].dist) ? sucessor_esq : sucessor_dir;
}

static void swap(Neighbor *x, Neighbor *y) 
{
    Neighbor temp = *x;
    *x = *y;
    *y = temp;
}

void heap_push(Heap* h, Neighbor x)
{
    // Insere o item na ultima posicao do vetor
    h->neighbors[h->size] = x;
    // Obtem o ancestral do item
    long atual = h->size;
    long ancestral = get_ancestor(atual);
    // Troca o item com seu ancestral até que a condição do heap seja satisfeita
    while (atual > 0 && h->neighbors[atual].dist > h->neighbors[ancestral].dist) {
        swap(&h->neighbors[atual], &h->neighbors[ancestral]);
        atual = ancestral;
        ancestral = get_ancestor(atual);
    }
    h->size++; // Incrementa o tamanho do heap
}

Neighbor heap_pop(Heap* h)
{   
    // Checa por um heap vazio
    if (empty(h)) {
        printf("\nErro. Impossivel remover elemento de um heap vazio.\n");
        exit(1);
    }
    // Guarda uma copia do vizinho a ser retirado (raiz)
    Neighbor ret = h->neighbors[0];
    // Atribui o vizinho da ultima posicao do vetor a raiz e diminiu o size do heap
    h->neighbors[0] = h->neighbors[h->size - 1];
    h->size--;
    // Inicia a iteracao na nova raiz
    long atual = 0;
    // Obtem o maior sucessor da nova raiz 
    long maior_sucessor = get_max_sucessor(h, atual);
    // Troca a nova raiz com seu maior sucessor ate que a condicao do heap seja garantida
    while (maior_sucessor != -1 && h->neighbors[atual].dist < h->neighbors[maior_sucessor].dist) {
        swap(&h->neighbors[atual], &h->neighbors[maior_sucessor]);
        atual = maior_sucessor;

        maior_sucessor = get_max_sucessor(h, atual);
    }
    // Retorna o vizinho retirado
    return ret;
}