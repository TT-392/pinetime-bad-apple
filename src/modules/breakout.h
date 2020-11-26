#include <stdlib.h>

struct Ball {
    uint8_t x;
    uint8_t y;
    int vx;
    int vy;
};

void render_breakout (uint8_t bat, struct Ball* ball);
