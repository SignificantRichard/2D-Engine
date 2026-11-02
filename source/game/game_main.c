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

void playerMove(World* world) {
    RenderedBody* plr = &world->player;
    Body* body = &plr->body;
    
    if (body->moveDirection.x == 0.0f) {
        body->velocity.x *= 0.8f;
    }
    if (body->moveDirection.y == 0.0f) {
        body->velocity.y *= 0.8f;
    }

    body->acceleration = body->moveDirection;
    
    body->velocity.x += body->acceleration.x;
    body->velocity.y += body->acceleration.y;
    
    float maxSpeedF = (float)body->maxSpeed;
    float speedSq = body->velocity.x * body->velocity.x + body->velocity.y * body->velocity.y;
    if (speedSq > maxSpeedF * maxSpeedF) {
        float speed = sqrtf(speedSq);
        body->velocity.x = (body->velocity.x / speed) * maxSpeedF;
        body->velocity.y = (body->velocity.y / speed) * maxSpeedF;
    }
    
    Vect displacement = body->velocity;
    if (displacement.x == 0.0f && displacement.y == 0.0f) {
        return;
    }

    Vect currentPos = body->position;
    int hitOccurred = 0;

    for (int iteration = 0; iteration < 2; iteration++) {
        float totalDispLen = sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);
        if (totalDispLen == 0.0f) break;

        float closestDistNorm = 1.0f;
        Edge hitEdge = {0};

        for (int i = 0; i < world->walls.size; i++) {
            Edge wall = world->walls.data[i];
            RaycastHit hit = raycast(currentPos, displacement, wall); // Uses raycast[cite: 3]
            
            if (hit.hit) {
                float distNorm = hit.distance / totalDispLen;
                if (distNorm >= 0.0f && distNorm <= 1.0f && distNorm < closestDistNorm) {
                    closestDistNorm = distNorm;
                    hitOccurred = 1;
                    hitEdge = wall;
                }
            }
        }

        if (hitOccurred) {
            float safeFraction = (closestDistNorm > 0.01f) ? (closestDistNorm - 0.01f) : 0.0f;
            Vect safePoint = createVector(
                currentPos.x + displacement.x * safeFraction,
                currentPos.y + displacement.y * safeFraction
            );
            
            float remainingFraction = 1.0f - closestDistNorm;
            Vect remainingMove = createVector(
                displacement.x * remainingFraction, 
                displacement.y * remainingFraction
            );

            Vect edgeVec = createVector(hitEdge.b.x - hitEdge.a.x, hitEdge.b.y - hitEdge.a.y);
            float edgeLen = sqrtf(edgeVec.x * edgeVec.x + edgeVec.y * edgeVec.y);
            
            if (edgeLen > 0.0f) {
                Vect edgeDir = createVector(edgeVec.x / edgeLen, edgeVec.y / edgeLen);
                
                float dot = remainingMove.x * edgeDir.x + remainingMove.y * edgeDir.y;
                displacement = createVector(edgeDir.x * dot, edgeDir.y * dot);
                currentPos = safePoint;
            } else {
                currentPos = safePoint;
                displacement = createVector(0.0f, 0.0f);
                break;
            }
        } else {
            currentPos.x += displacement.x;
            currentPos.y += displacement.y;
            break;
        }
    }

    body->position = currentPos;
    
    if (hitOccurred) {
        body->velocity = displacement;
    }
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