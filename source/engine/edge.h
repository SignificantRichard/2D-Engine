#ifndef EDGE_H
#define EDGE_H

#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

// Edge
typedef struct {
    Vect a;
    Vect b;
} Edge;


typedef struct {
    Edge* data;      // Contiguous array of Edge elements
    int size;     // Current number of edges
    int capacity; // Total allocated slots
} EdgeArray;

// Create and initialize a new Edge dynamic array
EdgeArray createEdgeArray(int initialCapacity);
// Expand array buffer when full
void expandEdgeArray(EdgeArray* arr);
// Append an Edge to the end of the array
void edgeArrayPushBack(EdgeArray* arr, Edge add);
// Replace an Edge at a given index
void edgeArrayReplace(EdgeArray* arr, int pos, Edge with);
// Reduce capacity down by 'by' elements (without shrinking below current size)
void shrinkEdgeArray(EdgeArray* arr, int by);
// Free the dynamic array buffer
void freeEdgeArray(EdgeArray* arr);

#endif