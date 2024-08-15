// biuaidi
// Authors: Wagner Meira Jr. 
//		    Bernardo Alves Costa Pereira	
// Version history:
//    1.0 - 21/07/2024
//    1.1 - 07/08/2024
//	  2.0 - 15/08/2024	
//
// Usage: 
// biuaidinaive x y
//    x - geographical x coordinate of current location
//    y - geographical y coordinate of current location
// 
// Output:
//    10 closest recharging locations
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "quadtree.h"
#include "qnode.h"
#include "heap.h"
#include "boundary.h"

// Variável global para armazenar o número de pontos de recarga
int nrecharge = 0;

// Função para imprimir as informações do ponto de recarga
// Recebe a posição do nó na quadtree como argumento
void printrecharge(int pos) {
	QuadTreeNode aux;
	// Recupera as informações do nó na quadtree
	node_get(pos, &aux);
	// Imprime os detalhes do ponto de recarga
	printf("%s %s, %d, %s, %s, %d", aux.key.sigla_tipo,
				aux.key.nome_logra, aux.key.numero_imo,
				aux.key.nome_bairr, aux.key.nome_regio,
				aux.key.cep);
}

// Função para imprimir um mapa ilustrativo usando gnuplot
// Recebe um array de estruturas Neighbor, o número máximo de vizinhos (kmax),
// o número de pontos de recarga (nrec) e as coordenadas (tx, ty) como argumentos
void printmap(Neighbor* kvet, int kmax, int nrec, double tx, double ty) {
	FILE *out1, *out2;

	// Exporta os dados da quadtree para um arquivo
    export_quadtree("plot/quadtree.gpdat");

    // Cria um script gnuplot para gerar o mapa
    // O script será salvo em "plot/out.gp"
	out1 = fopen("plot/out.gp","wt");
	fprintf(out1,"set term postscript eps\n");
	fprintf(out1,"set output \"plot/out.eps\"\n");
	fprintf(out1,"set size square\n");
	fprintf(out1,"set key bottom right\n");
	fprintf(out1,"set title \"BiUaiDi Recharging Stations\"\n");
	fprintf(out1,"set xlabel \"\"\n");
	fprintf(out1,"set ylabel \"\"\n");
	fprintf(out1,"unset xtics \n");
	fprintf(out1,"unset ytics \n");
	fprintf(out1,"plot \"plot/origin.gpdat\" t \"Your location\" pt 4 ps 2, \"plot/recharge.gpdat\" t \"\", \"plot/suggested.gpdat\" t \"Nearest stations\" pt 7 ps 2, \"plot/deactivated.gpdat\" t \"Deactivated stations\" pt 2 ps 1, \"plot/quadtree.gpdat\" u (($1+$2)/2):(($3+$4)/2):(($2-$1)/2):(($4-$3)/2) w boxxy t \"\"\n");
	fclose(out1);

	// Ponto de origem, apenas um par de coordenadas x, y
	out1 = fopen("plot/origin.gpdat","wt");
	fprintf(out1,"%f %f\n",tx, ty);
	fclose(out1);

	// Pontos de recarga ativos
	out1 = fopen("plot/recharge.gpdat","wt");
	// Pontos de recarga desativados
    out2 = fopen("plot/deactivated.gpdat","wt");
	QuadTreeNode aux;
	for (int i = 0; i < nrec; i++) {
		node_get(i, &aux);
		if (aux.key.idend == NULL) {
			continue;
		}
		if (!aux.key.ativo) {
			fprintf(out2,"%f %f\n", aux.key.x, aux.key.y);
			continue;
		}
		fprintf(out1,"%f %f\n", aux.key.x, aux.key.y);
	}
	fclose(out1);
	fclose(out2);

	// Os pontos de recarga mais próximos
	out1 = fopen("plot/suggested.gpdat","wt");
	for (int i = 0; i < kmax; i++) {
		node_get(kvet[i].addr, &aux);
		fprintf(out1,"%f %f\n", aux.key.x, aux.key.y);
	}
	fclose(out1);
}

// Estrutura para armazenar informações de consulta - para ativação e desativação de pontos de recarga
typedef struct {
	char* idend;
	double x;
	double y;
} Consulta;

Consulta* vet;

// Função de busca binária para encontrar um ponto de recarga pelo ID, de modo a permitir a busca na quadtree pelas coordenadas
Consulta* bin_search(char* idend, Consulta* vet, int n) {
	int l = 0, r = n - 1;
	while (l <= r) {
		int m = l + (r - l) / 2;
		if (!strcmp(vet[m].idend, idend)) {
			return &vet[m];
		}
		if (strcmp(vet[m].idend, idend) < 0) {
			l = m + 1;
		}
		else {
			r = m - 1;
		}
	}
	return NULL;
}

