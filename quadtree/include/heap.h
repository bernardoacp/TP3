#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "qnode.h"

typedef struct {
    nodeaddr_t addr;
    double dist;
} Neighbor;

// Max heap.
typedef struct s_heap {
    long size;
    Neighbor* neighbors;
} Heap;

// Cria um novo heap e inicializa seu tamanho e o array de dados.
Heap* heap_initialize(long max_size);

// Libera a memoria alocada para os dados.
void heap_destroy(Heap* h);

// Insere um novo elemento, garantido a manutencao das propriedades do heap.
void heap_push(Heap* h, Neighbor x);

// Remove a raiz, garantindo a manutencao das propriedades do heap.
Neighbor heap_pop(Heap* h);

// Retorna 1 caso h esteja vazio, 0 caso contrário.
bool empty(Heap* h); 

#endif