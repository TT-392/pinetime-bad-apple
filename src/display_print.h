#pragma once

#include "nrf.h"

void drawChar (int x, int y, char character, uint16_t color_text, uint16_t color_bg, _Bool Alpha);


void drawString (int x, int y, char* text, uint16_t color_text, uint16_t color_bg);


void drawNumber (int x, int y, int number, uint16_t color_text, uint16_t color_bg, int clearLength, _Bool Alpha);


void drawSelection (int x, int y, char* text, uint16_t color_text, uint16_t color_bg, int selection, _Bool vertical);


int stringBMP (uint8_t* bitmap, char* text, int stringLength);
