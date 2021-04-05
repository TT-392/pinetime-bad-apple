#pragma once
#include <stdint.h>
#include "display.h"
#include "wdt.h"
#include "watchface.h"
#include "statusbar.h"
#include "steamLocomotive.h"
#include "sleep.h"

struct menuBMP { // (x2 - x1 + 1) % 8 should always be 0
    int x1;
    int y1;
    int x2;
    int y2;
    uint16_t color;
    char* BMP;
};

struct menu_item {
    char* name;
    int elements;
    struct menuBMP element[5];
};

struct menu_properties {
    uint16_t top;
    uint16_t bottom;
    int length;
    struct menu_item* items;
    int item_size;
    int icon_top;
    int icon_height;
    int icon_width; // in bytes
};

int scrollMenu_init ();

int drawScrollMenu ();
