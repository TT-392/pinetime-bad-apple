#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.c"
#include "display.h"
#include "display_defines.h"
#include "modules/battery.c"
#include "test.c"
#include "frame.c"
#include "modules/steamLocomotive.h"
#include "modules/breakout.c"

#include "touch.c"

_Bool toggle = 0;

int time = 1;


static int seed = 1;

int randnumber (void) {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed/65536) % 32768;
}


int main(void) {
    battery_init();
    bool osRunning = 1;

    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);


    /// uint8_t bitmap [50*50*2] = {};
    /// for (int x = 0; x < 50*50*2; x++) {
    ///         bitmap[x] = (x%2)*0xff;
    /// }

    //drawBitmap(50,50,99,99,bitmap);
    // while (1) {
    //     //drawSquare(0, 0, 239, 239, 0xffff);
    //     //drawSquare(0, 0, 239, 239, 0xf800);
    //     //drawSquare(0, 0, 239, 239, 0xf800);
    //     drawMono(0, 0, 239, 239, frame, 0x0000, 0xffff);
    //     //display_init();
    //     wdt_feed();
    // }




    //    nrf_gpio_cfg_output(16);

    //  for (int i = 0; i < 240; i++) {
    //      drawSquare(i, 10+10*sin((float)i/10), i, 10+10*sin((float)i/10), 0xffff);
    //  }

    //    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);
    //    nrf_gpio_pin_write(16,0);

    //   sl(65, 0xffff, 0x0000);
    while (!nrf_gpio_pin_read(13)) {
        while (!sl_nextFrameReady) {
            __WFI();
        }

        sl(65, 0xffff, 0x0000);
        wdt_feed();
        battery_percent(200,0,0xffff,0x0000);
    }
    //    nrf_gpio_pin_write(16,1);

    while(osRunning) {
        wdt_feed();

        display_backlight(0);

        drawSquare(0, 0, 239, 239, 0x0000);
        nrf_delay_ms(1000);

        //   if (nrf_gpio_pin_read(13)) {
        //       osRunning = 0;
        //   }
        //   display_backlight(255);
        //   nrf_delay_ms(100);


    }
}
