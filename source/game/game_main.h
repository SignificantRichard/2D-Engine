#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "../engine/world.h"
#include "../engine/image.h"
#include "../engine/memory.h"
#include "../engine/obama.h"
#include "../engine/vector.h"
#include "../engine/body.h" 
#include "../engine/edge.h"
#include "../engine/face.h"
#include "../engine/files.h"
#include "../engine/sound.h"
#include "../engine/renderqueue.h"

#include <SDL3/SDL.h>

void onTick();
void onRender(World* world);
int onEvent(SDL_Event* event, World* world, Vect* plrMovementVect);
void onWorldGen(World* world);
RenderQueue* passRenderQueue();

#endif