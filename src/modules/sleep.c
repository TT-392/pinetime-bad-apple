#include "display.h"
#include "touch.h"
#include "nrf_delay.h"
#include <stdbool.h>

static bool wake = 0;

void wakeInterrupt() {
    wake = 1;
}

void sleep() {
    wake = 0;
    display_backlight(0);
    subscribeTouchInterrupt(wakeInterrupt);


    while (!wake) {
        __WFI();
    }

}
