#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * Creates a new Vect struct with the given x and y values.
 * 
 * @param x The x value of the vector.
 * @param y The y value of the vector.
 * 
 * @return A new Vect struct with the given x and y values.
 */
Vect createVector(float x, float y) {
    Vect vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

/**
 * Adds two Vect structs together.
 * 
 * @param original The Vect struct to add to.
 * @param additor The Vect struct to add.
 */
void addVectors(Vect* original, Vect* additor) {
    original->x += additor->x;
    original->y += additor->y;
}

/**
 * Subtracts two Vect structs together.
 * 
 * @param original The Vect struct to subtract from.
 * @param subtractor The Vect struct to subtract.
 */
void subVectors(Vect* original, Vect* subtractor) {
    original->x -= subtractor->x;
    original->y -= subtractor->y;
}

/**
 * Multiplies a Vect struct by a scalar value.
 * 
 * @param original The Vect struct to multiply.
 * @param multiplier The scalar value to multiply by.
 */
void multVectors(Vect* original, Vect* multiplier) {
    original->x *= multiplier->x;
    original->y *= multiplier->y;
}

/**
 * Divides a Vect struct by a scalar value.
 * 
 * @param original The Vect struct to divide.
 * @param dividor The scalar value to divide by.
 */
void diviVectors(Vect* original, Vect* dividor) {
    original->x /= dividor->x;
    original->y /= dividor->y;
}

/**
 * Calculates the magnitude of two Vect structs.
 * 
 * @param v1 The first Vect struct.
 * @param v2 The second Vect struct.
 * 
 * @return The magnitude of the difference between the two Vect structs.
 */
float magniVectors(Vect* v1, Vect* v2) {
    return sqrtf(pow(v1->x - v2->x, 2) + pow(v1->y - v2->y, 2));
}

/**
 * Calculates the magnitude of a single Vect struct.
 * 
 * @param v The Vect struct.
 * 
 * @return The magnitude of the Vect struct.
 */
float magniVector(Vect* v) {
    return sqrtf(pow(v->x, 2) + pow(v->y, 2));
}

/**
 * Creates a new VectArray struct with the given initial capacity.
 * 
 * @param initialCapacity The initial capacity of the array.
 * 
 * @return A new VectArray struct with the given initial capacity.
 */
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

/**
 * Doubles the capacity of a VectArray struct.
 * 
 * @param arr The VectArray struct to double the capacity of.
 */
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

/**
 * Appends a new Vect struct to the end of a VectArray struct.
 * 
 * @param arr The VectArray struct to append to.
 * @param add The Vect struct to append.
 */
void vectArrayPushBack(VectArray* arr, Vect add) {
    if (arr->size >= arr->capacity) {
        expandVectArray(arr);
    }
    arr->data[arr->size] = add;
    arr->size++;
}

/**
 * Replaces an element at a valid index in a VectArray struct.
 * 
 * @param arr The VectArray struct to replace in.
 * @param pos The index to replace.
 * @param with The new Vect struct to replace with.
 */
void vectArrayReplace(VectArray* arr, int pos, Vect with) {
    if (pos > arr->size) {
        printf("Position %d is invalid (too large for array size %d)\n", pos, arr->size);
        return;
    } else if (pos == arr->size) {
        printf("Position %d is out of bounds. Did you mean vectArrayPushBack?\n", pos);
        return;
    }

    arr->data[pos] = with;
}

/**
 * Shrink the capacity of a VectArray struct by a specified amount.
 * 
 * @param arr The VectArray struct to shrink.
 * @param by The amount to shrink by.
 */
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

/**
 * Frees the memory allocated by a VectArray struct.
 * 
 * @param arr The VectArray struct to free.
 */
void freeVectArray(VectArray* arr) {
    if (arr->data) {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
}