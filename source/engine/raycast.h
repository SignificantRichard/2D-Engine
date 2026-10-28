#ifndef RAYCAST_H
#define RAYCAST_H

#include "edge.h"
#include "vector.h"

typedef struct {
    int hit;
    Vect point;     // Exact (x, y) coordinates of the hit
    float distance; // Distance from origin to the hit point
} RaycastHit;

RaycastHit raycast(Vect origin, Vect ray_vec, Edge edge);

#endif