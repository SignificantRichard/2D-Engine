#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "image.h"
#include "files.h"

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

Sprite loadImageAsSprite(char* path) {
    BinaryFileContents imageData = readBinFromFile(path);
    uint16_t headerLength = (imageData.content[0] << 8) + imageData.content[1];

    uint8_t widthLength = imageData.content[2];
    uint8_t heightLength = imageData.content[3];

    uint16_t width = 0;
    uint16_t height = 0;

    int idx = 4;
    for (int i = 0; i < widthLength; i++) {
        width = (width << 8) + imageData.content[idx++];
    }
    for (int i = 0; i < heightLength; i++) {
        height = (height << 8) + imageData.content[idx++];
    }

    Sprite sprite = constructSprite(width, height);

    // Use a separate pixel index counter
    int pixelIndex = 0;
    for (int i = headerLength; i < (height * width * 3) + headerLength; i += 3) {
        if (pixelIndex >= sprite.size) break;

        uint32_t color = makeColor(
            imageData.content[i],
            imageData.content[i + 1],
            imageData.content[i + 2],
            255
        );

        int x = pixelIndex % width;
        int y = pixelIndex / width;
        setPixel(&sprite, x, y, color);
        pixelIndex++;
    }

    free(imageData.content);

    return sprite;
}