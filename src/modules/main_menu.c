#include "scrollMenu.h"
#include "core.h"
#include "main_menu.h"

struct process main_menu = {
    .runExists = 1,
    .run = &menu_run,
    .startExists = 1,
    .start = &menu_init,
    .stopExists = 1,
    .stop = &menu_stop,
    .event = &event_always,
};

void menu_init() {    
    drawSquare(0, 0, 239, 319, 0x0000);
    scrollMenu_init();
    core_start_process(&statusbar);
}

void menu_run() {
    int selectedItem = drawScrollMenu();

    if (selectedItem != -1) {
        display_scroll(320, 0, 0, 0);

        if (selectedItem == 2) {
            core_stop_process(&main_menu);
            core_start_process(&sl);
            return;
        }
        if (selectedItem == 0) {
            core_stop_process(&main_menu);
            core_start_process(&watchface);
            return;
        }
        if (selectedItem == 1) {
            sleep();
            drawSquare(0, 0, 239, 319, 0x0000);
            scrollMenu_init();
            display_backlight(255);
        }
    }
}

void menu_stop() {
    core_stop_process(&statusbar);
}
