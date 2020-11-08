#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.h"
#include "display.h"
#include "display_defines.h"
#include "modules/battery.c"
#include "test.c"
#include "frame.c"
#include "modules/steamLocomotive.h"
#include "modules/breakout.c"
#include "modules/date.c"
#include "modules/heart.c"
#include "touch.c"
#include "icons.c"
#include "semihost.h"


static int seed = 3322492478;
int randnumber (void) {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed/65536) % 32768;
}

void statusBar_refresh() {
    uint16_t color = 0xffff; 
    if (!nrf_gpio_pin_read(19))
        color = 0x67EC;
    battery_percent(200,0,color,0x0000);
    drawDate(0,0,"%H:%M:%S");
}


int scrollPosition(int lowerBound, int upperBound) {
    while (1) {
        if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
            semihost_print("test\n", 5);
        }

    }
    static int touchAtStart = 0;
    static int lastEvent = 0;
    static int scrollAtTouchUp = 0;


    struct touchPoints touchPoint;
    int error;
   // do {
        error = touch_refresh(&touchPoint);
   // } while (touchPoint.touchY == 0 && error == 0);
    
    
    int touchY = 240 - touchPoint.touchY;

    if (lastEvent == 0 && touchPoint.event != 0) { // touch down
        touchAtStart = touchY;
    }

    if (lastEvent != 0 && touchPoint.event == 0) { // touch up
        scrollAtTouchUp += touchY - touchAtStart;
    }

    if (touchPoint.event != 0) {
        if ((scrollAtTouchUp + touchY - touchAtStart) <= lowerBound) { // if lower bound is reached
            scrollAtTouchUp = lowerBound;
            touchAtStart = touchY; // switch these around to change performance
                                   // to leaving off when reached bound

        } else if ((scrollAtTouchUp + touchY - touchAtStart) >= upperBound) {
            scrollAtTouchUp = upperBound;
            touchAtStart = touchY;                                    
        }
    }


    lastEvent = touchPoint.event;
    

    if (touchPoint.event != 0) 
        return scrollAtTouchUp + touchY - touchAtStart;
    else
        return scrollAtTouchUp;

}


int main(void) {
    battery_init();
    display_init();
    date_init();
    bool osRunning = 1;
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    drawSquare(0, 0, 239, 319, 0x0000);
    display_backlight(255);

    int i = 320;
    int counter = 0;

    for (int i = 0; i < 5; i++) {
        int logoColor = randnumber() % 0xfff;
        drawMono(0, i*55+20, 55, i*55+68, termux, logoColor, 0x0000);
        drawNumber(100, i*55+20, i+1, 0xffff, 0x0000, 1, 0);
    }

    touch_init();

//    heart_init();


    int scrollPos = 0;
    while(osRunning) {
        counter++;
        wdt_feed();


        statusBar_refresh();

        int currentScroll = scrollPosition(0, 300);

        
        if (scrollPos < currentScroll) {
            scrollPos ++;
        } if (scrollPos > currentScroll) {
            scrollPos --;
        }
        scrollPos = currentScroll;

//        scroll(20, 300, 0, 20 + (uint16_t)scrollPos % 300);
    }
}