// Função de comparação para ordenar pontos de recarga pelo ID
int cmp_idend(const void* a, const void* b) {
	Consulta* k1 = (Consulta*) a;
	Consulta* k2 = (Consulta*) b;
	if (strcmp(k1->idend, k2->idend) > 0) return 1;
	else if (strcmp(k1->idend, k2->idend) < 0) return -1;
	else return 0;
}

// Função para carregar os pontos de recarga a partir de um arquivo
void load_recharge_stations(const char* filename) 
{
    // Abre o arquivo para leitura
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        // Se o arquivo não puder ser aberto, imprime uma mensagem de erro e encerra o programa
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
        exit(1);
    }

    // Lê o arquivo e popula a quadtree
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        // Se não for possível ler o número de pontos de recarga, imprime uma mensagem de erro e encerra o programa
        fprintf(stderr, "Erro: nao foi possivel ler o numero de pontos de recarga\n");
        fclose(file);      
        exit(1);
    }	
    
    // Remove o caractere de nova linha, se presente
    buffer[strcspn(buffer, "\n")] = 0;
    // Converte a string lida para um número inteiro e armazena em nrecharge
    sscanf(buffer, "%d", &nrecharge);

    // Cria a quadtree com a capacidade calculada e os limites especificados
	// (extraidos do arquivo que contem os pontos de recarga em potencial)
    quadtree_create(4 * nrecharge - 1, (Boundary) {598017.313632323, 619122.989979841, 7785041.75619417, 7812836.09085508});
    // Aloca memória para o vetor de consultas
    vet = malloc(nrecharge * sizeof(Consulta));

    Item aux;
    for (int i = 0; i < nrecharge; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            // Se não for possível ler um ponto de recarga, imprime uma mensagem de erro e encerra o programa
            fprintf(stderr, "Erro: nao foi possivel ler o ponto de recarga %d\n", i);
            fclose(file);
            exit(1);
        }
        // Remove o caractere de nova linha, se presente
        buffer[strcspn(buffer, "\n")] = 0;

        // Faz o parsing da linha
        char* token = strtok(buffer, ";");
        aux.idend = strdup(token);
        vet[i].idend = strdup(token);
        
        token = strtok(NULL, ";");
        aux.id_logrado = atol(token);

        token = strtok(NULL, ";");
        aux.sigla_tipo = strdup(token);

        token = strtok(NULL, ";");
        aux.nome_logra = strdup(token);

        token = strtok(NULL, ";");
        aux.numero_imo = atoi(token);

        token = strtok(NULL, ";");
        aux.nome_bairr = strdup(token);

        token = strtok(NULL, ";");
        aux.nome_regio = strdup(token);

        token = strtok(NULL, ";");
        aux.cep = atoi(token);

        token = strtok(NULL, ";");
        aux.x = atof(token);
        vet[i].x = atof(token);

        token = strtok(NULL, ";");
        aux.y = atof(token);
        vet[i].y = atof(token);

        // Marca o ponto de recarga como ativo
        aux.ativo = true;
        
        // Insere o ponto de recarga na quadtree
        quadtree_insert(aux);
    }
    // Fecha o arquivo
    fclose(file);

    // Ordena o vetor de consultas pelo ID
    qsort(vet, nrecharge, sizeof(Consulta), cmp_idend);
}

// Função para ativar um ponto de recarga
void activate_recharge_station(char* id) 
{
    // Busca binária para encontrar o ponto de recarga pelo ID
    Consulta* query = bin_search(id, vet, nrecharge);
    if (query == NULL) {
        // Se o ponto de recarga não for encontrado, imprime uma mensagem de erro e retorna
        fprintf(stderr, "Ponto de recarga %s não encontrado.\n", id);
        return;
    }
    
    // Busca na quadtree pelo endereço do ponto de recarga
    nodeaddr_t addr = quadtree_search(id, query->x, query->y);
    if (addr == INVALIDADDR) {
        // Se o endereço não for encontrado, imprime uma mensagem de erro e retorna
        fprintf(stderr, "Ponto de recarga %s não encontrado.\n", id);
        return;
    }

    QuadTreeNode node;
    // Recupera o nó da quadtree
    node_get(addr, &node);
    if (node.key.ativo) {
        // Se o ponto de recarga já estiver ativo, imprime uma mensagem e retorna
        printf("Ponto de recarga %s já estava ativo.\n", id);
        return;
    }
    // Ativa o ponto de recarga
    node.key.ativo = true;
    // Atualiza o nó na quadtree
    node_put(addr, &node);
    printf("Ponto de recarga %s ativado.\n", id);
}

