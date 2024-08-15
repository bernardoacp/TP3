#include "qnode.h"

// Variáveis encapsuladas que mantêm o vetor de nós
QuadTreeNode* nodevet = NULL; // Vetor de nós da QuadTree
Boundary boundary = INVALIDBOUNDARY; // Limites padrão inválidos
long nodevetsz = 0; // Tamanho do vetor de nós
long nodesallocated = 0; // Número de nós alocados
long firstavail = INVALIDADDR; // Primeiro endereço disponível

// Definição de um nó inválido
#define INVALIDNODE {boundary, INVALIDKEY, INVALIDADDR, INVALIDADDR, INVALIDADDR, INVALIDADDR}

// Função auxiliar para verificar se um nó é inválido
static bool is_invalid_node(QuadTreeNode* node) {
    QuadTreeNode invalid_node = INVALIDNODE;
    return memcmp(node, &invalid_node, sizeof(QuadTreeNode)) == 0;
}

// Função para resetar um nó, removendo qualquer informação de uso anterior
void node_reset(QuadTreeNode* pn) {
    pn->boundary = boundary;
    pn->key = INVALIDKEY;
    pn->ne = INVALIDADDR;
    pn->nw = INVALIDADDR;
    pn->se = INVALIDADDR;
    pn->sw = INVALIDADDR;
}

// Função para copiar o conteúdo de um nó src para um nó dst
void node_copy(QuadTreeNode* dst, QuadTreeNode* src) {
    dst->key = src->key;
    dst->boundary = src->boundary;
    dst->nw = src->nw;
    dst->ne = src->ne;
    dst->sw = src->sw;
    dst->se = src->se;
}

// Função para inicializar um vetor de nós que conterá no máximo numnodes
long node_initialize(long numnodes, Boundary qt_boundary) {
    // Aloca o vetor de nós
    nodevet = (QuadTreeNode*) malloc(numnodes * sizeof(QuadTreeNode));
    if (nodevet == NULL) {
        fprintf(stderr,"node_initialize: could not allocate nodevet\n");
        return 0;
    }
    // Inicializa os limites
    boundary = qt_boundary;
    // Inicializa o tamanho do vetor de nós
    nodevetsz = numnodes;
    // Cria a cadeia de nós disponíveis como uma lista encadeada
    for (long i = 0; i < nodevetsz; i++) {
        node_reset(&(nodevet[i]));
        nodevet[i].nw = (nodeaddr_t) i + 1;
    }
    // Último nó na cadeia
    nodevet[nodevetsz - 1].nw = INVALIDADDR;
    // Nó 0 é o primeiro disponível
    firstavail = (nodeaddr_t) 0;
    return numnodes;
}

// Função para criar um nó a partir de pn
nodeaddr_t node_create(QuadTreeNode* pn) {
    nodeaddr_t ret;
    // Remove um nó da cadeia de disponíveis, atualiza os controles e copia pn 
	// para ele
    ret = firstavail;
    firstavail = nodevet[ret].nw;
    nodesallocated++;
    node_copy(&(nodevet[ret]), pn);
    return ret;
}

// Função para deletar virtualmente um nó, tornando-o disponível para futura 
// criação
void node_delete(nodeaddr_t ad) {
    // Verifica se o endereço é válido
    if (ad < 0 || ad >= nodevetsz) {
        fprintf(stderr,"node_delete: address out of range\n");
        return;
    }
    if (is_invalid_node(&(nodevet[ad]))) {
        fprintf(stderr,"node_delete: node already deleted\n");
    }
    // Apenas reseta e adiciona à frente da lista de disponíveis
    node_reset(&(nodevet[ad]));
    nodevet[ad].nw = firstavail;
    firstavail = ad;
    nodesallocated--;
}

// Função para recuperar um nó do vetor a partir do endereço ad e copiá-lo para 
// pn
void node_get(nodeaddr_t ad, QuadTreeNode* pn) {
    // Verifica se o endereço é válido
    if (ad < 0 || ad >= nodevetsz) {
        fprintf(stderr,"node_get: address out of range\n");
        pn->key = INVALIDKEY;
        return;
    }
    if (is_invalid_node(&(nodevet[ad]))) {
        fprintf(stderr,"node_get: node is invalid\n");
    }
    node_copy(pn, &(nodevet[ad]));
}

// Função para armazenar um nó no vetor a partir do endereço ad e copiá-lo de pn
void node_put(nodeaddr_t ad, QuadTreeNode* pn) {
    // Verifica se o endereço é válido
    if (ad < 0 || ad >= nodevetsz) {
        fprintf(stderr,"node_put: address out of range\n");
        return;
    }
    node_copy(&(nodevet[ad]), pn);
}

// Função para imprimir nodevet para fins de depuração
void node_dump(int ad, int level) {
    // Verifica se o endereço é válido
    if (ad < 0 || ad >= nodevetsz) {
        fprintf(stderr,"node_dump: address out of range\n");
        return;
    }
    // Insere espaços de nível para fins de indentação
    for (int i = 0; i < level; i++) printf(" ");
    // Imprime as informações do nó
    printf("(%f %f) key %ld nw %ld ne %ld sw %ld se\n",
            nodevet[ad].key.x, nodevet[ad].key.y,
            nodevet[ad].nw, nodevet[ad].ne, nodevet[ad].sw, nodevet[ad].se);
}

// Função para imprimir todo o vetor
void node_dumpvet() {
    // Informações gerais
    printf("sz %ld alloc %ld 1stavail %ld\n",
            nodevetsz, nodesallocated, firstavail);
    // Imprime cada entrada do vetor
    for (int i = 0; i < nodevetsz; i++) {
        printf("(%f %f) key %ld nw %ld ne %ld sw %ld se\n",
                nodevet[i].key.x, nodevet[i].key.y,
                nodevet[i].nw, nodevet[i].ne, nodevet[i].sw, nodevet[i].se);
    }
}

// Função para destruir o vetor de nós, liberando a memória alocada
void node_destroy() {
    free(nodevet);
    nodevet = NULL;
    nodevetsz = 0;
    nodesallocated = 0;
    firstavail = INVALIDADDR;
}