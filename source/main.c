#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <math.h>

// Let me be clear -Barack Barack
#define true 1
#define false 0
#define print printf
#define bool int

// envars
#define DEFAULT_MAX_SPEED 16
#define MAX_ELEMENTS_IN_WORLD 255
#define TICK_RATE 2 // tick every 2 frames
#define FRAME_RATE 60
#define INTERNAL_WIDTH 640
#define INTERNAL_HEIGHT 480

// abstract vector operands
#define Vector_new createVector
#define Vector_add addVectors
#define Vector_mult multVectors
#define Vector_div diviVectors

// abstract body manipulation
#define Body_new createBody
#define body_speedLim setSpeedLimit
#define body_setAcc setBodyAccel
#define body_setVel setBodyVel
#define body_setPos setBodyPosition

// 2D representation of Vector
typedef struct {
    int x;
    int y;
} Vect;

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

typedef struct {
    Vect position;
    Vect velocity;
    Vect acceleration;
    int maxSpeed;
} Body;

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

typedef struct {
    Vect* data;      // Pointer to contiguous heap buffer
    size_t size;     // Current number of elements stored
    size_t capacity; // Allocated capacity in elements
} VectArray;

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
typedef struct {
    Vect* a;
    Vect* b;
} Edge;

typedef struct {
    Edge* data;      // Contiguous array of Edge elements
    size_t size;     // Current number of edges
    size_t capacity; // Total allocated slots
} EdgeArray;

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

typedef struct {
    Edge a;
    Edge b;
    Edge c;
} Face;

typedef struct {
    Face* data;      // Contiguous array of Face elements
    size_t size;     // Current number of faces
    size_t capacity; // Total allocated slots
} FaceArray;

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

typedef struct {
    Face* faces;
} Mesh;

typedef struct {
    Mesh* meshes; // all static elements (should be wrapped later)
    Body* bodies; // all moving elements
} World;

typedef struct {
    Uint32* image;
} Sprite;

typedef struct {
    Sprite sprite; // what is drawn
    Body body; // movement
    Mesh mesh; // collision mesh
    int alpha; // 255, set to 0 to hide
} RenderedBody;

typedef struct {
    Sprite sprite;
    Mesh mesh;
    Vect position;
} StaticBody;

World initEngine() {
    World world;
    // calloc MAX_ELEMENTS_IN_WORLD elements because I can't be bothered
    world.meshes = (Mesh*)calloc(MAX_ELEMENTS_IN_WORLD, sizeof(Mesh));
    world.bodies = (Body*)calloc(MAX_ELEMENTS_IN_WORLD, sizeof(Body));
    return world;
}

void ModifyPixel(SDL_Surface* surface, Uint32 x, Uint32 y, int r, int g, int b, int a) {
    // Lock surface for direct pixel access
    if (SDL_MUSTLOCK(surface)) {
        if (!SDL_LockSurface(surface)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock surface: %s", SDL_GetError());
            return;
        }
    }

    // Cast raw void* pixel buffer to 32-bit unsigned integer pointer (for RGBA8888)
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;
    
    // Pitch is in bytes. Divide by sizeof(Uint32) to get pitch in 32-bit pixels
    int pitchInPixels = surface->pitch / sizeof(Uint32);

    // Map standard R, G, B, A to the pixel format of this surface
    Uint32 color = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), NULL, r, g, b, a);

    // Write pixel using row-pitch offset
    pixels[y * pitchInPixels + x] = color;

    // Unlock surface when finished
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
}

void ModifyPixels(SDL_Surface* surface, int x, int y, int w, int h, int r, int g, int b, int a) {
    for (int i = x; i < w+x; i++) {
        for (int j = y; j < h+y; j++) {
            ModifyPixel(surface, i, j, r, g, b, a);
        }
    }
}

// Struct to store intersection results
typedef struct {
    bool hit;
    Vect point;     // Exact (x, y) coordinates of the hit
    float distance; // Distance from origin to the hit point
} RaycastHit;

RaycastHit raycast(Vect origin, Vect direction, int magnitude, Edge edge) {
    RaycastHit result = { .hit = false, .point = {0, 0}, .distance = 0.0f };

    Vect ray_vec;
    ray_vec = Vector_new(direction.x, direction.y);
    Vect mvect;
    mvect = Vector_new(magnitude, magnitude);
    Vector_mult(&ray_vec, &mvect);
    
    Vect* p1 = edge.a;
    Vect* p2 = edge.b;
    Vect edge_vec = Vector_new(p2->x - p1->x, p2->y - p1->y);

    // 2. Compute 2D cross product: r x s
    float r_cross_s = (float)(ray_vec.x * edge_vec.y - ray_vec.y * edge_vec.x);

    // If parallel or collinear, no unique intersection point exists
    if (r_cross_s == 0.0f) {
        return result;
    }

    // Vector from edge start point to ray origin (qp)
    Vect qp = Vector_new(origin.x - p1->x, origin.y - p1->y);

    // 3. Solve parametric factors t and u
    // t: normalized position along ray [0.0 = origin, 1.0 = end of ray]
    // u: normalized position along edge [0.0 = p1, 1.0 = p2]
    float t = (float)(qp.x * edge_vec.y - qp.y * edge_vec.x) / r_cross_s;
    float u = (float)(qp.x * ray_vec.y - qp.y * ray_vec.x) / r_cross_s;

    // 4. Verify valid intersection bounds
    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        result.hit = true;

        // Calculate exact point on the edge: origin + (t * ray_vec)
        result.point.x = origin.x + (int)(t * ray_vec.x);
        result.point.y = origin.y + (int)(t * ray_vec.y);

        // Distance = t * total_ray_magnitude
        result.distance = t * (float)magnitude;
    }

    return result;
}


// Do game checks here
void tickHit() {

}

int Work() {
    const Uint32 frameDelay = 1000 / FRAME_RATE;
    bool running = true;
    Uint32 frameCount = 0;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!SDL_CreateWindowAndRenderer("Pixel Manipulation Engine", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // buffer surface
    SDL_Surface* surface = SDL_CreateSurface(INTERNAL_WIDTH, INTERNAL_HEIGHT, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create surface: %s", SDL_GetError());
        return 1;
    }

    // Create a streaming GPU texture matching the surface dimensions
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        surface->format, 
        SDL_TEXTUREACCESS_STREAMING, 
        surface->w, 
        surface->h
    );

    // Define everything and everyone

    World world;
    world = initEngine();

    while (running) {
        Uint64 frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // Upload surface pixel data to GPU texture
        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

        // Clear and Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render texture scaled to fill window
        SDL_RenderTexture(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);

        // Cap frame rate
        Uint64 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - (Uint32)frameTime);
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main() {
    return Work();
}