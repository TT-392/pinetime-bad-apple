#include "nrf.h"
#include "display.h"
#include <math.h>
void drawCircle(int x, int y, int radius, uint16_t color) {
    for (int i = -radius; i <= radius; i++) {
        int halfLength = sqrt(radius*radius - i*i);
        drawSquare(x - halfLength, y + i, x + halfLength, y + i, color);
    }
}

