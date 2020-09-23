#pragma once

extern volatile bool sl_nextFrameReady;


void sl_static(int x, int y, uint16_t color_text, uint16_t color_bg);

int sl(int y, uint16_t color_text, uint16_t color_bg);
