#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
} Boundary;

bool boundary_contains(Boundary* bd, double x, double y); 
bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist);

#endif