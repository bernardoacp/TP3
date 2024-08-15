#include "boundary.h"

bool boundary_contains(Boundary* bd, double x, double y)
{
    return (x >= bd->x_min && x < bd->x_max &&
            y >= bd->y_min && y < bd->y_max);
}

static double min_dist_to_boundary(Boundary* boundary, double x, double y) {
    double dx = fmax(fmax(boundary->x_min - x, 0), x - boundary->x_max);
    double dy = fmax(fmax(boundary->y_min - y, 0), y - boundary->y_max);
    return sqrt(dx * dx + dy * dy);
}

bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist) {
    double min_dist = min_dist_to_boundary(boundary, x, y);
    return min_dist < max_dist;
}