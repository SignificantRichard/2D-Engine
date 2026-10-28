#include "edge.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

// Create and initialize a new Edge dynamic array
EdgeArray createEdgeArray(int initialCapacity) {
    EdgeArray arr;
    arr.size = 0;
    arr.capacity = initialCapacity > 0 ? initialCapacity : 4;
    arr.data = (Edge*)malloc(arr.capacity * sizeof(Edge));

    if (!arr.data) {
        printf("Memory allocation failed in createEdgeArray()\n");
        exit(EXIT_FAILURE);
    }
    return arr;
}

// Expand array buffer when full
void expandEdgeArray(EdgeArray* arr) {
    int newCapacity = arr->capacity * 2;
    Edge* newData = (Edge*)realloc(arr->data, newCapacity * sizeof(Edge));

    if (!newData) {
        printf("Memory reallocation failed in expandEdgeArray()\n");
        free(arr->data);
        exit(EXIT_FAILURE);
    }

    arr->data = newData;
    arr->capacity = newCapacity;
}

// Append an Edge to the end of the array
void edgeArrayPushBack(EdgeArray* arr, Edge add) {
    if (arr->size >= arr->capacity) {
        expandEdgeArray(arr);
    }
    arr->data[arr->size] = add;
    arr->size++;
}

// Replace an Edge at a given index
void edgeArrayReplace(EdgeArray* arr, int pos, Edge with) {
    if (pos > arr->size) {
        printf("Position %zu is invalid (too large for array size %zu)\n", pos, arr->size);
        return;
    } else if (pos == arr->size) {
        printf("Position %zu is out of bounds. Did you mean edgeArrayPushBack?\n", pos);
        return;
    }

    arr->data[pos] = with;
}

// Reduce capacity down by 'by' elements (without shrinking below current size)
void shrinkEdgeArray(EdgeArray* arr, int by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    int targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;

    if (targetCapacity < arr->size) {
        targetCapacity = arr->size;
    }
    if (targetCapacity == 0) {
        targetCapacity = 1;
    }

    Edge* newData = (Edge*)realloc(arr->data, targetCapacity * sizeof(Edge));
    if (!newData) {
        printf("Memory reallocation failed in shrinkEdgeArray()\n");
        return;
    }

    arr->data = newData;
    arr->capacity = targetCapacity;
}

// Free the dynamic array buffer
void freeEdgeArray(EdgeArray* arr) {
    if (arr->data) {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->capacity = 0;
}