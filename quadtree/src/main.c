#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"

int main(void)
{
    quadtree_create(100, (Boundary){0, 100, 0, 100});
    quadtree_insert((Item) {10, 10});
    quadtree_insert((Item) {75, 75});
    quadtree_insert((Item) {10, 75});
    quadtree_insert((Item) {75, 10});
}