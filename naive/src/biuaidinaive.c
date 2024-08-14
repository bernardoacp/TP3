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
#include "quadtree.h"
#include "qnode.h"
#include "heap.h"
#include "boundary.h"

int nrecharge = 0;

typedef struct {
	char* idend;
	double x;
	double y;
} Consulta;

Consulta* vet;

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
void printrecharge(int pos, FILE* out) {
	QuadTreeNode aux;
	node_get(pos, &aux);
	fprintf(out, "%s %s, %d, %s, %s, %d", aux.key.sigla_tipo,
				aux.key.nome_logra, aux.key.numero_imo,
				aux.key.nome_bairr, aux.key.nome_regio,
				aux.key.cep);
}

// print illustrative map using gnuplot
void printmap(Neighbor* kvet, int kmax, int nrec, double tx, double ty) {
	FILE *out1, *out2;

	// Export the quadtree data
    export_quadtree("plot/quadtree.gpdat");

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
	fprintf(out1,"plot \"plot/origin.gpdat\" t \"Your location\" pt 4 ps 2, \"plot/recharge.gpdat\" t \"\", \"plot/suggested.gpdat\" t \"Nearest stations\" pt 7 ps 2, \"plot/deactivated.gpdat\" t \"Deactivated stations\" pt 2 ps 1, \"plot/quadtree.gpdat\" u (($1+$2)/2):(($3+$4)/2):(($2-$1)/2):(($4-$3)/2) w boxxy t \"\"\n");
	fclose(out1);

	// origin point, just one pair of x, y coordinates
	out1 = fopen("plot/origin.gpdat","wt");
	fprintf(out1,"%f %f\n",tx, ty);
	fclose(out1);

	// active recharging locations
	out1 = fopen("plot/recharge.gpdat","wt");
	//  deactivated recharging locations
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

	// the nearest recharging locations
	out1 = fopen("plot/suggested.gpdat","wt");
	for (int i = 0; i < kmax; i++) {
		node_get(kvet[i].addr, &aux);
		fprintf(out1,"%f %f\n", aux.key.x, aux.key.y);
	}
	fclose(out1);
}

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

int cmpknn(const void* a, const void* b) {
	Consulta* k1 = (Consulta*) a;
	Consulta* k2 = (Consulta*) b;
	if (strcmp(k1->idend, k2->idend) > 0) return 1;
	else if (strcmp(k1->idend, k2->idend) < 0) return -1;
	else return 0;
}

void load_recharge_stations(const char* filename) 
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
		exit(1);
	}

	// read the file and populate the quadtree
	char buffer[1024];
	if (fgets(buffer, sizeof(buffer), file) == NULL) {
		fprintf(stderr, "Erro: nao foi possivel ler o numero de pontos de recarga\n");
		fclose(file);
		exit(1);
	}	
	
	buffer[strcspn(buffer, "\n")] = 0;
	sscanf(buffer, "%d", &nrecharge);

	quadtree_create(nrecharge, (Boundary) {598017.313632323, 619122.989979841, 7785041.75619417, 7812836.09085508});
	vet = malloc(nrecharge * sizeof(Consulta));

	Item aux;
	for (int i = 0; i < nrecharge; i++) {
		if (fgets(buffer, sizeof(buffer), file) == NULL) {
			fprintf(stderr, "Erro: nao foi possivel ler o ponto de recarga %d\n", i);
			fclose(file);
			exit(1);
		}
		// remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;

		// parse the line
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

		aux.ativo = true;
		
		quadtree_insert(aux);
	}
	fclose(file);

	qsort(vet, nrecharge, sizeof(Consulta), cmpknn);
}

