#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <math.h>
#include "../game/game_main.h"

World initEngine() {
    World world;
    // calloc MAX_ELEMENTS_IN_WORLD elements because I can't be bothered
    world.bodies = (RenderedBody*)calloc(MAX_ELEMENTS_IN_WORLD, sizeof(RenderedBody));
    
    world.player = createRenderedBody();
    world.player.body.velocity = Vector_new(0, 0); // for some reason the player's velocity is not init

    EdgeArray colliders;
    colliders = createEdgeArray(0);

    world.walls = colliders;

    Camera camera;
    camera.target = &world.player.body;
    world.camera = camera;
    return world;
}

int ModifyPixel(SDL_Surface* surface, Uint32 x, Uint32 y, int r, int g, int b, int a) {
    if (x < 0 || y < 0 || x > INTERNAL_WIDTH || y > INTERNAL_HEIGHT) {
        return 1;
    }

    // Lock surface for direct pixel access
    if (SDL_MUSTLOCK(surface)) {
        if (!SDL_LockSurface(surface)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock surface: %s", SDL_GetError());
            return 1;
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
    return 0;
}

void ModifyPixels(SDL_Surface* surface, int x, int y, int w, int h, int r, int g, int b, int a) {
    for (int i = x; i < w+x; i++) {
        for (int j = y; j < h+y; j++) {
            ModifyPixel(surface, i, j, r, g, b, a);
        }
    }
}

void drawSprite(SDL_Surface* surface, int x, int y, Sprite* sprite) {
    for (int i = 0; i < sprite->size; i++) {
        int localX = i % sprite->width;
        int localY = i / sprite->width;
        
        uint32_t color = sprite->data[i];
        int r = getColorFragment(color, 3);
        int g = getColorFragment(color, 2);
        int b = getColorFragment(color, 1);
        int a = getColorFragment(color, 0);

        ModifyPixel(surface, x + localX, y + localY, r, g, b, a);
    }
}

// Do game checks here
void tickHit(World* world) {

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
    Vect plrMovementVect;
    plrMovementVect = Vector_new(0, 0);

    // Call exposed game stuff
    onWorldGen(&world);

    while (1) {
        Uint64 frameStart = SDL_GetTicks();

        SDL_Event event;

        // Call exposed game stuff
        onEvent(&event, &world, &plrMovementVect);
        onRender(&world);
        
        // draw player
        drawSprite(surface, world.player.body.position.x - 2, world.player.body.position.y - 2, &world.player.sprite);

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

        // clear all
        ModifyPixels(surface, 0, 0, INTERNAL_WIDTH, INTERNAL_HEIGHT, 0, 0, 0, 255);
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