#pragma once
#include "nrf.h"

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
    uint8_t error;
    uint8_t tab;
    uint64_t touchTime;
};

int touch_init();

int touch_refresh(struct touchPoints* touchPoint);
