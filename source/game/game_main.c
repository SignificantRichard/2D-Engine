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
#include "hud.h"

// Self
#include "game_main.h"

// C libraries
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <SDL3/SDL.h>

// envar
#define rotationSpeed 6.0f
#define RAY_AMOUNT 1280
#define FOV 1.22f
#define RANGE 1500

static RenderQueue renderQueue;
static RenderQueue hudQueue;

static float rotationDirection = 0.0f; // player rotation
static float walkDirection = 0.0f;
static float rotation = 0.0f;

static HUDInfo hud;

void onTick(World* world, uint32_t tick) {
    if (tick % 10 == 0) {
        hud.portrait.instance++;
        hud.portrait.portraits.body.position.y = -(float)(hud.portrait.instance) / 3.0f * 64 + INTERNAL_HEIGHT - 128;
        hud.portrait.portraits.renderPercentageStart = (float)hud.portrait.instance / 3.0f;
        hud.portrait.portraits.renderPercentage = ((float)hud.portrait.instance + 1.0f) / 3.0f;
        if (hud.portrait.instance == 3) hud.portrait.instance = 0;
    }
}

void playerMove(World* world, float dt) {
    RenderedBody* plr = &world->player;

    rotation += rotationDirection * dt;

    while (rotation < 0) {
        rotation += TPI;
    }
    while (rotation > TPI) {
        rotation -= TPI;
    }

    float x = cosf(rotation) * walkDirection;
    float y = sinf(rotation) * walkDirection;

    plr->body.moveDirection = Vector_new(x, y);

    moveAndSlide(plr, world, dt);
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
    int prevChangeWall = 0;

    // create a floor polygon
    Polygon floor;
    floor.a = Vector_new(0, INTERNAL_HEIGHT / 2);
    floor.b = Vector_new(INTERNAL_WIDTH, INTERNAL_HEIGHT / 2);
    floor.c = Vector_new(0, INTERNAL_HEIGHT);
    floor.d = Vector_new(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    floor.aSideBrightness = 0.0f;
    floor.cSideBrightness = 1.5f;
    floor.texture = "./source/res/images/alternate";
    polyArray.data[polyArray.used] = floor;
    polyArray.used++;
                

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
            float brightness = 1.0f - correctedDistance / RANGE;
            if (brightness < 0.0f) brightness = 0.0f;
            if (brightness > 1.0f) brightness = 1.0f;

            if (lastIndex != currentIndex) {
                Polygon p;
                p.a = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                p.b = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                p.c = Vector_new((float)(i + 1) / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                p.d = Vector_new((float)(i + 1) / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                p.aSideBrightness = brightness;
                p.cSideBrightness = brightness;
                p.texture = "./source/res/images/default";
                polyArray.data[polyArray.used] = p;
                polyArray.used++;
                prevChangeWall = 1;
            } else {
                if (polyArray.used - 2 > 1 && prevChangeWall) {
                    polyArray.data[polyArray.used - 2].c = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                    polyArray.data[polyArray.used - 2].d = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                }
                polyArray.data[polyArray.used - 1].c = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 + wallHeight));
                polyArray.data[polyArray.used - 1].d = Vector_new((float)i / RAY_AMOUNT * INTERNAL_WIDTH, (float)(INTERNAL_HEIGHT / 2 - wallHeight));
                polyArray.data[polyArray.used - 1].cSideBrightness = brightness;
                prevChangeWall = 0;
            }

        }

        lastIndex = currentIndex;
    }
    return polyArray;
}

PolygonArray onRender(World* world, float dt) {
    playerMove(world, dt);
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
    queues[1] = &hudQueue;
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
    hudBackground->body.position.y = INTERNAL_HEIGHT - 64;
    hudBackground->sprite = resample(hudBackground->sprite, INTERNAL_WIDTH, 100, 3);
    
    renderQueuePushBack(&hudQueue, hudBackground);
    renderQueuePushBack(&hudQueue, &hud.portrait.portraits);
    
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