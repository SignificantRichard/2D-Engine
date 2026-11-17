#include "body.h"
#include "vector.h"
#include "obama.h"
#include "image.h"

// abstract body manipulation
Body createBody() {
    Body body;
    
    body.position = Vector_new(0.0f, 0.0f);
    body.acceleration = Vector_new(0.0f, 0.0f);
    body.moveDirection = Vector_new(0.0f, 0.0f);
    body.maxSpeed = DEFAULT_MAX_SPEED;

    return body;
}

void setSpeedLimit(Body* body, int maxSpeed) {
    body->maxSpeed = maxSpeed;
}

void setBodyPosition(Body* body, float x, float y) {
    body->position.x = x;
    body->position.y = y;
}

void setBodyAccel(Body* body, float x, float y) {
    body->acceleration.x = x;
    body->acceleration.y = y;
}

void setBodyPos(Body* body, float x, float y) {
    body->velocity.x = x;
    body->velocity.y = y;
}

RenderedBody createRenderedBody(char* spriteFilePath) {
    Body body;
    RenderedBody render;

    body = Body_new();
    render.body = body;
    
    Sprite sprite = loadImageAsSprite(spriteFilePath);
    render.sprite = sprite;
    render.renderPercentageStart = 0.0f;
    render.renderPercentage = 1.0f;

    return render;
}