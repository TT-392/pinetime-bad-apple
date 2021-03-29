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
    uint8_t fingerStatus; // 1 = debounced touch
    _Bool New;
    int errorCount;
};


int touch_init();

void touch_wake();
void touch_sleep();

int touch_refresh(struct touchPoints* touchPoint);

// only one can be subscribed for now
void subscribeTouchInterrupt(void function());


