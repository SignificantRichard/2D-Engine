#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unistd.h>
#include <math.h>
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

int GrabPixel(SDL_Surface* surface, Uint32 x, Uint32 y) {
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
    Uint32 color = pixels[y * pitchInPixels + x];

    // Unlock surface when finished
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }
    return color;
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
        
        uint32_t color = getRenderPixel(sprite, i, 0); // same as x, y
        int r = getColorFragment(color, 3);
        int g = getColorFragment(color, 2);
        int b = getColorFragment(color, 1);
        int a = getColorFragment(color, 0);

        ModifyPixel(surface, x + localX, y + localY, r, g, b, a);
    }
}

void drawSpriteFromBody(SDL_Surface* surface, int x, int y, RenderedBody* renderedBody) {
    Sprite sprite = renderedBody->sprite;
    int startAt = sprite.height * renderedBody->renderPercentageStart; 
    int stopAt = sprite.height * renderedBody->renderPercentage;
    for (int i = 0; i < sprite.size; i++) {

        int localX = i % sprite.width;
        int localY = i / sprite.width;

        if (localY >= stopAt) {
            break;
        } else if (localY < startAt) {
            continue;
        }
        
        uint32_t color = getRenderPixel(&sprite, i, 0); // same as x, y
        int r = getColorFragment(color, 3);
        int g = getColorFragment(color, 2);
        int b = getColorFragment(color, 1);
        int a = getColorFragment(color, 0);

        if (a == 0) {
            continue;
        } else if (a != 255) {
            // time to blend
            float perc = a / 255;
            Uint32 baseColor = GrabPixel(surface, x + localX, y + localY);
            int r = clampColor(getColorFragment(color, 3) * perc + getColorFragment(baseColor, 3) * (1.0f - perc));
            int g = clampColor(getColorFragment(color, 2) * perc + getColorFragment(baseColor, 2) * (1.0f - perc));
            int b = clampColor(getColorFragment(color, 1) * perc + getColorFragment(baseColor, 1) * (1.0f - perc));
            int a = clampColor(getColorFragment(color, 0) * perc + getColorFragment(baseColor, 0) * (1.0f - perc));
            color = makeColor(r, g, b, a);
        }

        ModifyPixel(surface, x + localX, y + localY, r, g, b, a);
    }
}