void read_commands(const char* filename) 
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "Erro: nao foi possivel abrir o arquivo %s\n", filename);
		exit(1);
	}

	FILE* out = fopen("./files/geracarga.out", "w");
	if (file == NULL) {
		fprintf(stderr, "Erro: nao foi possivel abrir o arquivo geracarga.out\n");
		exit(1);
	}

	char buffer[1024];
	if (fgets(buffer, sizeof(buffer), file) == NULL) {
        fprintf(stderr, "Erro: nao foi possivel ler o numero de comandos\n");
        fclose(file);
        fclose(out);
        exit(1);
    }
	int num_commands;
    sscanf(buffer, "%d", &num_commands);

	for (int i = 0; i < num_commands; i++) {
		if (fgets(buffer, sizeof(buffer), file) == NULL) {
			fprintf(stderr, "Erro: nao foi possivel ler o comando %d\n", i);
			fclose(file);
			fclose(out);
			exit(1);
		}
		// remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;

		char operation;
		char id[20];
		Neighbor* result;
		Consulta* query;

		double x, y;
		long n;
		nodeaddr_t addr;
		
		switch (buffer[0]) {
		case 'A':
			// activate recharge station
			sscanf(buffer, "%c %s", &operation, id);
			
			fprintf(out, "%c %s\n", operation, id);

			query = bin_search(id, vet, nrecharge);
			
			addr = quadtree_search(id, query->x, query->y);
			
			if (addr == INVALIDADDR) {
				fprintf(out, "Ponto de recarga %s não encontrado.\n", id);
			}
			else {
				QuadTreeNode node;
				node_get(addr, &node);
				if (node.key.ativo) {
					fprintf(out, "Ponto de recarga %s já estava ativo.\n", id);
				}
				else {
					node.key.ativo = true;
					node_put(addr, &node);
					fprintf(out, "Ponto de recarga %s ativado.\n", id);
				}
			}
			break;
		case 'D':
			// deactivate recharge station
			sscanf(buffer, "%c %s", &operation, id);

			fprintf(out, "%c %s\n", operation, id);

			query = bin_search(id, vet, nrecharge);

			addr = quadtree_search(id, query->x, query->y);
			
			if (addr == INVALIDADDR) {
				fprintf(out, "Ponto de recarga %s não encontrado.\n", id);
			}
			else {
				QuadTreeNode node;
				node_get(addr, &node);
				if (!node.key.ativo) {
					fprintf(out, "Ponto de recarga %s já estava desativado.\n", id);
				}
				else {
					node.key.ativo = false;
					node_put(addr, &node);
					fprintf(out, "Ponto de recarga %s desativado.\n", id);
				}
			}
			break;
		case 'C':
			// find n closest recharge stations
			sscanf(buffer, "%c %lf %lf %ld", &operation, &x, &y, &n);

			fprintf(out, "%c %lf %lf %ld\n", operation, x, y, n);

			if (n > nrecharge) {
				fprintf(stderr, "Número de pontos de recarga solicitados maior que o número de pontos de recarga disponíveis.\n");
				break;
			}
			result = malloc(n * sizeof(Neighbor));
			if (result == NULL) {
				fprintf(stderr, "Erro ao alocar memória.\n");
				break;
			}
			quadtree_k_nearest(x, y, n, result);
			
			// print the n nearest recharging locations
			QuadTreeNode aux;
			for (int i = 0; i < n; i++) {
				printrecharge(result[i].addr, out);
				fprintf(out, "(%.3f)\n", result[i].dist);
			}
			printmap(result,n,nrecharge,x,y);
			free(result);
			break;
		default:
			fprintf(stderr, "Comando inválido.\n");
			break;
		}
	}
}

int main(int argc, char** argv) 
{
	struct timespec inittp, endtp, restp;
    int retp;
	
	load_recharge_stations("./files/geracarga.base");

	retp = clock_gettime(CLOCK_MONOTONIC, &inittp);
	read_commands("./files/geracarga.ev");
	retp = clock_gettime(CLOCK_MONOTONIC, &endtp);
	clkDiff(inittp, endtp, &restp);

	printf("%ld.%.9ld\n", restp.tv_sec, restp.tv_nsec);

	return 0;
}
