#pragma once
#include "stdint.h"

/*
 * Function to init the display, 
 * this will initialize and use NRF_TIMER3 and the first 8 PPI channels
 */
void display_init();

/*
 * Function to draw a square
 */
void drawSquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/*
 * placeholder for actual brightness control, 0 = off and > 1 is 100%
 */
void display_backlight(char brightness);

/*
 * Function to draw a 1 bit bitdepth bitmap
 */
void drawMono(int x1, int y1, int x2, int y2, uint8_t *frame, uint16_t posColor, uint16_t negColor);

/*
 * Function to draw a bitmap to the display
 * bitmap is an array of 8 bit integers, 2 of these form a single pixel. RGB 565 
(most significant byte first)
 */
void drawBitmap (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t* bitmap);



void display_scroll(uint16_t TFA, uint16_t VSA, uint16_t BFA, uint16_t scroll_value);
/*
 * the following function currently doesn't work
 */
void partialMode(uint16_t PSL, uint16_t PEL);

