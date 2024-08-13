// biuaidinaive
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
//    1.1 - 07/08/2024
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
#include "heap.h"
#include "boundary.h"
#include "qnode.h"
#include "quadtree.h"

int nrecharge = 0;

void clkDiff(struct timespec t1, struct timespec t2, struct timespec * res) {
    // Descricao: calcula a diferenca entre t2 e t1, que e armazenada em res
    // Entrada: t1, t2
    // Saida: res
    if (t2.tv_nsec < t1.tv_nsec){
        // ajuste necessario, utilizando um segundo de tv_sec
        res->tv_nsec = 1000000000 + t2.tv_nsec - t1.tv_nsec;
        res->tv_sec = t2.tv_sec - t1.tv_sec - 1;
    } else {
        // nao e necessario ajuste
        res->tv_nsec = t2.tv_nsec - t1.tv_nsec;
        res->tv_sec = t2.tv_sec - t1.tv_sec;
    }
}

// print the recharge location information
void printrecharge(int pos) {
	QuadTreeNode aux;
	node_get(pos, &aux);
	printf("%s %s, %d, %s, %s, %d", aux.key.sigla_tipo,
				aux.key.nome_logra, aux.key.numero_imo,
				aux.key.nome_bairr, aux.key.nome_regio,
				aux.key.cep);
}

// print illustrative map using gnuplot
/*
void printmap(ptr_knn_t kvet, int kmax, ptr_addr_t rvet, int nrec, double tx, double ty) {
	FILE *out1, *out2;

	// gnuplot script. to obtain the actual map, it is necessary to run:
	// gnuplot out.gp
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
	fprintf(out1,"plot \"plot/origin.gpdat\" t \"Your location\" pt 4 ps 2, \"plot/recharge.gpdat\" t \"\", \"plot/suggested.gpdat\" t \"Nearest stations\" pt 7 ps 2, \"plot/deactivated.gpdat\" t \"Deactivated stations\" pt 2 ps 1\n");
	fclose(out1);

	// origin point, just one pair of x, y coordinates
	out1 = fopen("plot/origin.gpdat","wt");
	fprintf(out1,"%f %f\n",tx, ty);
	fclose(out1);

	// active recharging locations
	out1 = fopen("plot/recharge.gpdat","wt");
	//  deactivated recharging locations
    out2 = fopen("plot/deactivated.gpdat","wt");
	for (int i=0; i<nrec; i++) {
		if (!rvet[i].ativo) {
			fprintf(out2,"%f %f\n",rvet[i].x, rvet[i].y);
			continue;
		}
		fprintf(out1,"%f %f\n",rvet[i].x, rvet[i].y);
	}
	fclose(out1);
	fclose(out2);

	// the nearest recharging locations
	out1 = fopen("plot/suggested.gpdat","wt");
	for (int i=0; i<kmax; i++) {
		fprintf(out1,"%f %f\n",rvet[kvet[i].id].x, rvet[kvet[i].id].y);
	}
	fclose(out1);
}

*/

void load_recharge_stations(const char* filename) 
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
		exit(1);
	}
	char ch;

	// count the number of lines in the file
	while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            nrecharge++;
        }
    }
	
	quadtree_create(nrecharge, (Boundary) {600000, 700000, 7000000, 8000000});

	// read the file and populate the rechargevet vector
	rewind(file);
	char buffer[1024];
	int i = 0;
	Item aux;
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;

		// parse the line
		char* token = strtok(buffer, ",");
		aux.idend = strdup(token);
		
		token = strtok(NULL, ",");
		aux.id_logrado = atol(token);

		token = strtok(NULL, ",");
		aux.sigla_tipo = strdup(token);

		token = strtok(NULL, ",");
		aux.nome_logra = strdup(token);

		token = strtok(NULL, ",");
		aux.numero_imo = atoi(token);

		token = strtok(NULL, ",");
		aux.nome_bairr = strdup(token);

		token = strtok(NULL, ",");
		aux.nome_regio = strdup(token);

		token = strtok(NULL, ",");
		aux.cep = atoi(token);

		token = strtok(NULL, ",");
		aux.x = atof(token);

		token = strtok(NULL, ",");
		aux.y = atof(token);

		aux.ativo = 1;
		
		quadtree_insert(aux);
	}
	fclose(file);
}

void read_commands(const char* filename) 
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
		exit(1);
	}

	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;

		char operation;
		char id[20];
		Neighbor* result;
		double x, y;
		int n;
		nodeaddr_t addr;
		switch (buffer[0]) {
		case 'A':
			// activate recharge station
			sscanf(buffer, "%c %s %lf %lf", &operation, id, &x, &y);
			addr = quadtree_search(id, x, y);
			if (addr == INVALIDADDR) {
				printf("Ponto de recarga %s não encontrado.\n", id);
			}
			else {
				QuadTreeNode node;
				node_get(addr, &node);
				if (node.key.ativo) {
					printf("Ponto de recarga %s já estava ativado.\n", id);
				}
				else {
					node.key.ativo = 1;
					node_put(addr, &node);
					printf("Ponto de recarga %s ativado.\n", id);
				}
			}
			break;
		case 'D':
			// deactivate recharge station
			sscanf(buffer, "%c %s %lf %lf", &operation, id, &x, &y);
			addr = quadtree_search(id, x, y);
			if (addr == INVALIDADDR) {
				printf("Ponto de recarga %s não encontrado.\n", id);
			}
			else {
				QuadTreeNode node;
				node_get(addr, &node);
				if (!node.key.ativo) {
					printf("Ponto de recarga %s já estava desativado.\n", id);
				}
				else {
					node.key.ativo = 0;
					node_put(addr, &node);
					printf("Ponto de recarga %s desativado.\n", id);
				}
			}
			break;
		case 'C':
			// find n closest recharge stations
			sscanf(buffer, "%c %lf %lf %d", &operation, &x, &y, &n);
			if (n > nrecharge) {
				printf("Número de pontos de recarga solicitados maior que o número de pontos de recarga disponíveis.\n");
				break;
			}
			result = malloc(n * sizeof(Neighbor));
			if (result == NULL) {
				printf("Erro ao alocar memória.\n");
				break;
			}
			quadtree_k_nearest(x, y, n, result);
			
			// print the n nearest recharging locations
			QuadTreeNode aux;
			for (int i = 0; i < n; i++) {
				printrecharge(result[i].addr);
				printf(" (%f)\n", result[i].dist);
			}
			//printmap(kvet,n,rechargevet,nrecharge,x,y);
			//free(kvet);
			break;
		default:
			printf("Comando inválido.\n");
			break;
		}
	}
}

int main(int argc, char** argv) 
{
	struct timespec inittp, endtp, restp;
    int retp;
	
	load_recharge_stations("./files/estacoes.csv");

	retp = clock_gettime(CLOCK_MONOTONIC, &inittp);
	read_commands("./files/comandos.txt");
	retp = clock_gettime(CLOCK_MONOTONIC, &endtp);
	clkDiff(inittp, endtp, &restp);

	printf("%ld.%.9ld\n", restp.tv_sec, restp.tv_nsec);

	return 0;
}
