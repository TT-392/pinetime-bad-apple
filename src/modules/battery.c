#include "../display_print.h"
#include "../external/timecake/battery_pine.c"

float filteredPercentage = 0;

void battery_init () {
    battery_setup();

    int flags;
    float voltage;

    battery_read(&flags, &voltage, &filteredPercentage);
}

void battery_percent (int x, int y, uint16_t color_text, uint16_t color_bg) {
    int flags;
    float voltage, percent;

    battery_read(&flags, &voltage, &percent);

    filteredPercentage += 0.001 * (percent - filteredPercentage);

    char batteryString[] = "000%";

    batteryString[2] = ((int)filteredPercentage % 10) + 48;
    batteryString[1] = (((int)filteredPercentage/10) % 10) + 48;
    batteryString[0] = (((int)filteredPercentage/100) % 10) + 48;
    if (filteredPercentage < 100) {
        batteryString[0] = 32;
        if (filteredPercentage < 10) {
            batteryString[1] = 32;
        }
    }

    drawString (x, y, batteryString, color_text, color_bg);
}
