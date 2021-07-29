#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "touch.h"
#include "wdt.h"
#include "display.h"
#include "display_defines.h"
#include "battery.h"
#include "semihost.h"
#include "statusbar.h"
#include "scrollMenu.h"
//#include "systick.h"
#include "date.h"
#include "uart.h"
#include "sleep.h"
#include "core.h"
#include "main_menu.h"
#include "bad_apple.h"
#include "bad_apple_flash.h"
#include "flash.h"
#include "adler32.h"
#include "display_print.h"

static bool toggle = 1;

int main(void) {
//    battery_init();
    display_init();
//    date_init();
    //sysTick_init();
//    date_init();
    bool osRunning = 1;

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    drawSquare(0,0,239,239,0x0000);
    //drawSquare(0,0,50,50,0xffff);

    spiflash_init();

    display_backlight(255);
    write_video();

    render_video();
    while (1);


    //uart_init();

    int x = 0;


    int i = 320;
    int counter = 0;


    nrf_gpio_cfg_output(15);	
    nrf_gpio_pin_write(15,1);
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);


    struct touchPoints touchPoint = {0};
    bool backlight = 0;

    //statusBar_refresh();
    touch_init();

    core_start_process(&main_menu);
    core_start_process(&statusbar);

    while(osRunning) {
        //partialMode(0,90);
        core_run();
        wdt_feed();
    }
}
