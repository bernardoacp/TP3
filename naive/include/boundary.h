#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

// Estrutura que define os limites de um retângulo (Boundary)
typedef struct {
    double x_min; // Coordenada mínima no eixo x
    double x_max; // Coordenada máxima no eixo x
    double y_min; // Coordenada mínima no eixo y
    double y_max; // Coordenada máxima no eixo y
} Boundary;

// Função que verifica se um ponto (x, y) está contido dentro dos limites do 
// retângulo (Boundary)
bool boundary_contains(Boundary* bd, double x, double y); 

// Função que verifica se um retângulo (Boundary) pode conter um ponto mais 
// próximo que uma distância máxima (max_dist)
bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist);

#endif 