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
#include "../engine/polygon.h"

// game specific stuff
#include "wall.h"

// Self
#include "game_main.h"

// C libraries
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <SDL3/SDL.h>

// envar
#define rotationSpeed 0.1f
#define RAY_AMOUNT 320
#define FOV 1.22f
#define RANGE 1000

static RenderQueue renderQueue;

static float rotationDirection = 0.0f; // player rotation
static float walkDirection = 0.0f;
static float rotation = 0.0f;

static HUDInfo hud;

void onTick(World* world) {

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

float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

PolygonArray sight(World* world) {

    PolygonArray polyArray = createPolygonArray(RAY_AMOUNT);

    // Spread rays evenly across the field of view (FOV) centered on 'rotation'
    float startAngle = rotation - (FOV / 2.0f);
    float step = FOV / (float)RAY_AMOUNT;

    int lastIndex = -1;

    for (int i = 0; i < RAY_AMOUNT; i++) {
        float rayAngle = startAngle + (i * step);

        // Recalculate facing direction vector for EACH ray
        Vect facing = Vector_new(cosf(rayAngle) * RANGE, sinf(rayAngle) * RANGE);

        float minDistance = 1e9f; // Track closest intersection
        int hitAny = 0;

        RaycastHit outputRay;

        int currentIndex = 0;
        for (int j = 0; j < world->walls.size; j++) {
            Edge wall = world->walls.data[j];
            RaycastHit ray = raycast(world->player.body.position, facing, wall);

            if (ray.hit && ray.distance < minDistance) {
                minDistance = ray.distance;
                hitAny = 1;
                outputRay = ray;
                currentIndex = j;
            }
        }

        if (hitAny) {

            float correctedDistance = outputRay.distance * cosf(rayAngle - rotation);
            float wallHeight = (1.25f - correctedDistance / RANGE) * INTERNAL_HEIGHT / 2;

            if (lastIndex != currentIndex) {
                Polygon p;
                p.a = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                p.b = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                p.c = Vector_new((float)(i + 1) / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                p.d = Vector_new((float)(i + 1) / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                polyArray.data[polyArray.used] = p;
                
                polyArray.used++;
            } else {
                polyArray.data[polyArray.used - 1].c = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                polyArray.data[polyArray.used - 1].d = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
            }

        }

        lastIndex = currentIndex;
    }
    return polyArray;
}

PolygonArray onRender(World* world) {
    playerMove(world);
    return sight(world);
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
                    walkDirection = 1.0f;
                    break;
                case SDLK_DOWN:
                    walkDirection = -1.0f;
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
            } else if (key == SDLK_UP && walkDirection >= 1.0f) {
                walkDirection = 0.0f;
            } else if (key == SDLK_DOWN && walkDirection <= -1.0f) {
                walkDirection = 0.0f;
            }
        }
    }
    return 0;
}

// What you want to render
RenderQueue** passRenderQueue() {
    RenderQueue** queues = malloc(sizeof(RenderQueue*) * 2);
    queues[0] = &renderQueue;
    return queues;
}

void onWorldGen(World* world) {
    printf("Hello world!\n");

    renderQueue = createRenderQueue();
    hud = createHUD();

    // Allocate on the heap so it persists after onWorldGen returns
    RenderedBody* hudBackground = malloc(sizeof(RenderedBody));
    *hudBackground = createRenderedBody("./source/res/images/hud");
    hudBackground->body.position.x = 0;
    hudBackground->body.position.y = 0;
    hudBackground->sprite = resample(hudBackground->sprite, INTERNAL_WIDTH, 100, 3);
    
    // renderQueuePushBack(&hudQueue, hudBackground);
    // renderQueuePushBack(&hudQueue, &hud.portrait.portraits);
    
    EdgeArray colliders;
    colliders = createEdgeArray(0);
    
    // Test walls
    Wall wall1 = createWall();
    Wall wall2 = createWall();
    Wall wall3 = createWall();
    Wall wall4 = createWall();
    Edge edge1 = { .a = Vector_new(0, 0), .b = Vector_new(1000, 0), .wall=wall1}; // Top wall
    Edge edge2 = { .a = Vector_new(1000, 0), .b = Vector_new(1000, 1000), .wall=wall2}; // Right wall
    Edge edge3 = { .a = Vector_new(1000, 1000), .b = Vector_new(0, 1000), .wall=wall3}; // Bottom wall
    Edge edge4 = { .a = Vector_new(0, 1000), .b = Vector_new(0, 0), .wall=wall4}; // Left wall

    edgeArrayPushBack(&colliders, edge1);
    edgeArrayPushBack(&colliders, edge2);
    edgeArrayPushBack(&colliders, edge3);
    edgeArrayPushBack(&colliders, edge4);

    world->walls = colliders;

    world->player.sprite.brightness = 10;
}