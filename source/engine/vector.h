#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdlib.h>

// abstract vector operands
#define Vector_new createVector
#define Vector_add addVectors
#define Vector_sub subVectors
#define Vector_mult multVectors
#define Vector_div diviVectors

// 2D representation of Vector
typedef struct {
    int x;
    int y;
} Vect;

Vect createVector(int x, int y);
void addVectors(Vect* original, Vect* additor);
void subVectors(Vect* original, Vect* subtractor);
void multVectors(Vect* original, Vect* multiplier);
void diviVectors(Vect* original, Vect* dividor);

typedef struct {
    Vect* data;      // Pointer to contiguous heap buffer
    int size;     // Current number of elements stored
    int capacity; // Allocated capacity in elements
} VectArray;

Vect* createVectorArray();
// Create and allocate a new Vect dynamic array
VectArray createVectArray(int initialCapacity);
// Internal helper to double capacity when full
void expandVectArray(VectArray* arr);
// Append an item to the end
void vectArrayPushBack(VectArray* arr, Vect add);
// Replace an element at a valid index
void vectArrayReplace(VectArray* arr, int pos, Vect with);
// Shrink allocation down by 'by' elements (or to current size if requested excess)
void shrinkVectArray(VectArray* arr, int by);

// Free memory allocated by the dynamic array
void freeVectArray(VectArray* arr);

#endif