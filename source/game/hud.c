#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../engine/image.h"
#include "../engine/body.h"
#include "../engine/obama.h"
#include "hud.h"

Ammunition createAmmunition() {
    Ammunition ammunition = {
        .bullet = 0,
        .shell = 0,
        .rocket = 0
    };
    return ammunition;
}

Portrait createPortrait() {
    Portrait portrait = {
        .instance = 0
    };
    portrait.portraits = createRenderedBody("./source/res/images/face");
    return portrait;
}

HUDInfo createHUD() {
    HUDInfo hud = {
        .ammo = createAmmunition(),
        .portrait = createPortrait()
    };
    hud.portrait.portraits.sprite = resample(hud.portrait.portraits.sprite, 100, 300, 0);
    changePortrait(&hud, 1);
    return hud;
}

void changePortrait(HUDInfo* hud, int portraitNumber) {
    float height = hud->portrait.portraits.sprite.height;
    hud->portrait.portraits.renderPercentageStart = (height / 3.0f * portraitNumber) / height;
    hud->portrait.portraits.renderPercentage = (height / 3.0f * (portraitNumber + 1)) / height;
    hud->portrait.portraits.body.position.y -= height / 3.0f * portraitNumber;
}