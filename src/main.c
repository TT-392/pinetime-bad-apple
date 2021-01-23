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
#include "statusbar.h"
#include "scrollMenu.h"
#include "systick.h"
#include "watchface.h"
#include "date.h"
#include "touch.h"
#include "keyboard.c"

static bool toggle = 1;



int main(void) {
    battery_init();
    display_init();
    date_init();
    sysTick_init();
    //date_init();
    bool osRunning = 1;
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    drawSquare(0, 0, 239, 319, 0x0000);
    display_backlight(255);

    //    digitalWatch();



    int i = 320;
    int counter = 0;

    //    int logColor;
    //    for (int i = 0; i < 300; i++) {
    //        drawMenuLine (i, -1, i+20);
    //    }

    //    drawSquare(0, 0, 239, 319, 0xffff);

    nrf_gpio_cfg_output(15);	
    nrf_gpio_pin_write(15,1);
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);


    struct touchPoints touchPoint = {0};
    bool backlight = 0;
    while (1) {
        touch_refresh(&touchPoint);
        //if (touchPoint.error == 0) {
            if (touchPoint.tab == 1) {
                drawSquare(touchPoint.touchX-2,touchPoint.touchY-2,touchPoint.touchX+2,touchPoint.touchY+2,0x07FC);
            } else if (touchPoint.tab == 2) {
                drawSquare(touchPoint.touchX-2,touchPoint.touchY-2,touchPoint.touchX+2,touchPoint.touchY+2,0xff00);
            } else {
                drawSquare(touchPoint.touchX,touchPoint.touchY,touchPoint.touchX,touchPoint.touchY,0xffff);
            }
            drawNumber(140,50,touchPoint.errorCount,0xffff,0x0000,10,0);
        //}
    }

    if (touchPoint.error == 0) {
    }




    //statusBar_refresh();

    scrollMenu_init();
    while(osRunning) {

        statusBar_refresh();
        int error;
        // do {
        //error = touch_refresh(&touchPoint1);
        wdt_feed();



        //if (!nrf_gpio_pin_read(13)) {
        //    display_backlight(255);
        //}
        //else {
        //    display_backlight(0);
        //}




        int selectedItem = drawScrollMenu();
        if (selectedItem != -1) {
            display_scroll(320, 0, 0, 0);
            if (selectedItem == 2) {
                drawSquare(0, 20, 239, 319, 0x0000);

                bool running = 1;
                while (running) {
                    wdt_feed();
                    if (sl_nextFrameReady) {
                        running = !sl(65, 0xffff, 0x0000);
                        statusBar_refresh();
                    }
                }
                scrollMenu_init();

            }
            if (selectedItem == 0) {
                digitalWatch();
                drawSquare(0, 0, 239, 319, 0x0000);
                scrollMenu_init();
            }
        }
    }
    }
