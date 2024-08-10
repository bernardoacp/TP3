#include "boundary.h"

bool boundary_contains(Boundary* bd, double x, double y)
{
    return (x > bd->x_min && x <= bd->x_max &&
            y > bd->y_min && y <= bd->y_max);
}