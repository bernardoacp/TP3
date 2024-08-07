// biuaidinaive
// Author: Wagner Meira Jr. 
// Version history:
//    1.0 - 21/07/2024
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
	FILE * out;

	// gnuplot script. to obtain the actual map, it is necessary to run:
	// gnuplot out.gp
	out = fopen("out.gp","wt");
	fprintf(out,"set term postscript eps\n");
	fprintf(out,"set output \"out.eps\"\n");
	fprintf(out,"set size square\n");
	fprintf(out,"set key bottom right\n");
	fprintf(out,"set title \"BiUaiDi Recharging Stations\"\n");
	fprintf(out,"set xlabel \"\"\n");
	fprintf(out,"set ylabel \"\"\n");
	fprintf(out,"unset xtics \n");
	fprintf(out,"unset ytics \n");
	fprintf(out,"plot \"origin.gpdat\" t \"Your location\" pt 4 ps 2, \"recharge.gpdat\" t \"\", \"suggested.gpdat\" t \"Nearest stations\" pt 7 ps 2\n");
	fclose(out);

	// origin point, just one pair of x, y coordinates
	out = fopen("origin.gpdat","wt");
	fprintf(out,"%f %f\n",tx, ty);
	fclose(out);

	// all recharging locations
	out = fopen("recharge.gpdat","wt");
	for (int i=0; i<nrec; i++) {
		fprintf(out,"%f %f\n",rvet[i].x, rvet[i].y);
	}
	fclose(out);

	// the nearest recharging locations
	out = fopen("suggested.gpdat","wt");
	for (int i=0; i<kmax; i++) {
		fprintf(out,"%f %f\n",rvet[kvet[i].id].x, rvet[kvet[i].id].y);
	}
	fclose(out);
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
	nrecharge++;
	
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
		
		i++;
	}
	fclose(file);
}

int main(int argc, char** argv) {

	// count the number of recharge locations we have in rechargevet
	// int numrecharge = 0;
	// while(rechargevet[numrecharge].id_logrado != -1) numrecharge++;

	// read the coordinates of the current point that needs recharging
	double tx = atof(argv[1]);
	double ty = atof(argv[2]);

	/*
	// create the vector of distances and populate it
	ptr_knn_t kvet = (ptr_knn_t) malloc(numrecharge*sizeof(knn_t));
	for (int i=0; i<numrecharge; i++) {
		kvet[i].id = i;
		kvet[i].dist = dist(tx, ty, rechargevet[i].x, rechargevet[i].y);
	}

	// sort the vector of distances
	qsort(kvet,numrecharge, sizeof(knn_t), cmpknn);

	// print the 10 nearest recharging locations
	int kmax = 10;
	for (int i=0; i<kmax; i++) {
		printrecharge(kvet[i].id);
		printf(" (%f)\n", kvet[i].dist);
	}

	printmap(kvet,kmax,rechargevet,numrecharge,tx,ty);

	free(kvet);
	exit(0);
	}
	*/
	load_recharge_stations("estacoes.csv");
	for (int i = 0; i < nrecharge; i++) {
		printf("%s %ld %s %s %d %s %s %d %lf %lf\n",
			   rechargevet[i].idend,
			   rechargevet[i].id_logrado,
			   rechargevet[i].sigla_tipo,
			   rechargevet[i].nome_logra,
			   rechargevet[i].numero_imo,
			   rechargevet[i].nome_bairr,
			   rechargevet[i].nome_regio,
			   rechargevet[i].cep,
			   rechargevet[i].x,
			   rechargevet[i].y);
	}
}
