#include <stdio.h>
#include <stdlib.h>

// Let me be clear -Barack Barack
#define true 1
#define false 0
#define print printf
#define bool int

// envars
#define DEFAULT_MAX_SPEED 16

// abstract vector operands
#define Vector_new createVector
#define Vector_add addVectors
#define Vector_mult multVectors
#define Vector_div diviVectors

#define Mesh VectArray

// abstract body manipulation
#define Body_new createBody
#define body_speedLim setSpeedLimit
#define body_setAcc setBodyAccel
#define body_setVel setBodyVel
#define body_setPos setBodyPosition

// 2D representation of Vector
struct Vector_t {
    int x;
    int y;
};

typedef struct Vector_t Vect;

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

struct Body_t {
    Vect position;
    Vect velocity;
    Vect acceleration;
    int maxSpeed;
};

typedef struct Body_t Body;

Body createBody() {
    Body body;

    // define sub stuff
    Vect position;
    position = Vector_new(0, 0);
    Vect acceleration;
    acceleration = Vector_new(0, 0);
    
    body.position = position;
    body.acceleration = acceleration;
    body.maxSpeed = DEFAULT_MAX_SPEED;

    return body;
}

void setSpeedLimit(Body* body, int maxSpeed) {
    body->maxSpeed = maxSpeed;
}

void setBodyPosition(Body* body, int x, int y) {
    body->position.x = x;
    body->position.y = y;
}

void setBodyAccel(Body* body, int x, int y) {
    body->acceleration.x = x;
    body->acceleration.y = y;
}

void setBodyPos(Body* body, int x, int y) {
    body->velocity.x = x;
    body->velocity.y = y;
}

// vector array

struct VectArray_t {
    Vect* data;      // Pointer to contiguous heap buffer
    size_t size;     // Current number of elements stored
    size_t capacity; // Allocated capacity in elements
};

typedef struct VectArray_t VectArray;

Vect* createVectorArray() {
    Vect* arr = malloc(sizeof(Vect));
    return arr;
}

// Create and allocate a new Vect dynamic array
VectArray createVectArray(size_t initialCapacity) {
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
    size_t newCapacity = arr->capacity * 2;
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
void vectArrayReplace(VectArray* arr, size_t pos, Vect with) {
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
void shrinkVectArray(VectArray* arr, size_t by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    size_t targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;
    
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

// Edge

struct Edge_t {
    Vect* a;
    Vect* b;
};

typedef struct Edge_t Edge;

struct Edge_t {
    Vect* a;
    Vect* b;
};

typedef struct Edge_t Edge;

struct EdgeArray_t {
    Edge* data;      // Contiguous array of Edge elements
    size_t size;     // Current number of edges
    size_t capacity; // Total allocated slots
};

typedef struct EdgeArray_t EdgeArray;

// Create and initialize a new Edge dynamic array
EdgeArray createEdgeArray(size_t initialCapacity) {
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
    size_t newCapacity = arr->capacity * 2;
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
void edgeArrayReplace(EdgeArray* arr, size_t pos, Edge with) {
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
void shrinkEdgeArray(EdgeArray* arr, size_t by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    size_t targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;

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

struct Face_t {
    Edge a;
    Edge b;
    Edge c;
};

typedef struct Face_t Face;

struct FaceArray_t {
    Face* data;      // Contiguous array of Face elements
    size_t size;     // Current number of faces
    size_t capacity; // Total allocated slots
};

// Create and initialize a new Face dynamic array
FaceArray createFaceArray(size_t initialCapacity) {
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
    size_t newCapacity = arr->capacity * 2;
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
void faceArrayReplace(FaceArray* arr, size_t pos, Face with) {
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
void shrinkFaceArray(FaceArray* arr, size_t by) {
    if (by == 0 || arr->capacity <= arr->size) return;

    size_t targetCapacity = (arr->capacity > by) ? (arr->capacity - by) : arr->size;

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

struct Mesh_t {
    Face* faces;
};

typedef struct Mesh_t Mesh;

struct World {
    Mesh* meshes; // all static elements (should be wrapped later)
    Bodies* bodies; // all moving elements
}

int main() {
    Vect vec;
    vec = Vector_new(10, 10);
    
    return 0;
}