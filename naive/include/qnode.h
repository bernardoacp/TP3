#ifndef QNODE_H
#define QNODE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "boundary.h"

// struct that contains the information about the recharging locations
typedef struct {
	char* idend;
	long id_logrado;
	char* sigla_tipo;
	char* nome_logra;
	int numero_imo;
	char* nome_bairr;
	char* nome_regio;
	int cep;
	double x;
	double y;
	int ativo;
} Item;

// there are no pointers in a vectorized implementation, just vector indices
typedef Item nodekey_t;
typedef long nodeaddr_t;

typedef struct {
    Boundary boundary;
    nodekey_t key;
    nodeaddr_t nw;
    nodeaddr_t ne;
    nodeaddr_t sw;
    nodeaddr_t se;
} QuadTreeNode;

#define INVALIDADDR -2
#define INVALIDKEY (nodekey_t) {NULL, 0, NULL, NULL, 0, NULL, NULL, 0, 0, 0}
#define INVALIDBOUNDARY (Boundary) {0, 0, 0, 0}

long node_initialize(long numnodes, Boundary qt_boundary);
nodeaddr_t node_create(QuadTreeNode* pn);
void node_delete(nodeaddr_t ad);
void node_get(nodeaddr_t ad, QuadTreeNode* pn);
void node_put(nodeaddr_t ad, QuadTreeNode* pn);
void node_dump(int ad, int level);
void node_dumpvet();
void node_reset(QuadTreeNode* pn);
void node_copy(QuadTreeNode* dst, QuadTreeNode* src);
void node_destroy();

#endif