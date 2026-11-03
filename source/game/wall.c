#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    char* texture; // file name
    int height; // how tall is the wall
    int voffset;
    int lowCutoff; // the low point of a window
    int highCutoff; // the high point of a window
    Edge* parent;
} Wall;

/**
 * @brief Create a Wall object that has no point or texture data
 * 
 * @return Wall 
 */
Wall createWall(Edge* parent) {
    Wall wall;
    wall.texture = "default";
    wall.parent = parent;

    return wall;
}

/**
 * @brief Repurposes walls
 * 
 * @param wall 
 * @param texture 
 * @param height 
 * @param lowCutoff -2 for no change -1 for no cut 
 * @param highCutoff -2 for no change -1 for no cut
 */
void repurposeWall(Wall* wall, char* texture, int height, int voffset, int lowCutoff, int highCutoff) {
    wall->texture = texture;
    wall->height = height;
    wall->voffset = voffset;
    if (lowCutoff != -2) {
        wall->lowCutoff = lowCutoff;
    }
    if (highCutoff != -2) {
        wall->highCutoff = highCutoff;
    }
}