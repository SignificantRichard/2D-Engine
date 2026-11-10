#include <stdlib.h>
#include "polygon.h"
#include "vector.h"

PolygonArray createPolygonArray(int size) {
    PolygonArray arr = { .used = 0, .size = size, .data = (Polygon*)malloc(sizeof(Polygon) * size) };
    return arr;
}

void resizePolygonArray(PolygonArray* arr, int to) {
    switch (to) {
        case -1:
            to = ++arr->size;
            break;
        case -2:
            to = --arr->size;
    }
    Polygon* temp = realloc(arr->data, sizeof(Polygon) * to);
    arr->data = temp;
}

void addToPolygonArray(PolygonArray* arr, Polygon poly) {
    resizePolygonArray(arr, -1);
    arr->data[arr->size - 1] = poly;
}


void removeFromPolygonArray(PolygonArray* arr, int index) {
    arr->data[index] = arr->data[arr->size - 1];
    resizePolygonArray(arr, -2);
}