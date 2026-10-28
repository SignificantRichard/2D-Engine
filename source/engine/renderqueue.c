#include "renderqueue.h"
#include "body.h"
#include <stdlib.h>
#include <stdio.h>

RenderQueue createRenderQueue() {
    RenderQueue queue;
    queue.size = 0;
    queue.sprites = (RenderedBody**)malloc(0);
    return queue;
}

void resizeRenderQueue(RenderQueue* queue, int newCapacity) {
    if (newCapacity == -1) {
        newCapacity = queue->size + 1;
    }

    RenderedBody** tmp = (RenderedBody**)realloc(queue->sprites, newCapacity * sizeof(RenderedBody*));

    if (!tmp) {
        printf("Failure to realloc");
        return;
    }

    queue->sprites = tmp;
    queue->size = newCapacity;
}

void renderQueuePushBack(RenderQueue* queue, RenderedBody* sprite) {
    resizeRenderQueue(queue, -1);
    queue->sprites[queue->size - 1] = sprite;
}

void renderQueueRemove(RenderQueue* queue, int position) {
    queue->sprites[position] = queue->sprites[queue->size - 1];
    resizeRenderQueue(queue, queue->size - 1);
}