#include "quadtree.h"

// A raiz da quadtree é encapsulada
nodeaddr_t root = INVALIDADDR; // Endereço inválido inicial para a raiz
long numpoints = 0; // Número de pontos na quadtree

// Funções privadas
static double euclidean_dist(double x1, double y1, double x2, double y2);
static int cmpknn(const void* a, const void* b);
static void subdivide(nodeaddr_t ad);
static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr);
static nodeaddr_t quadtree_search_rec(nodeaddr_t curr, char* idend, double x, double y);
static void quadtree_knn_rec(nodeaddr_t curr, double x, double y, long k, Heap* heap);

void quadtree_create(long numnodes, Boundary qt_boundary) {
    // Inicializa o vetor que conterá a quadtree
    node_initialize(numnodes, qt_boundary);
}

void quadtree_destroy() {
    // Primeiro desaloca o vetor que contém a quadtree
    node_destroy();
    // Reseta a raiz da quadtree
    root = INVALIDADDR;
}

// Função auxiliar para subdividir um nó da quadtree em quatro quadrantes
static void subdivide(nodeaddr_t ad)
{
    QuadTreeNode curr;
    // Recupera o nó atual da quadtree a partir do endereço fornecido
    node_get(ad, &curr);

    // Obtém os limites do nó atual
    double x_min = curr.boundary.x_min;
    double x_max = curr.boundary.x_max;
    double y_min = curr.boundary.y_min;
    double y_max = curr.boundary.y_max;

    // Define os limites dos quatro novos quadrantes
    Boundary nw = (Boundary) {x_min, (x_min+x_max)/2, (y_min+y_max)/2, y_max};
    Boundary ne = (Boundary) {(x_min+x_max)/2, x_max, (y_min+y_max)/2, y_max};
    Boundary sw = (Boundary) {x_min, (x_min+x_max)/2, y_min, (y_min+y_max)/2};
    Boundary se = (Boundary) {(x_min+x_max)/2, x_max, y_min, (y_min+y_max)/2};
    
    QuadTreeNode aux;
    // Reseta o nó auxiliar para reutilização
    node_reset(&aux);

    // Cria o quadrante noroeste e atualiza o nó atual
    aux.boundary = nw;
    curr.nw = node_create(&aux);

    // Reseta o nó auxiliar e cria o quadrante nordeste
    node_reset(&aux);
    aux.boundary = ne;
    curr.ne = node_create(&aux);

    // Reseta o nó auxiliar e cria o quadrante sudoeste
    node_reset(&aux);
    aux.boundary = sw;
    curr.sw = node_create(&aux);

    // Reseta o nó auxiliar e cria o quadrante sudeste
    node_reset(&aux);
    aux.boundary = se;
    curr.se = node_create(&aux);

    // Atualiza o nó atual na quadtree com os novos quadrantes
    node_put(ad, &curr);
}

// Função auxiliar recursiva para inserir um nó na quadtree
static void quadtree_insert_rec(nodekey_t key, nodeaddr_t curr)
{
    QuadTreeNode curr_node;
    // Recupera o nó atual da quadtree a partir do endereço fornecido
    node_get(curr, &curr_node);

    // Verifica se o ponto está dentro dos limites do nó atual
    if (!boundary_contains(&curr_node.boundary, key.x, key.y)) {
        return; // Se não estiver, retorna 
    }

    // Verifica se o nó atual está vazio e não subdividido
    if (curr_node.key.idend == NULL && curr_node.nw == INVALIDADDR) {
        // Insere a chave no nó atual
        curr_node.key = key;
        node_put(curr, &curr_node);
        return;
    }

    // Se o nó atual não estiver subdividido, subdivide-o
    if (curr_node.nw == INVALIDADDR) {
        subdivide(curr);
        // Atualiza o nó atual após a subdivisão
        node_get(curr, &curr_node);
    }

    // Insere recursivamente a chave nos quadrantes apropriados
    quadtree_insert_rec(key, curr_node.nw);
    quadtree_insert_rec(key, curr_node.ne);
    quadtree_insert_rec(key, curr_node.sw);
    quadtree_insert_rec(key, curr_node.se);
}

