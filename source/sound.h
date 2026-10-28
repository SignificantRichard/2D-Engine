#ifndef SOUND_H
#define SOUND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "sound.h"

typedef struct {
    char* filePath;
    Uint8* buffer;
    Uint32 length;
    SDL_AudioSpec spec;
    uint8_t volume;
} Sound;

static SDL_AudioDeviceID audio_device = 0;
static SDL_AudioSpec audio_spec;

int initAudio(void);
int destroyAudio(void);
Sound loadSound(const char* filePath);
void playSound(Sound* sound);
void destroySound(Sound* sound);

#endif