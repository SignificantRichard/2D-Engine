// Engine Stuff
#include "../engine/world.h"
#include "../engine/image.h"
#include "../engine/memory.h"
#include "../engine/obama.h"
#include "../engine/vector.h"
#include "../engine/body.h" 
#include "../engine/edge.h"
#include "../engine/face.h"
#include "../engine/files.h"
#include "../engine/sound.h"
#include "../engine/raycast.h"
#include "../engine/collision.h"

// Self
#include "game_main.h"

// C libraries
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <SDL3/SDL.h>

#define rotationSpeed 0.1f

static RenderQueue renderQueue;

static float rotationDirection = 0.0f; // player rotation
static float walkDirection = 0.0f;
static float rotation = 0.0f;

void onTick() {

}

void playerMove(World* world) {
    RenderedBody* plr = &world->player;

    rotation += rotationDirection;

    while (rotation < 0) {
        rotation += TPI;
    }
    while (rotation > TPI) {
        rotation -= TPI;
    }

    float x = cosf(rotation) * walkDirection;
    float y = sinf(rotation) * walkDirection;

    plr->body.moveDirection = Vector_new(x, y);

    moveAndSlide(plr, world);
}

void sight(World* world) {
    int amount = 30;
    int range = 250;
    float fov = PI / 2;
    RaycastHit rays[amount];

    float step = fov / amount * 2;

    Vect facing = Vector_new(cosf(rotation + fov) * range, sinf(rotation + fov) * range);

    // blast a stupid amount of rays to check for walls
    for (int i = 0; i < amount; i++) {
        for (int j = 0; j < world->walls.size; j++) {
            Edge wall = world->walls.data[j];
            RaycastHit ray = raycast(world->player.body.position, facing, wall);
        }
        fov -= step;
    }

    
}

void onRender(World* world) {
    playerMove(world);
    sight(world);
}

int onEvent(SDL_Event* event, World* world) {
    // pre instance rendered wall objects

    Vect plrMovementVect = world->player.body.moveDirection;
    
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_EVENT_QUIT) {
            return 1;
        } else if (event->type == SDL_EVENT_KEY_DOWN) {
            switch (event->key.key) {
                case SDLK_LEFT:
                    rotationDirection = -rotationSpeed;
                    break;
                case SDLK_RIGHT:
                    rotationDirection = rotationSpeed;
                    break;
                case SDLK_UP:
                    walkDirection = -1.0f;
                    break;
                case SDLK_DOWN:
                    walkDirection = 1.0f;
                    break;
                default:
                    break;
            }
        } else if (event->type == SDL_EVENT_KEY_UP) {
            SDL_Keycode key = event->key.key;

            if (key == SDLK_RIGHT && rotationDirection >= rotationSpeed) {
                rotationDirection = 0.0f;
            } else if (key == SDLK_LEFT && rotationDirection <= -rotationSpeed) {
                rotationDirection = 0.0f;
            } else if (key == SDLK_UP && plrMovementVect.y <= -1.0f) {
                walkDirection = 0.0f;
            } else if (key == SDLK_DOWN && plrMovementVect.y >= 1.0f) {
                walkDirection = 0.0f;
            }
        }
    }
    return 0;
}

// What you want to render
RenderQueue* passRenderQueue() {
    return &renderQueue;
}

void onWorldGen(World* world) {
    printf("Hello world!\n");

    renderQueue = createRenderQueue();
    renderQueuePushBack(&renderQueue, &world->player);

    EdgeArray colliders;
    colliders = createEdgeArray(0);

    // Test walls
    Edge edge1 = { .a = Vector_new(0, 0),     .b = Vector_new(200, 0) };    // Top wall
    Edge edge2 = { .a = Vector_new(200, 0),   .b = Vector_new(200, 200) };  // Right wall
    Edge edge3 = { .a = Vector_new(200, 200), .b = Vector_new(0, 200) };    // Bottom wall
    Edge edge4 = { .a = Vector_new(0, 200),   .b = Vector_new(0, 0) };      // Left wall

    edgeArrayPushBack(&colliders, edge1);
    edgeArrayPushBack(&colliders, edge2);
    edgeArrayPushBack(&colliders, edge3);
    edgeArrayPushBack(&colliders, edge4);

    world->walls = colliders;
}