// Função para inserir um nó na quadtree
void quadtree_insert(nodekey_t key)
{
    QuadTreeNode aux;
    // Reseta o nó auxiliar para reutilização
    node_reset(&aux);
    aux.key = key;

    // Se a raiz da quadtree estiver vazia, cria a raiz
    if (root == INVALIDADDR) {
        root = node_create(&aux);
        node_put(root, &aux);
        return;
    }

    // Insere a chave na quadtree a partir da raiz
    quadtree_insert_rec(key, root);
    numpoints++; // Incrementa o número de pontos na quadtree
}

// Função auxiliar recursiva para buscar um nó na quadtree pelo identificador e coordenadas (x, y)
static nodeaddr_t quadtree_search_rec(nodeaddr_t curr, char* idend, double x, double y)
{
    QuadTreeNode curr_node;
    // Recupera o nó atual da quadtree a partir do endereço fornecido
    node_get(curr, &curr_node);

    // Verifica se o id do nó atual corresponde ao id procurado
    if (!strcmp(curr_node.key.idend, idend)) {
        return curr; // Se corresponder, retorna o endereço do nó atual
    }

    // Verifica se o nó atual não possui subdivisões (é uma folha)
    if (curr_node.nw == INVALIDADDR) {
        return -1; // Se for uma folha, retorna -1 indicando que o nó não foi encontrado
    }

    QuadTreeNode aux;
    // Verifica em qual quadrante o ponto (x, y) está contido e chama a função recursivamente
    node_get(curr_node.nw, &aux);
    if (boundary_contains(&aux.boundary, x, y)) {
        return quadtree_search_rec(curr_node.nw, idend, x, y);
    }

    node_get(curr_node.ne, &aux);
    if (boundary_contains(&aux.boundary, x, y)) {
        return quadtree_search_rec(curr_node.ne, idend, x, y);
    }

    node_get(curr_node.sw, &aux);
    if (boundary_contains(&aux.boundary, x, y)) {
        return quadtree_search_rec(curr_node.sw, idend, x, y);
    }

    node_get(curr_node.se, &aux);
    if (boundary_contains(&aux.boundary, x, y)) {
        return quadtree_search_rec(curr_node.se, idend, x, y);
    }

    // Se o id não estiver contido em nenhum quadrante, retorna -1
    return -1;
}

nodeaddr_t quadtree_search(char* idend, double x, double y)
{
    // Verifica se a quadtree está vazia
    if (root == INVALIDADDR) {
        fprintf(stderr, "quadtree_search: tree empty\n");
        return INVALIDADDR; // Se estiver vazia, retorna um endereço inválido
    }
    // Chama a função recursiva para buscar o nó a partir da raiz
    return quadtree_search_rec(root, idend, x, y);
}

// Calcula a distancia euclidiana entre (x1,y1) e (x2,y2)
static double euclidean_dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0); 
}

