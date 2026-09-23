#include <stdio.h>
#include <stdlib.h>

// Let me be clear -Barack Barack
#define true 1
#define false 0
#define print printf
#define bool int

// envars
#define DEFAULT_MAX_SPEED 16

// abstract vector operands
#define Vector_new createVector
#define Vector_add addVectors
#define Vector_mult multVectors
#define Vector_div diviVectors

// abstract body manipulation
#define Body_new createBody
#define body_speedLim setSpeedLimit
#define body_setAcc setBodyAccel
#define body_setVel setBodyVel
#define body_setPos setBodyPosition

// 2D representation of Vector
struct Vector_t {
    int x;
    int y;
};

typedef struct Vector_t Vect;

Vect createVector(int x, int y) {
    Vect vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

void addVectors(Vect* original, Vect* additor) {
    original->x += additor->x;
    original->y += additor->y;
}

void subVectors(Vect* original, Vect* subtractor) {
    original->x -= subtractor->x;
    original->y -= subtractor->y;
}

void multVectors(Vect* original, Vect* multiplier) {
    original->x *= multiplier->x;
    original->y *= multiplier->y;
}

void diviVectors(Vect* original, Vect* dividor) {
    original->x /= dividor->x;
    original->y /= dividor->y;
}

struct Body_t {
    Vect position;
    Vect velocity;
    Vect acceleration;
    int maxSpeed;
};

typedef struct Body_t Body;

Body createBody() {
    Body body;

    // define sub stuff
    Vect position;
    position = Vector_new(0, 0);
    Vect acceleration;
    acceleration = Vector_new(0, 0);
    
    body.position = position;
    body.acceleration = acceleration;
    body.maxSpeed = DEFAULT_MAX_SPEED;

    return body;
}

void setSpeedLimit(Body* body, int maxSpeed) {
    body->maxSpeed = maxSpeed;
}

void setBodyPosition(Body* body, int x, int y) {
    body->position.x = x;
    body->position.y = y;
}

void setBodyAccel(Body* body, int x, int y) {
    body->acceleration.x = x;
    body->acceleration.y = y;
}

void setBodyPos(Body* body, int x, int y) {
    body->velocity.x = x;
    body->velocity.y = y;
}

// lines of intersection or some shit
struct collisionBuffer() {

}

int main() {
    Vect vec;
    vec = Vector_new(10, 10);
    
    return 0;
}