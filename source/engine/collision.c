#include "collision.h"
#include "../engine/vector.h"
#include "../engine/edge.h"
#include "../engine/body.h"
#include "../engine/raycast.h"
#include "../engine/world.h"
#include <math.h>

void moveAndSlide(RenderedBody* renderedBody, World* world) {
    Body* body = &renderedBody->body;
    
    if (body->moveDirection.x == 0.0f) {
        body->velocity.x *= 0.8f;
    }
    if (body->moveDirection.y == 0.0f) {
        body->velocity.y *= 0.8f;
    }

    body->acceleration = body->moveDirection;
    
    body->velocity.x += body->acceleration.x;
    body->velocity.y += body->acceleration.y;
    
    float maxSpeedF = (float)body->maxSpeed;
    float speedSq = body->velocity.x * body->velocity.x + body->velocity.y * body->velocity.y;
    if (speedSq > maxSpeedF * maxSpeedF) {
        float speed = sqrtf(speedSq);
        body->velocity.x = (body->velocity.x / speed) * maxSpeedF;
        body->velocity.y = (body->velocity.y / speed) * maxSpeedF;
    }
    
    Vect displacement = body->velocity;
    if (displacement.x == 0.0f && displacement.y == 0.0f) {
        return;
    }

    Vect currentPos = body->position;
    int hitOccurred = 0;

    for (int iteration = 0; iteration < 2; iteration++) {
        float totalDispLen = sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);
        if (totalDispLen == 0.0f) break;

        float closestDistNorm = 1.0f;
        Edge hitEdge = {0};

        for (int i = 0; i < world->walls.size; i++) {
            Edge wall = world->walls.data[i];
            RaycastHit hit = raycast(currentPos, displacement, wall); // Uses raycast[cite: 3]
            
            if (hit.hit) {
                float distNorm = hit.distance / totalDispLen;
                if (distNorm >= 0.0f && distNorm <= 1.0f && distNorm < closestDistNorm) {
                    closestDistNorm = distNorm;
                    hitOccurred = 1;
                    hitEdge = wall;
                }
            }
        }

        if (hitOccurred) {
            float safeFraction = (closestDistNorm > 0.01f) ? (closestDistNorm - 0.01f) : 0.0f;
            Vect safePoint = createVector(
                currentPos.x + displacement.x * safeFraction,
                currentPos.y + displacement.y * safeFraction
            );
            
            float remainingFraction = 1.0f - closestDistNorm;
            Vect remainingMove = createVector(
                displacement.x * remainingFraction, 
                displacement.y * remainingFraction
            );

            Vect edgeVec = createVector(hitEdge.b.x - hitEdge.a.x, hitEdge.b.y - hitEdge.a.y);
            float edgeLen = sqrtf(edgeVec.x * edgeVec.x + edgeVec.y * edgeVec.y);
            
            if (edgeLen > 0.0f) {
                Vect edgeDir = createVector(edgeVec.x / edgeLen, edgeVec.y / edgeLen);
                
                float dot = remainingMove.x * edgeDir.x + remainingMove.y * edgeDir.y;
                displacement = createVector(edgeDir.x * dot, edgeDir.y * dot);
                currentPos = safePoint;
            } else {
                currentPos = safePoint;
                displacement = createVector(0.0f, 0.0f);
                break;
            }
        } else {
            currentPos.x += displacement.x;
            currentPos.y += displacement.y;
            break;
        }
    }

    body->position = currentPos;
    
    if (hitOccurred) {
        body->velocity = displacement;
    }
}