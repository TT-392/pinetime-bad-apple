#pragma once

// resets wdt if button not pressed
// only use this function to feed the watchdog or risk bricking your device
void wdt_feed (void);
