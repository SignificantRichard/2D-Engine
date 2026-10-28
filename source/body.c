#include "body.h"
#include "vector.h"
#include "obama.h"

// abstract body manipulation
Body createBody() {
    Body body;
    
    body.position = Vector_new(0, 0);
    body.acceleration = Vector_new(0, 0);
    body.moveDirection = Vector_new(0, 0);
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