#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"

int main(void)
{
    FILE* file = fopen("points_data.txt", "w");
    if (!file) {
        fprintf(stderr, "Could not open file for writing\n");
        return 1;
    }
    
    quadtree_create(500, (Boundary){0, 100, 0, 100});
    for (int i = 0; i < 100; i++)
    {
        double x = rand() % 100;
        double y = rand() % 100;
        quadtree_insert((Item) {x, y});
        fprintf(file, "%f %f\n", x, y);
    }
    fclose(file);
    export_quadtree("quadtree_data.txt");
}