#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"

typedef struct {
    Vect a;
    Vect b;
    Vect c;
    Vect d;
} Polygon;

typedef struct {
    Polygon* data;
    int used;
    int size;
} PolygonArray;

PolygonArray createPolygonArray(int size);
void resizePolygonArray(PolygonArray* arr, int to);
void addToPolygonArray(PolygonArray* arr, Polygon poly);
void removeFromPolygonArray(PolygonArray* arr, int index);

#endif