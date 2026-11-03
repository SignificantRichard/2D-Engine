#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "wall.h"

/**
 * @brief Create a Wall object that has no point or texture data
 * 
 * @return Wall 
 */
Wall createWall() {
    Wall wall;
    wall.texture = "default";
    
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