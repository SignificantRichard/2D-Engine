#ifndef FACE_H
#define FACE_H

#include "edge.h"

typedef struct {
    Edge a;
    Edge b;
    Edge c;
} Face;

typedef struct {
    Face* data;      // Contiguous array of Face elements
    int size;     // Current number of faces
    int capacity; // Total allocated slots
} FaceArray;

// Create and initialize a new Face dynamic array
FaceArray createFaceArray(int initialCapacity);
// Expand array buffer when full
void expandFaceArray(FaceArray* arr);
// Append a Face to the end of the array
void faceArrayPushBack(FaceArray* arr, Face add);
// Replace a Face at a given index
void faceArrayReplace(FaceArray* arr, int pos, Face with);
// Reduce capacity down by 'by' elements (without shrinking below current size)
void shrinkFaceArray(FaceArray* arr, int by);
// Free the dynamic array buffer
void freeFaceArray(FaceArray* arr);

#endif