#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "boundary.h"
#include "qnode.h"
#include "heap.h"

// Cria uma quadtree com um número especificado de nós e um limite espacial
void quadtree_create(long numnodes, Boundary boundary);

// Destroi a quadtree, liberando a memória alocada
void quadtree_destroy();

// Insere um nó na quadtree com a chave especificada
void quadtree_insert(nodekey_t k);

// Busca um nó na quadtree pelo identificador, a partir das coordenadas (x, y)
nodeaddr_t quadtree_search(char* idend, double x, double y);

// Encontra os k nós mais próximos das coordenadas (x, y) e armazena os resultados no vetor result
void quadtree_knn(double x, double y, long k, Neighbor* result);

// Exporta a estrutura da quadtree para um arquivo
void export_quadtree(const char* filename);

// Exporta um nó específico da quadtree para um arquivo
void export_node(nodeaddr_t addr, FILE* file);

#endif