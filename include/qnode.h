#ifndef QNODE_H
#define QNODE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "boundary.h"

// Estrutura que contém as informações sobre os locais de recarga
typedef struct {
    char* idend;        // Identificador do endereco
    long id_logrado;    // Identificador do logradouro
    char* sigla_tipo;   // Sigla do tipo de logradouro
    char* nome_logra;   // Nome do logradouro
    int numero_imo;     // Número do imóvel
    char* nome_bairr;   // Nome do bairro
    char* nome_regio;   // Nome da região
    int cep;            // Código de Endereçamento Postal (CEP)
    double x;           // Coordenada x 
    double y;           // Coordenada y 
    bool ativo;         // Status de atividade do local de recarga
} Item;

// Não há ponteiros em uma implementação vetorizada, apenas índices de vetor
typedef Item nodekey_t;  // Tipo de chave do nó
typedef long nodeaddr_t; // Tipo de endereço do nó

// Estrutura que representa um nó da QuadTree
typedef struct {
    Boundary boundary;  // Limites do nó
    nodekey_t key;      // Chave do nó
    nodeaddr_t nw;      // Endereço do quadrante noroeste
    nodeaddr_t ne;      // Endereço do quadrante nordeste
    nodeaddr_t sw;      // Endereço do quadrante sudoeste
    nodeaddr_t se;      // Endereço do quadrante sudeste
} QuadTreeNode;

// Definições de endereços e chaves inválidas
#define INVALIDADDR -2
#define INVALIDKEY (nodekey_t) {NULL, 0, NULL, NULL, 0, NULL, NULL, 0, 0, 0}
#define INVALIDBOUNDARY (Boundary) {0, 0, 0, 0}

// Inicializa o vetor de nós da QuadTree com um número especificado de nós e
// um limite inicial
long node_initialize(long numnodes, Boundary qt_boundary);

// Cria um novo nó na QuadTree e retorna seu endereço
nodeaddr_t node_create(QuadTreeNode* pn);

// Deleta um nó da QuadTree a partir de seu endereço
void node_delete(nodeaddr_t ad);

// Recupera um nó da QuadTree a partir de seu endereço
void node_get(nodeaddr_t ad, QuadTreeNode* pn);

// Atualiza um nó da QuadTree a partir de seu endereço
void node_put(nodeaddr_t ad, QuadTreeNode* pn);

// Imprime a estrutura da QuadTree a partir de um endereço e nível
void node_dump(int ad, int level);

// Imprime o vetor de nós da QuadTree
void node_dumpvet();

// Reseta um nó da QuadTree
void node_reset(QuadTreeNode* pn);

// Copia os dados de um nó da QuadTree para outro
void node_copy(QuadTreeNode* dst, QuadTreeNode* src);

// Destroi o vetor de nós da QuadTree, liberando a memória alocada
void node_destroy();

#endif 