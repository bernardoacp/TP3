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
} addr_t, *ptr_addr_t;

// struct that contain the distance information between the origin location and
// each recharging location.
typedef struct knn {
	double dist; // distance between origin location and recharging location
	int id;      // recharging location id
} knn_t, *ptr_knn_t;

// vector containing the information about the recharging locations
addr_t* rechargevet = NULL;
int nrecharge = 0;

// qsort comparison function between distances to recharging locations
int cmpknn(const void* a, const void* b) {
	ptr_knn_t k1 = (ptr_knn_t) a;
	ptr_knn_t k2 = (ptr_knn_t) b;
	if (k1->dist > k2->dist) return 1;
	else if (k1->dist < k2->dist) return -1;
	else return 0;
}

// calculates Euclidean distance between (x1,y1) and (x2,y2)
double dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0); 
}

// print the recharge location information
void printrecharge(int pos){
	printf("%s %s, %d, %s, %s, %d",rechargevet[pos].sigla_tipo,
				rechargevet[pos].nome_logra, rechargevet[pos].numero_imo,
				rechargevet[pos].nome_bairr, rechargevet[pos].nome_regio,
				rechargevet[pos].cep);
}

// print illustrative map using gnuplot
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
	
	// allocate memory for the rechargevet vector
	rechargevet = (ptr_addr_t) malloc(nrecharge*sizeof(addr_t));
	if (rechargevet == NULL) {
		fprintf(stderr, "Erro: nao foi possivel alocar memoria para o vetor de estacoes de recarga.\n");
		exit(1);
	}

	// read the file and populate the rechargevet vector
	rewind(file);
	char buffer[1024];
	int i = 0;
	while (fgets(buffer, sizeof(buffer), file)) {
		// remove newline character if present
		buffer[strcspn(buffer, "\n")] = 0;

		// parse the line
		char* token = strtok(buffer, ",");
		rechargevet[i].idend = strdup(token);
		
		token = strtok(NULL, ",");
		rechargevet[i].id_logrado = atol(token);

		token = strtok(NULL, ",");
		rechargevet[i].sigla_tipo = strdup(token);

		token = strtok(NULL, ",");
		rechargevet[i].nome_logra = strdup(token);

		token = strtok(NULL, ",");
		rechargevet[i].numero_imo = atoi(token);

		token = strtok(NULL, ",");
		rechargevet[i].nome_bairr = strdup(token);

		token = strtok(NULL, ",");
		rechargevet[i].nome_regio = strdup(token);

		token = strtok(NULL, ",");
		rechargevet[i].cep = atoi(token);

		token = strtok(NULL, ",");
		rechargevet[i].x = atof(token);

		token = strtok(NULL, ",");
		rechargevet[i].y = atof(token);

		rechargevet[i].ativo = 1;
		
		i++;
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
		double x, y;
		int n;
		
		switch (buffer[0]) {
		case 'A':
			// activate recharge station
			sscanf(buffer, "%c %s", &operation, id);
			for (int i = 0; i < nrecharge; i++) {
				if (!strcmp(rechargevet[i].idend, id)) {
					if (rechargevet[i].ativo) {
						printf("Ponto de recarga %s já estava ativo.\n", id);
					}
					else {
						rechargevet[i].ativo = 1;
						printf("Ponto de recarga %s ativado.\n", id);
					}
					break;
				}
			}
			break;
		case 'D':
			// deactivate recharge station
			sscanf(buffer, "%c %s", &operation, id);
			for (int i = 0; i < nrecharge; i++) {
				if (!strcmp(rechargevet[i].idend, id)) {
					if (!rechargevet[i].ativo) {
						printf("Ponto de recarga %s já estava desativado.\n", id);
					}
					else {
						rechargevet[i].ativo = 0;
						printf("Ponto de recarga %s desativado.\n", id);
					}
					break;
				}
			}
			break;
		case 'C':
			// find n closest recharge stations
			sscanf(buffer, "%c %lf %lf %d", &operation, &x, &y, &n);
			
			// create the vector of distances and populate it
			ptr_knn_t kvet = (ptr_knn_t) malloc(nrecharge*sizeof(knn_t));
			int kvet_size = 0; // track the number of active stations
			
			for (int i = 0; i < nrecharge; i++) {
				if (!rechargevet[i].ativo) {
					continue;
				}
				kvet[kvet_size].id = i;
				kvet[kvet_size].dist = dist(x, y, rechargevet[i].x, rechargevet[i].y);
				kvet_size++;
			}

			// sort the vector of distances
			qsort(kvet, kvet_size, sizeof(knn_t), cmpknn);
			
			// print the n nearest recharging locations
			for (int i = 0; i < n; i++) {
				printrecharge(kvet[i].id);
				printf(" (%f)\n", kvet[i].dist);
			}
			printmap(kvet,n,rechargevet,nrecharge,x,y);
			free(kvet);
			break;
		default:
			printf("Comando inválido.\n");
			break;
		}
	}
}

int main(int argc, char** argv) 
{
	load_recharge_stations("./files/estacoes.csv");
	read_commands("./files/comandos.txt");
	return 0;
}
