// wall.h
#ifndef WALL_H
#define WALL_H

typedef struct {
    char* texture; // file name
    int height; // how tall is the wall
    int voffset;
    int lowCutoff; // the low point of a window
    int highCutoff; // the high point of a window
    Edge* parent;
} Wall;

Wall createWall(Edge* parent);
void repurposeWall(Wall* wall, char* texture, int height, int voffset, int lowCutoff, int highCutoff);

#endif  // WALL_H