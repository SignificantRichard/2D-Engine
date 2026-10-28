#ifndef WORLD_H
#define WORLD_H

#include "vector.h"
#include "edge.h"
#include "body.h"

typedef struct {
    Vect offset;
    Body* target;
} Camera;

typedef struct {
    RenderedBody* bodies; // all moving elements
    RenderedBody player; // player
    Camera camera; // camera
    EdgeArray walls; // static collision bounds
} World;

#endif