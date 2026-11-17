#ifndef HUD_H
#define HUD_H

#include "../engine/body.h"

typedef struct {
    int bullet;
    int shell;
    int rocket;
} Ammunition;

typedef struct {
    uint8_t instance;
    RenderedBody portraits;
} Portrait;

typedef struct {
    Ammunition ammo;
    uint8_t* health;
    uint8_t* armor;
    Portrait portrait;
} HUDInfo;


Ammunition createAmmunition();
Portrait createPortrait();
HUDInfo createHUD();
void changePortrait(HUDInfo* hud, int portraitNumber);

#endif