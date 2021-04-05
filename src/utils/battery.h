#pragma once

#include "nrf.h"

void battery_init ();

int battery_percent();

void battery_draw_percent (int x, int y, uint16_t color_text, uint16_t color_bg);
