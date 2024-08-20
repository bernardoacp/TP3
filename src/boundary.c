#include "boundary.h"

bool boundary_contains(Boundary* bd, double x, double y)
{
    // Retorna verdadeiro se o ponto (x, y) estiver dentro dos limites do 
    // retângulo
    return (x >= bd->x_min && x < bd->x_max &&
            y >= bd->y_min && y < bd->y_max);
}

// Função auxiliar que calcula a distância mínima de um ponto (x, y) até os 
// limites do retângulo (Boundary)
static double min_dist_to_boundary(Boundary* boundary, double x, double y) {
    // Calcula a distância no eixo x até o limite mais próximo do retângulo
    double dx = fmax(fmax(boundary->x_min - x, 0), x - boundary->x_max);
    // Calcula a distância no eixo y até o limite mais próximo do retângulo
    double dy = fmax(fmax(boundary->y_min - y, 0), y - boundary->y_max);
    // Retorna a distância euclidiana mínima até o retângulo
    return sqrt(dx * dx + dy * dy);
}

bool can_contain_closer_point(Boundary* boundary, double x, double y, double max_dist) {
    // Calcula a distância mínima do ponto (x, y) até os limites do retângulo
    double min_dist = min_dist_to_boundary(boundary, x, y);
    // Retorna verdadeiro se a distância mínima for menor que a distância
    // máxima permitida
    return min_dist < max_dist;
}