// Função recursiva para encontrar os k nós mais próximos na quadtree
static void quadtree_knn_rec(nodeaddr_t curr, double x, double y, long k, Heap* heap)
{	
    // Verifica se o nó atual é inválido
    if (curr == INVALIDADDR) {
        return;
    }

    QuadTreeNode curr_node;
    // Recupera o nó atual da quadtree a partir do endereço fornecido
    node_get(curr, &curr_node);

    // Verifica se o nó atual está vazio
    if (curr_node.key.idend == NULL) {
        return;
    }
    
    // Calcula a distância euclidiana entre o ponto (x, y) e o nó atual
    double dist = euclidean_dist(x, y, curr_node.key.x, curr_node.key.y);
    
    // Se o heap ainda não estiver cheio e o nó atual estiver ativo, adiciona o nó ao heap
    if (heap->size < k && curr_node.key.ativo) {
        heap_push(heap, (Neighbor) {curr, dist});
    }
    // Se a distância do nó atual for menor que a maior distância no heap e o nó atual estiver ativo, substitui o nó no heap
    else if (dist < heap->neighbors[0].dist && curr_node.key.ativo) {
        heap_pop(heap);
        heap_push(heap, (Neighbor) {curr, dist});
    }

    QuadTreeNode aux;
    // Verifica se o quadrante noroeste pode conter um ponto mais próximo e chama a função recursivamente
    if (curr_node.nw != INVALIDADDR) {
        node_get(curr_node.nw, &aux);
        if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
            quadtree_knn_rec(curr_node.nw, x, y, k, heap);
        }
    }
    // Verifica se o quadrante nordeste pode conter um ponto mais próximo e chama a função recursivamente
    if (curr_node.ne != INVALIDADDR) {
        node_get(curr_node.ne, &aux);
        if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
            quadtree_knn_rec(curr_node.ne, x, y, k, heap);
        }
    }
    // Verifica se o quadrante sudoeste pode conter um ponto mais próximo e chama a função recursivamente
    if (curr_node.sw != INVALIDADDR) {
        node_get(curr_node.sw, &aux);
        if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
            quadtree_knn_rec(curr_node.sw, x, y, k, heap);
        }
    }
    // Verifica se o quadrante sudeste pode conter um ponto mais próximo e chama a função recursivamente
    if (curr_node.se != INVALIDADDR) {
        node_get(curr_node.se, &aux);
        if (heap->size < k || can_contain_closer_point(&aux.boundary, x, y, heap->neighbors[0].dist)) {
            quadtree_knn_rec(curr_node.se, x, y, k, heap);
        }
    }
}

// Função de comparação para o k-NN (k-nearest neighbors)
static int cmpknn(const void* a, const void* b) {
    Neighbor* k1 = (Neighbor*) a;
    Neighbor* k2 = (Neighbor*) b;
    // Compara as distâncias dos vizinhos
    if (k1->dist > k2->dist) return 1;
    else if (k1->dist < k2->dist) return -1;
    else return 0;
}

void quadtree_knn(double x, double y, long k, Neighbor* result)
{
    // Verifica se a quadtree está vazia
    if (root == INVALIDADDR) {
        fprintf(stderr,"quadtree_search: tree empty\n");
        return;
    }
    // Inicializa um heap para armazenar os k vizinhos mais próximos
    Heap* heap = heap_initialize(k);
    // Chama a função recursiva para encontrar os k vizinhos mais próximos a partir da raiz
    quadtree_knn_rec(root, x, y, k, heap);

    // Ordena os vizinhos encontrados pela distância
    qsort(heap->neighbors, k, sizeof(Neighbor), cmpknn);
    
    // Copia os vizinhos ordenados para o array de resultados
	memcpy(result, heap->neighbors, k * sizeof(Neighbor));
}

void export_node(nodeaddr_t addr, FILE* file) {
    // Verifica se o endereço do nó é inválido
    if (addr == INVALIDADDR) return;

    QuadTreeNode node;
    // Recupera o nó atual da quadtree a partir do endereço fornecido
    node_get(addr, &node);

    // Escreve os limites do nó atual no arquivo
    fprintf(file, "%f %f %f %f\n", node.boundary.x_min, node.boundary.x_max, node.boundary.y_min, node.boundary.y_max);

    // Exporta recursivamente os nós filhos
    export_node(node.nw, file);
    export_node(node.ne, file);
    export_node(node.sw, file);
    export_node(node.se, file);
}

void export_quadtree(const char* filename) {
    // Abre o arquivo para escrita
    FILE* file = fopen(filename, "w");
    if (!file) {
        // Se não for possível abrir o arquivo, imprime uma mensagem de erro
        fprintf(stderr, "export_quadtree: could not open file for writing\n");
        return;
    }

    // Inicia a exportação a partir da raiz da quadtree
    export_node(root, file);

    // Fecha o arquivo após a exportação
    fclose(file);
}