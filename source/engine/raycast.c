#include "raycast.h"
#include "vector.h"
#include "edge.h"
#include <math.h>

RaycastHit raycast(Vect origin, Vect ray_vec, Edge edge) {
    RaycastHit result = { .hit = 0, .point = {0, 0}, .distance = 0.0f };
    
    Vect p1 = edge.a;
    Vect p2 = edge.b;
    Vect edge_vec = Vector_new(p2.x - p1.x, p2.y - p1.y);

    float r_cross_s = (float)(ray_vec.x * edge_vec.y - ray_vec.y * edge_vec.x);
    if (r_cross_s == 0.0f) return result;

    // Fixed: changed from origin - p1 to p1 - origin
    Vect qp = Vector_new(p1.x - origin.x, p1.y - origin.y);

    float t = (float)(qp.x * edge_vec.y - qp.y * edge_vec.x) / r_cross_s;
    float u = (float)(qp.x * ray_vec.y - qp.y * ray_vec.x) / r_cross_s;

    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        result.hit = 1;
        result.point.x = origin.x + (int)(t * ray_vec.x);
        result.point.y = origin.y + (int)(t * ray_vec.y);
        result.distance = t * sqrtf(ray_vec.x * ray_vec.x + ray_vec.y * ray_vec.y);
    }

    return result;
}