#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "files.h"

typedef struct {
    uint32_t* data;
    int width;
    int height;
    int size;
} Sprite;

Sprite constructSprite(int width, int height);
uint32_t getPixel(Sprite* sprite, int x, int y);
void setPixel(Sprite* sprite, int x, int y, uint32_t color);
uint32_t makeColor(int r, int g, int b, int a);
int getColorFragment(uint32_t color, int place);
Sprite loadImageAsSprite(char* path);

#endif