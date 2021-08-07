#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "wdt.h"
#include "display.h"
#include "display_defines.h"
#include "bad_apple.h"
#include "bad_apple_flash.h"
#include "flash.h"
#include "display_print.h"

static bool toggle = 1;

int main(void) {
    display_init();
    spiflash_init();

    // clear screen
    drawSquare(0, 0, 239, 239, 0x0000);
    display_backlight(255);

    // setup button
    nrf_gpio_cfg_output(15);
    nrf_gpio_pin_write(15, 1);
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);

    write_video();

    drawString(8, 110, "<- Play", 0x0000, 0xffff);

    while (!nrf_gpio_pin_read(13))
        wdt_feed();

    drawString(8, 110, "        ", 0x0000, 0xffff);


    render_video();
    while (1);
}
