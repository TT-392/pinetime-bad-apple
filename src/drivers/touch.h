#pragma once
#include "nrf.h"

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
};

int touch_init();

int touch_refresh(struct touchPoints* touchPoint);
