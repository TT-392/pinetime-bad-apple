#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.h"
#include "display.h"
#include "display_defines.h"
#include "battery.h"
#include "frame.c"
#include "steamLocomotive.h"
#include "breakout.h"
//#include "modules/date.c"
//#include "modules/heart.c"
#include "semihost.h"
//#include "modules/statusbar.c"
#include "scrollMenu.h"
#include "systick.h"
#include "watchface.h"
#include "touch.h"

static bool toggle = 1;



int main(void) {
    battery_init();
    display_init();
    sysTick_init();
    //date_init();
    bool osRunning = 1;
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    drawSquare(0, 0, 239, 319, 0x0000);
    display_backlight(255);

    digitalWatch();



    int i = 320;
    int counter = 0;

//    int logColor;
//    for (int i = 0; i < 300; i++) {
//        drawMenuLine (i, -1, i+20);
//    }


    nrf_gpio_cfg_output(15);	
    nrf_gpio_pin_write(15,1);
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);

    //touch_refresh(&touchPoint1);




    //statusBar_refresh();

    while(osRunning) {

        //statusBar_refresh();
        int error;
        // do {
        //error = touch_refresh(&touchPoint1);
        wdt_feed();



        if (!nrf_gpio_pin_read(13)) {
            display_backlight(255);
        }
        else {
            display_backlight(0);
        }



        //statusBar_refresh();

        int selectedItem = drawScrollMenu();
        if (selectedItem != -1) {
            if (selectedItem == 2) {
                drawSquare(0, 20, 239, 319, 0x0000);
                scroll(320, 0, 0, 0);

                bool running = 1;
                while (running) {
                    wdt_feed();
                    if (sl_nextFrameReady) {
                        running = !sl(65, 0xffff, 0x0000);
                        //statusBar_refresh();
                    }
                }

                while (1) {
                    //statusBar_refresh();
                }
            }
        }
    }
}
