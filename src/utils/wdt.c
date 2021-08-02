#include "nrf.h"
#include "nrf_gpio.h"

// resets wdt if button not pressed
// only use this function to feed the watchdog or risk (soft) bricking your device
void wdt_feed (void) {
    if (!nrf_gpio_pin_read(13)) {
        NRF_WDT->RR[0] = 0x6E524635;
    }
}
