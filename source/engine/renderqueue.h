#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H

#include "body.h"

typedef struct {
    RenderedBody** sprites;
    uint32_t size;
} RenderQueue;

RenderQueue createRenderQueue();
void renderQueuePushBack(RenderQueue* queue, RenderedBody* sprite);

#endif