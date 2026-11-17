#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "renderqueue.h"
#include "polygon.h"
#include "image.h"
#include "../game/game_main.h"

static inline uint8_t clampColor(float val) {
    if (val < 0.0f) return 0;
    if (val > 255.0f) return 255;
    return (uint8_t)val;
}

World initEngine() {
    World world;
    // calloc MAX_ELEMENTS_IN_WORLD elements because I can't be bothered
    world.bodies = (RenderedBody*)calloc(MAX_ELEMENTS_IN_WORLD, sizeof(RenderedBody));
    
    world.player = createRenderedBody("./source/res/images/default");
    world.player.body.maxSpeed = 16;
    world.player.body.velocity = Vector_new(0, 0); // for some reason the player's velocity is not init
    world.player.body.position = Vector_new(20, 20);

    Camera camera;
    camera.target = &world.player.body;
    world.camera = camera;
    return world;
}

// Do game checks here
void tickHit(World* world, uint32_t tick) {
    onTick(world, tick);
}

int Work() {
    const Uint32 frameDelay = 1000 / FRAME_RATE;
    Uint32 frameCount = 0;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!SDL_CreateWindowAndRenderer("Engine", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderLogicalPresentation(renderer, INTERNAL_WIDTH, INTERNAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    World world;
    world = initEngine();
    onWorldGen(&world);

    struct timespec start, end;
    double elapsed_time;
    double fps;

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        Uint64 frameStart = SDL_GetTicks();
        
        SDL_Event event;
        if (onEvent(&event, &world))
            break;
        if (++frameCount % TICK_RATE == 0)
            tickHit(&world, frameCount);
        
        PolygonArray polyArray = onRender(&world, elapsed_time);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // SDL_RenderClear(renderer);

        for (int i = 0; i < polyArray.used; i++) {
            Polygon poly = polyArray.data[i];
            Sprite sprite = loadImageAsSprite(poly.texture);
            
            if (sprite.data != NULL) {
                // Create a temporary surface to upload raw sprite pixels to an SDL_Texture on the GPU
                SDL_Surface* temp_surf = SDL_CreateSurfaceFrom(
                    sprite.width, sprite.height, 
                    SDL_PIXELFORMAT_RGBA8888, 
                    sprite.data, 
                    sprite.width * sizeof(uint32_t)
                );
                
                SDL_Texture* gpu_texture = SDL_CreateTextureFromSurface(renderer, temp_surf);
                SDL_DestroySurface(temp_surf);

                if (gpu_texture != NULL) {
                    SDL_SetTextureScaleMode(gpu_texture, SDL_SCALEMODE_NEAREST);

                    SDL_Vertex verts[4];

                    verts[0] = (SDL_Vertex){
                        {poly.a.x, poly.a.y},
                        {poly.aSideBrightness, poly.aSideBrightness, poly.aSideBrightness, 1.0f},
                        {0.0f, 0.0f}
                    };

                    verts[1] = (SDL_Vertex){
                        {poly.b.x, poly.b.y},
                        {poly.aSideBrightness, poly.aSideBrightness, poly.aSideBrightness, 1.0f},
                        {1.0f, 0.0f}
                    };

                    verts[2] = (SDL_Vertex){
                        {poly.c.x, poly.c.y},
                        {poly.cSideBrightness, poly.cSideBrightness, poly.cSideBrightness, 1.0f},
                        {0.0f, 1.0f}
                    };

                    verts[3] = (SDL_Vertex){
                        {poly.d.x, poly.d.y},
                        {poly.cSideBrightness, poly.cSideBrightness, poly.cSideBrightness, 1.0f},
                        {1.0f, 1.0f}
                    };

                    int indices[] = { 0, 1, 2,  1, 2, 3 };

                    SDL_RenderGeometry(renderer, gpu_texture, verts, 4, indices, 6);

                    SDL_DestroyTexture(gpu_texture);
                }

                // Free raw sprite pixel buffer loaded from file storage
                free(sprite.data);
            }

            // Optional: Hardware debug wireframe overlay lines
            // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            // SDL_RenderLine(renderer, poly.a.x, poly.a.y, poly.b.x, poly.b.y);
            // SDL_RenderLine(renderer, poly.d.x, poly.d.y, poly.c.x, poly.c.y);
            // SDL_RenderLine(renderer, poly.a.x, poly.a.y, poly.c.x, poly.c.y);
            // SDL_RenderLine(renderer, poly.d.x, poly.d.y, poly.b.x, poly.b.y);
            // SDL_RenderLine(renderer, poly.a.x, poly.a.y, poly.d.x, poly.d.y);
            // SDL_RenderLine(renderer, poly.c.x, poly.c.y, poly.b.x, poly.b.y);
        }

        RenderQueue** renderQueues = passRenderQueue();
        RenderQueue* renderQueue = renderQueues[0];
        RenderQueue* hudQueue = renderQueues[1];

        for (int q = 0; q < 2; q++) {
            RenderQueue* currentQueue = (q == 0) ? renderQueue : hudQueue;
            for (int i = 0; i < currentQueue->size; i++) {
                RenderedBody* body = currentQueue->sprites[i];
                Sprite sprite = body->sprite;
                
                if (sprite.data != NULL) {
                    SDL_Surface* s_surf = SDL_CreateSurfaceFrom(
                        sprite.width, sprite.height, 
                        SDL_PIXELFORMAT_RGBA8888, 
                        sprite.data, 
                        sprite.width * sizeof(uint32_t)
                    );
                    SDL_Texture* sprite_tex = SDL_CreateTextureFromSurface(renderer, s_surf);
                    SDL_DestroySurface(s_surf);

                    if (sprite_tex) {
                        int startY = (int)(sprite.height * body->renderPercentageStart);
                        int stopY  = (int)(sprite.height * body->renderPercentage);
                        int sliceHeight = stopY - startY;

                        if (sliceHeight > 0) {
                            SDL_FRect srcRect = {
                                0,
                                startY,
                                sprite.width,
                                sliceHeight
                            };
                            
                            SDL_FRect dstRect = {
                                (float)body->body.position.x,
                                (float)body->body.position.y + startY,
                                (float)sprite.width,
                                (float)sliceHeight
                            };

                            SDL_RenderTexture(renderer, sprite_tex, &srcRect, &dstRect);
                        }
                        
                        SDL_DestroyTexture(sprite_tex);
                    }
                }
            }
        }
        free(renderQueues);

        SDL_RenderPresent(renderer);

        // Frame rate limiter & calculation
        Uint64 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - (Uint32)frameTime);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        if (elapsed_time > 0.0) {
            fps = 1.0 / elapsed_time;
            printf("FPS: %.2f\r", fps);
            fflush(stdout);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main() {
    return Work();
}