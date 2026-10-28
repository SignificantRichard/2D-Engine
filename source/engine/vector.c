#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

Vect createVector(int x, int y) {
    Vect vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

void addVectors(Vect* original, Vect* additor) {
    original->x += additor->x;
    original->y += additor->y;
}

void subVectors(Vect* original, Vect* subtractor) {
    original->x -= subtractor->x;
    original->y -= subtractor->y;
}

void multVectors(Vect* original, Vect* multiplier) {
    original->x *= multiplier->x;
    original->y *= multiplier->y;
}

void diviVectors(Vect* original, Vect* dividor) {
    original->x /= dividor->x;
    original->y /= dividor->y;
}

Vect* createVectorArray() {
    Vect* arr = malloc(sizeof(Vect));
    return arr;
}

// Create and allocate a new Vect dynamic array
VectArray createVectArray(int initialCapacity) {
    VectArray arr;
    arr.size = 0;
    arr.capacity = initialCapacity > 0 ? initialCapacity : 4;
    arr.data = (Vect*)malloc(arr.capacity * sizeof(Vect));
    
    if (!arr.data) {
        printf("Memory allocation failed in createVectArray()\n");
        exit(EXIT_FAILURE);
    }
    return arr;
}

// Internal helper to double capacity when full
void expandVectArray(VectArray* arr) {
    int newCapacity = arr->capacity * 2;
    Vect* newData = (Vect*)realloc(arr->data, newCapacity * sizeof(Vect));
    
    if (!newData) {
        printf("Memory reallocation failed in expandVectArray()\n");
        free(arr->data); // Free original allocation on failure
        exit(EXIT_FAILURE);
    }
    
    arr->data = newData;
    arr->capacity = newCapacity;
}

// Append an item to the end
void vectArrayPushBack(VectArray* arr, Vect add) {
    if (arr->size >= arr->capacity) {
        expandVectArray(arr);
    }
    arr->data[arr->size] = add;
    arr->size++;
}

// Replace an element at a valid index
void vectArrayReplace(VectArray* arr, int pos, Vect with) {
    if (pos > arr->size) {
        printf("Position %zu is invalid (too large for array size %zu)\n", pos, arr->size);
        return;
    } else if (pos == arr->size) {
        printf("Position %zu is out of bounds. Did you mean vectArrayPushBack?\n", pos);
        return;
    }

    arr->data[pos] = with;
}

// Shrink allocation down by 'by' elements (or to current size if requested excess)
void shrinkVectArray(VectArray* arr, int by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    int targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;
    
    // Ensure we never shrink below current valid size or 1
    if (targetCapacity < arr->size) {
        targetCapacity = arr->size;
    }
    if (targetCapacity == 0) {
        targetCapacity = 1;
    }

    Vect* newData = (Vect*)realloc(arr->data, targetCapacity * sizeof(Vect));
    if (!newData) {
        printf("Memory reallocation failed in shrinkVectArray()\n");
        return;
    }

    arr->data = newData;
    arr->capacity = targetCapacity;
}

// Free memory allocated by the dynamic array
void freeVectArray(VectArray* arr) {
    if (arr->data) {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
}