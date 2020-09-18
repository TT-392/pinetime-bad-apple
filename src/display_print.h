#pragma once

#include "nrf.h"

void drawChar (int x, int y, char character, uint16_t color_text, uint16_t color_bg);


void drawString (int x, int y, char* text, uint16_t color_text, uint16_t color_bg);


void drawNumber (int x, int y, int number, uint16_t color_text, uint16_t color_bg, int clearLength);


void drawSelection (int x, int y, char* text, uint16_t color_text, uint16_t color_bg, int selection, _Bool vertical);
