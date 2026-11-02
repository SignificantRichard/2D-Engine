#ifndef BODY_H
#define BODY_H

#include "vector.h"
#include "image.h"

#define Body_new createBody
#define body_speedLim setSpeedLimit
#define body_setAcc setBodyAccel
#define body_setVel setBodyVel
#define body_setPos setBodyPosition

typedef struct {
    Vect position;
    Vect velocity;
    Vect acceleration;
    Vect moveDirection; // attempting to set accelerate to here
    int maxSpeed;
} Body;

// abstract body manipulation
Body createBody();
void setSpeedLimit(Body* body, int maxSpeed);
void setBodyPosition(Body* body, float x, float y);
void setBodyAccel(Body* body, float x, float y);
void setBodyPos(Body* body, float x, float y);

typedef struct {
    Sprite sprite;
    Vect position;
} StaticBody;

typedef struct {
    Sprite sprite; // what is drawn
    Body body; // movement
    int alpha; // 255, set to 0 to hide
} RenderedBody;

RenderedBody createRenderedBody(char* spriteFilePath);

#endif