#include "face.h"
#include <stdio.h>
#include <stdlib.h>

#include "edge.h"

// Create and initialize a new Face dynamic array
FaceArray createFaceArray(int initialCapacity) {
    FaceArray arr;
    arr.size = 0;
    arr.capacity = initialCapacity > 0 ? initialCapacity : 4;
    arr.data = (Face*)malloc(arr.capacity * sizeof(Face));

    if (!arr.data) {
        printf("Memory allocation failed in createFaceArray()\n");
        exit(EXIT_FAILURE);
    }
    return arr;
}

// Expand array buffer when full
void expandFaceArray(FaceArray* arr) {
    int newCapacity = arr->capacity * 2;
    Face* newData = (Face*)realloc(arr->data, newCapacity * sizeof(Face));

    if (!newData) {
        printf("Memory reallocation failed in expandFaceArray()\n");
        free(arr->data);
        exit(EXIT_FAILURE);
    }

    arr->data = newData;
    arr->capacity = newCapacity;
}

// Append a Face to the end of the array
void faceArrayPushBack(FaceArray* arr, Face add) {
    if (arr->size >= arr->capacity) {
        expandFaceArray(arr);
    }
    arr->data[arr->size] = add;
    arr->size++;
}

// Replace a Face at a given index
void faceArrayReplace(FaceArray* arr, int pos, Face with) {
    if (pos > arr->size) {
        printf("Position %zu is invalid (too large for array size %zu)\n", pos, arr->size);
        return;
    } else if (pos == arr->size) {
        printf("Position %zu is out of bounds. Did you mean faceArrayPushBack?\n", pos);
        return;
    }

    arr->data[pos] = with;
}

// Reduce capacity down by 'by' elements (without shrinking below current size)
void shrinkFaceArray(FaceArray* arr, int by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    int targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;

    if (targetCapacity < arr->size) {
        targetCapacity = arr->size;
    }
    if (targetCapacity == 0) {
        targetCapacity = 1;
    }

    Face* newData = (Face*)realloc(arr->data, targetCapacity * sizeof(Face));
    if (!newData) {
        printf("Memory reallocation failed in shrinkFaceArray()\n");
        return;
    }

    arr->data = newData;
    arr->capacity = targetCapacity;
}

// Free the dynamic array buffer
void freeFaceArray(FaceArray* arr) {
    if (arr->data) {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
}