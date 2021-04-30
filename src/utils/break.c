#include "nrf.h"

// resets wdt if button not pressed
// only use this function to feed the watchdog or risk bricking your device
void breakPoint() {
    __NOP();
}
