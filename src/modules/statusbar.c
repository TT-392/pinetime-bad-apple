#include "date.h"
#include "battery.h"
#include "nrf_gpio.h"
#include "clock_pine.h"
#include "core.h"
#include "statusbar.h"

struct process statusbar = {
    .runExists = 1,
    .run = &statusbar_run,
    .startExists = 0,
    .stopExists = 0,
    .event = &event_always,
};

void statusbar_run() {
    if (secondPassed) {
        uint16_t color = 0xffff; 
        if (!nrf_gpio_pin_read(19))
            color = 0x67EC;
        battery_percent(200,0,color,0x0000);
        drawDate(0,0,"%H:%M:%S");
        secondPassed = 0;
    } 
}
