#include "nrf.h"
#include "external/timecake/lcd_font.c"
#include "display.h"
#include "display_print.h"
#include <math.h>

void drawChar (int x, int y, char character, uint16_t color_text, uint16_t color_bg) {

    uint8_t frame[(8*16 / 8)];

    for (int charY = 0; charY < 16; charY++) {
        uint8_t pixelByte = 0;
        for (int charX = 0; charX < 6; charX++) {
            if ((funfont_6x16r[((character - 32) * 6) + charX] >> (16 - charY)) & 1) {
                pixelByte |= 1 << (charX + 1);
            }
        }


        frame[charY] = pixelByte;
    }

    drawmono(x, y, x + 8 - 1, y + 16 - 1, frame, color_text, color_bg);
}

void drawString (int x, int y, char* text, uint16_t color_text, uint16_t color_bg) {
    _Bool terminated = 0;
    int i = 0;

    while (!terminated) {
        if (text[i] == '\0') {
            terminated = 1;
        } else {
            drawChar (x + i*8, y, text[i], color_text, color_bg);
            i++;
        }
    }
}

void drawNumber (int x, int y, int number, uint16_t color_text, uint16_t color_bg, int clearLength) {
    int i = 0;
    while (number > 0) {
        drawChar (x - i*8, y, number % 10 + 48, color_text, color_bg);
        number /= 10;
        i++;
    }
    while (clearLength > i) {
        drawChar (x - i*8, y, 32, color_text, color_bg);
        i++;
    }
}

void drawSelection (int x, int y, char* text, uint16_t color_text, uint16_t color_bg, int selection, _Bool vertical) {
    _Bool terminated = 0;
    int i = 0;

    while (!terminated) {
        if (text[i] == '\0') {
            terminated = 1;
        } else {
            if (i == selection) {
                drawChar (x + i*8*!vertical, y + 16*i*vertical, text[i], color_bg, color_text);
            } else {
                drawChar (x + i*8*!vertical, y + 16*i*vertical, text[i], color_text, color_bg);
            }
            i++;
        }
    }
}
