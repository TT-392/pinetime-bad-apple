#pragma once

extern volatile bool sl_nextFrameReady;


void sl_static(int x, int y, uint16_t color_text, uint16_t color_bg);

/*
 * Example usage:
 *
 * bool running = 1;
 * while (running) {
 *     wdt_feed();
 *     if (sl_nextFrameReady) {
 *         running = !sl(65, 0xffff, 0x0000);
 *     }
 * }
 */
int sl(int y, uint16_t color_text, uint16_t color_bg);
