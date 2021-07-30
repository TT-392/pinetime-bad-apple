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

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    drawSquare(0,0,239,239,0xffff);

    display_backlight(255);
    write_video();


    render_video();
    while (1);
}