void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, int r, int g, int b, int a) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        ModifyPixel(surface, x0, y0, r, g, b, a);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Do game checks here
void tickHit(World* world) {
    onTick(world);
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

    // Call exposed game stuff
    onWorldGen(&world);

    while (1) {
        Uint64 frameStart = SDL_GetTicks(); // will last ~1x10^10 years at 60 fps. Cpu would be atomized before then

        SDL_Event event;

        // Call exposed game stuff
        if (onEvent(&event, &world))
            break;
        
       PolygonArray polyArray = onRender(&world);

        for (int i = 0; i < polyArray.used; i++) {
            Polygon poly = polyArray.data[i];
            Sprite texture = loadImageAsSprite(poly.texture);

            if (texture.data != NULL) { // I will cry if this breaks
                int min_y = (int)fminf(fminf(poly.c.y, poly.d.y), fminf(poly.a.y, poly.b.y));
                int max_y = (int)fmaxf(fmaxf(poly.c.y, poly.d.y), fmaxf(poly.a.y, poly.b.y));

                if (min_y < 0) min_y = 0;
                if (max_y >= INTERNAL_HEIGHT) max_y = INTERNAL_HEIGHT - 1;

                for (int y = min_y; y <= max_y; y++) {
                    float left_x = (float)INTERNAL_WIDTH;
                    float right_x = -1.0f;
                    float u_left = 0.0f, v_left = 0.0f;
                    float u_right = 0.0f, v_right = 0.0f;

                    float dy_ca = poly.a.y - poly.c.y;
                    if (dy_ca != 0.0f && y >= fminf(poly.c.y, poly.a.y) && y <= fmaxf(poly.c.y, poly.a.y)) {
                        float t = (float)(y - poly.c.y) / dy_ca;
                        float x = poly.c.x + t * (poly.a.x - poly.c.x);
                        if (x < left_x) { left_x = x; u_left = 0.0f; v_left = t; }
                        if (x > right_x) { right_x = x; u_right = 0.0f; v_right = t; }
                    }

                    float dy_db = poly.b.y - poly.d.y;
                    if (dy_db != 0.0f && y >= fminf(poly.d.y, poly.b.y) && y <= fmaxf(poly.d.y, poly.b.y)) {
                        float t = (float)(y - poly.d.y) / dy_db;
                        float x = poly.d.x + t * (poly.b.x - poly.d.x);
                        if (x < left_x) { left_x = x; u_left = 1.0f; v_left = t; }
                        if (x > right_x) { right_x = x; u_right = 1.0f; v_right = t; }
                    }

                    float dy_cd = poly.d.y - poly.c.y;
                    if (dy_cd != 0.0f && y >= fminf(poly.c.y, poly.d.y) && y <= fmaxf(poly.c.y, poly.d.y)) {
                        float t = (float)(y - poly.c.y) / dy_cd;
                        float x = poly.c.x + t * (poly.d.x - poly.c.x);
                        if (x < left_x) { left_x = x; u_left = t; v_left = 0.0f; }
                        if (x > right_x) { right_x = x; u_right = t; v_right = 0.0f; }
                    }

                    float dy_ab = poly.b.y - poly.a.y;
                    if (dy_ab != 0.0f && y >= fminf(poly.a.y, poly.b.y) && y <= fmaxf(poly.a.y, poly.b.y)) {
                        float t = (float)(y - poly.a.y) / dy_ab;
                        float x = poly.a.x + t * (poly.b.x - poly.a.x);
                        if (x < left_x) { left_x = x; u_left = t; v_left = 1.0f; }
                        if (x > right_x) { right_x = x; u_right = t; v_left = 1.0f; }
                    }

                    int start_x = (int)fmaxf(0.0f, ceilf(left_x));
                    int end_x = (int)fminf((float)(INTERNAL_WIDTH - 1), floorf(right_x));

                    for (int x = start_x; x <= end_x; x++) {
                        float span = (right_x > left_x) ? (float)(x - left_x) / (right_x - left_x) : 0.0f;
                        float u_tex = u_left + span * (u_right - u_left);
                        float v_tex = v_left + span * (v_right - v_left);

                        float tileFactorX = (end_x - start_x) / (float)texture.width;
                        float tileFactorY = (max_y - min_y) / (float)texture.height;

                        float tiled_u = fabsf(fmodf(u_tex * tileFactorX, 1.0f));
                        float tiled_v = fabsf(fmodf(v_tex * tileFactorY, 1.0f));

                        int tex_x = (int)(tiled_u * (texture.width - 1));
                        int tex_y = (int)(tiled_v * (texture.height - 1));

                        if (tex_x < 0) tex_x = 0;
                        if (tex_x >= texture.width) tex_x = texture.width - 1;
                        if (tex_y < 0) tex_y = 0;
                        if (tex_y >= texture.height) tex_y = texture.height - 1;

                        int tex_idx = tex_y * texture.width + tex_x;
                        uint32_t color = getRenderPixel(&texture, tex_idx, 0);

                        int r = getColorFragment(color, 3);
                        int g = getColorFragment(color, 2);
                        int b = getColorFragment(color, 1);
                        int a = getColorFragment(color, 0);

                        if (a == 0) continue;
                        if (a != 255) {
                            float perc = a / 255.0f;
                            Uint32 baseColor = GrabPixel(surface, x, y);
                            r = (int)(r * perc + getColorFragment(baseColor, 3) * (1.0f - perc));
                            g = (int)(g * perc + getColorFragment(baseColor, 2) * (1.0f - perc));
                            b = (int)(b * perc + getColorFragment(baseColor, 1) * (1.0f - perc));
                            a = (int)(a * perc + getColorFragment(baseColor, 0) * (1.0f - perc));
                        }

                        float currentBrightness = poly.aSideBrightness + span * (poly.cSideBrightness - poly.aSideBrightness);
                        r = clampColor(r * currentBrightness);
                        g = clampColor(g * currentBrightness);
                        b = clampColor(b * currentBrightness);

                        ModifyPixel(surface, x, y, r, g, b, a);
                    }
                }
            }

            // Keep your wireframe debug lines to verify alignment
            drawLine(surface, poly.a.x, poly.a.y, poly.b.x, poly.b.y, 255, 255, 255, 255);
            drawLine(surface, poly.d.x, poly.d.y, poly.c.x, poly.c.y, 255, 255, 255, 255);
            drawLine(surface, poly.a.x, poly.a.y, poly.c.x, poly.c.y, 255, 255, 255, 255);
            drawLine(surface, poly.d.x, poly.d.y, poly.b.x, poly.b.y, 255, 255, 255, 255);
            drawLine(surface, poly.a.x, poly.a.y, poly.d.x, poly.d.y, 255, 255, 255, 255);
            drawLine(surface, poly.c.x, poly.c.y, poly.b.x, poly.b.y, 255, 255, 255, 255);
        }

        RenderQueue** renderQueues = passRenderQueue();
        RenderQueue* renderQueue = renderQueues[0];
        RenderQueue* hudQueue = renderQueues[1];

        for (int i = 0; i < renderQueue->size; i++) {
            RenderedBody* body = renderQueue->sprites[i];
            drawSpriteFromBody(surface, body->body.position.x, body->body.position.y, body);
        }
        for (int i = 0; i < hudQueue->size; i++) {
            RenderedBody* body = hudQueue->sprites[i];
            drawSpriteFromBody(surface, body->body.position.x, body->body.position.y, body);
        }

        free(renderQueues);

        // render collisions (walls)
        // for (int i = 0; i < world.walls.size; i++) {
        //     Edge edge = world.walls.data[i];
        //     drawLine(surface, (int)edge.a.x, (int)edge.a.y, (int)edge.b.x, (int)edge.b.y, 255, 0, 255, 255);
        // }

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