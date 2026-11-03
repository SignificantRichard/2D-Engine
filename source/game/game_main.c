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

static RenderQueue renderQueue;

void onTick(World* world) {

}

void playerMove(World* world) {
    RenderedBody* plr = &world->player;
    moveAndSlide(plr, world);
}

void onRender(World* world) {
    playerMove(world);
}

int onEvent(SDL_Event* event, World* world) {
    Vect plrMovementVect = world->player.body.moveDirection;
    
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_EVENT_QUIT) {
            return 1;
        } else if (event->type == SDL_EVENT_KEY_DOWN) {
            switch (event->key.key) {
                case SDLK_UP:
                    plrMovementVect.y = -1.0f;
                    break;
                case SDLK_DOWN:
                    plrMovementVect.y = 1.0f;
                    break;
                case SDLK_LEFT:
                    plrMovementVect.x = -1.0f;
                    break;
                case SDLK_RIGHT:
                    plrMovementVect.x = 1.0f;
                    break;
                default:
                    break;
            }
            world->player.body.moveDirection = plrMovementVect;
        } else if (event->type == SDL_EVENT_KEY_UP) {
            SDL_Keycode key = event->key.key;

            if (key == SDLK_RIGHT && plrMovementVect.x >= 1.0f) {
                plrMovementVect.x = 0.0f;
            } else if (key == SDLK_LEFT && plrMovementVect.x <= -1.0f) {
                plrMovementVect.x = 0.0f;
            } else if (key == SDLK_UP && plrMovementVect.y <= -1.0f) {
                plrMovementVect.y = 0.0f;
            } else if (key == SDLK_DOWN && plrMovementVect.y >= 1.0f) {
                plrMovementVect.y = 0.0f;
            }
            world->player.body.moveDirection = plrMovementVect;
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