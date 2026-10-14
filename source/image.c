#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "image.h"

Sprite constructSprite(int width, int height) {
    Sprite sprite;
    sprite.width = width;
    sprite.height = height;
    sprite.size = width * height;
    
    sprite.data = (uint32_t*)malloc(sprite.size * sizeof(uint32_t));

    for (int i = 0; i < sprite.size; i++) {
        sprite.data[i] = makeColor(255, 255, 255, 255);
    }

    return sprite;
}

uint32_t getPixel(Sprite* sprite, int x, int y) {
    return sprite->data[y * sprite->width + x];
}

void setPixel(Sprite* sprite, int x, int y, uint32_t color) {
    sprite->data[y * sprite->width + x] = color;
}

uint32_t makeColor(int r, int g, int b, int a) {
    uint32_t color = 0;
    
    r <<= 24;
    g <<= 16;
    b <<= 8;
    r |= g;
    r |= b;
    r |= a;

    color |= r;
    return color;
}

int getColorFragment(uint32_t color, int place) {
    // 0a 1g 2b 3a
    color >>= place * 8;
    // mask
    color &= 0x000000ff;
    return (int)color;
}
