#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_clock.h"

#include "wdt.c"
#include "display.h"
#include "display_defines.h"
#include "modules/battery.c"
#include "test.c"

_Bool toggle = 0;

int time = 1;

uint64_t randomvalue = 0;

//static unsigned int seed = 1;

int randnumber (void) {
    //seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    randomvalue += (randomvalue* 1103515245U + 12345U) & 0x7fffffffU;
    return (int)randomvalue;
}



int main(void) {
    battery_init();
    bool osRunning = 1;

    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);

    display_backlight(255);
    display_init();
    

    nrf_gpio_cfg_output(LCD_COMMAND);




    uint8_t frame[originalFrameLength] = {};
    _Bool currentColor = 0;
    uint16_t binAddress = 0;
    for (int i = 0; i < compressedLength; i++) {
        for (int j = 0; j < compressedFrame[i]; j++) {
            frame[binAddress / 8] |= currentColor << (binAddress % 8);
            binAddress++;
        }
        currentColor = !currentColor;
    }


   //drawmono(0, 0, 239, 239, frame, 0x0000, 0xffff);


   writesquare(0, 0, 200, 50, 0xf8f0);
   writesquare(50, 50, 100, 200, 0xffff);
   writesquare(150, 150, 154, 158, 0x5FE8);
  // writesquare(0, 0, 240, 240, 0xffff);
//    writesquare(50, 50, 50, 50, 0xffff);

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    bool backlight = 1;
    bool lastButState = 0;
    int swipeUp = 0;

    int lastTime = 0;
    int i = 0;

    bool running = 1;
//    while (running) {
//        wdt_feed();
//        if (sl_nextFrameReady) {
//            running = !sl(65, 0xffff, 0x0000);
//        }
//    }

    while(osRunning) {
//        battery_percent(200,0,0xffff,0x0000);
        wdt_feed();

 //       if (sl_nextFrameReady) {
 //          sl(65, 0xffff, 0x0000);
 //       }

        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
        }
        display_backlight(255);
        nrf_delay_ms(100);
       // uint16_t x = randnumber() % 20; uint16_t y = randnumber() % 20;
       // writesquare(x % 20, y % 20, (uint16_t)(x+(randnumber() % 20)) % 20, (uint16_t)(y+(randnumber() % 20))% 20, 0xf800);
//        tempFunction();
    }
}