// Função para desativar um ponto de recarga
void deactivate_recharge_station(char* id) 
{
    // Busca binária para encontrar o ponto de recarga pelo ID
    Consulta* query = bin_search(id, vet, nrecharge);
    if (query == NULL) {
        // Se o ponto de recarga não for encontrado, imprime uma mensagem de erro e retorna
        fprintf(stderr, "Ponto de recarga %s não encontrado.\n", id);
        return;
    }
    
    // Busca na quadtree pelo endereço do ponto de recarga
    nodeaddr_t addr = quadtree_search(id, query->x, query->y);
    if (addr == INVALIDADDR) {
        // Se o endereço não for encontrado, imprime uma mensagem de erro e retorna
        fprintf(stderr, "Ponto de recarga %s não encontrado.\n", id);
        return;
    }

    QuadTreeNode node;
    // Recupera o nó da quadtree
    node_get(addr, &node);
    if (!node.key.ativo) {
        // Se o ponto de recarga já estiver desativado, imprime uma mensagem e retorna
        printf("Ponto de recarga %s já estava desativado.\n", id);
        return;
    }
    // Desativa o ponto de recarga
    node.key.ativo = false;
    // Atualiza o nó na quadtree
    node_put(addr, &node);
    printf("Ponto de recarga %s desativado.\n", id);
}

// Função para encontrar os n pontos de recarga mais próximos
void closest_recharge_stations(double x, double y, long n) 
{
    // Array para armazenar os resultados dos pontos de recarga mais próximos
    Neighbor result[n];
    
    // Encontra os n pontos de recarga mais próximos usando a quadtree
    quadtree_k_nearest(x, y, n, result);
    
    // Imprime os n pontos de recarga mais próximos
    QuadTreeNode aux;
    for (int i = 0; i < n; i++) {
        printrecharge(result[i].addr);
        printf(" (%.3f)\n", result[i].dist);
    }
}

// Função para ler e executar comandos a partir de um arquivo
void read_commands(const char* filename) 
{
    // Abre o arquivo para leitura
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        // Se o arquivo não puder ser aberto, imprime uma mensagem de erro e encerra o programa
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
        exit(1);
    }

    char buffer[1024];
    // Lê a primeira linha do arquivo para obter o número de comandos
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        // Se não for possível ler o número de comandos, imprime uma mensagem de erro e encerra o programa
        fprintf(stderr, "Erro: nao foi possivel ler o numero de comandos\n");
        fclose(file);
        exit(1);
    }
    int num_commands;
    // Converte a string lida para um número inteiro e armazena em num_commands
    sscanf(buffer, "%d", &num_commands);

    // Itera sobre cada comando no arquivo
    for (int i = 0; i <= num_commands; i++) {
        // Lê a linha do comando
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            // Se não for possível ler o comando, imprime uma mensagem de erro e encerra o programa
            fprintf(stderr, "Erro: nao foi possivel ler o comando %d\n", i);
            fclose(file);
            exit(1);
        }
        // Remove o caractere de nova linha, se presente
        buffer[strcspn(buffer, "\n")] = 0;

        char operation;
        char id[20];

        double x, y;
        long n;
        
        // Verifica o tipo de operação a ser realizada
        switch (buffer[0]) {
        case 'A':
            // Ativar ponto de recarga
            sscanf(buffer, "%c %s", &operation, id);
            printf("%c %s\n", operation, id);

            // Chama a função para ativar o ponto de recarga
            activate_recharge_station(id);
            
            break;
        case 'D':
            // Desativar ponto de recarga
            sscanf(buffer, "%c %s", &operation, id);
            printf("%c %s\n", operation, id);

            // Chama a função para desativar o ponto de recarga
            deactivate_recharge_station(id);
            
            break;
        case 'C':
            // Encontrar n pontos de recarga mais próximos
            sscanf(buffer, "%c %lf %lf %ld", &operation, &x, &y, &n);
            printf("%c %lf %lf %ld\n", operation, x, y, n);

            // Verifica se o número de pontos de recarga solicitados é maior que o disponível
            if (n > nrecharge) {
                fprintf(stderr, "Número de pontos de recarga solicitados maior que o número de pontos de recarga disponíveis.\n");
                break;
            }
            // Chama a função para encontrar os pontos de recarga mais próximos
            closest_recharge_stations(x, y, n);
            
            break;
        default:
            // Comando inválido
            fprintf(stderr, "Comando inválido.\n");
            break;
        }
    }
}

int main(int argc, char** argv) 
{	
    // Carrega os pontos de recarga a partir do arquivo "geracarga.base"
    load_recharge_stations("geracarga.base");
    // Lê os comandos a partir do arquivo "geracarga.ev"
    read_commands("geracarga.ev");

    // Destroi a quadtree
    quadtree_destroy();

    return 0;
}
