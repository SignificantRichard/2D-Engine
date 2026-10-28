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

// Self
#include "game_main.h"

// C libraries
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <SDL3/SDL.h>

static RenderQueue renderQueue;

void onTick() {

}

void onRender(World* world) {
    int curVel = sqrt(pow(world->player.body.velocity.x, 2) + pow(world->player.body.velocity.y, 2));
    if (curVel <= world->player.body.maxSpeed) {
        Vector_add(&(world->player.body.velocity), &(world->player.body.acceleration));
    }

    // Apply friction/deceleration if no input
    if (world->player.body.acceleration.x == 0) {
        world->player.body.velocity.x *= 0.75;
    }
    if (world->player.body.acceleration.y == 0) {
        world->player.body.velocity.y *= 0.75;
    }

    // Add input direction to acceleration
    world->player.body.acceleration = world->player.body.moveDirection;

    world->player.body.position.x += world->player.body.velocity.x;
    for (int i = 0; i < world->walls.size; i++) {
        Edge collider = world->walls.data[i];
        // Create a bounding check or raycast from slightly behind the player
        Vect rayVec = { .x = world->player.body.velocity.x, .y = 0 };
        RaycastHit ray = raycast(world->player.body.position, rayVec, collider);
        if (ray.hit && ray.distance <= 16.0f) { // 4.0f acts as a small player-radius buffer
            world->player.body.position.x -= world->player.body.velocity.x;
            world->player.body.velocity.x = 0;
        }
    }

    world->player.body.position.y += world->player.body.velocity.y;
    for (int i = 0; i < world->walls.size; i++) {
        Edge collider = world->walls.data[i];
        Vect rayVec = { .x = 0, .y = world->player.body.velocity.y };
        RaycastHit ray = raycast(world->player.body.position, rayVec, collider);
        if (ray.hit && ray.distance <= 16.0f) {
            world->player.body.position.y -= world->player.body.velocity.y;
            world->player.body.velocity.y = 0;
        }
    }
}

int onEvent(SDL_Event* event, World* world, Vect* plrMovementVect) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_EVENT_QUIT) {
            return 1;
        } else if (event->type == SDL_EVENT_KEY_DOWN) {

            switch (event->key.key) {
                case SDLK_UP:
                    plrMovementVect->y = -1;
                    break;
                case SDLK_DOWN:
                    plrMovementVect->y = 1;
                    break;
                case SDLK_LEFT:
                    plrMovementVect->x = -1;
                    break;
                case SDLK_RIGHT:
                    plrMovementVect->x = 1;
                    break;
                default:
                    break;
            }
            world->player.body.moveDirection = *plrMovementVect;
        } else if (event->type == SDL_EVENT_KEY_UP) {

            SDL_Keycode key = event->key.key;

            if (key == SDLK_RIGHT && plrMovementVect->x == 1) {
                plrMovementVect->x = 0;
            } else if (key == SDLK_LEFT && plrMovementVect->x == -1) {
                plrMovementVect->x = 0;
            } else if (key == SDLK_UP && plrMovementVect->y == -1) {
                plrMovementVect->y = 0;
            } else if (key == SDLK_DOWN && plrMovementVect->y == 1) {
                plrMovementVect->y = 0;
            }
            world->player.body.moveDirection = *plrMovementVect;
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
}