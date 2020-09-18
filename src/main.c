#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "wdt.c"

#include "display.h"
#include "touch.c"
#include "modules/steamLocomotive.c"
#include "modules/keyboard.c"


_Bool toggle = 0;
_Bool touchEvent = 0;
//void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
//    touchEvent = 1;
//    toggle = !toggle;
//}

int main(void) {
    bool osRunning = 1;

    //osRunning = !nrf_sdh_enable_request();
//    sd_softdevice_enable();
    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);


    display_backlight(255);
    display_init();

//    drawmono(0, 0, 239, 239, frame);

    writesquare(0, 0, 240, 240, 0x0);
    //heart_init();
   // battery_init();
   // battery_percent (200, 0, 0xffff, 0x0000);
//    for (int i = 0; i < 240; i++) {
//        writesquare(i, i, i, i, 0xffff);
//        writesquare(240-i, i, 240-i, i, 0xffff);
//    }

//    keyboard_init();

    touch_init();

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


    //sl(40, 90, 0xffff, 0x0000);
   // battery_init();
    //date_init();

//    nrf_drv_gpiote_init();
//
//    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
//    in_config.pull = NRF_GPIO_PIN_NOPULL;
//
//    nrf_drv_gpiote_in_init(28, &in_config, in_pin_handler);
//
//    nrf_drv_gpiote_in_event_enable(28, true);


    bool backlight = 1;
    bool lastButState = 0;
    int swipeUp = 0;
    while(osRunning) {
       // heart_loop();
        wdt_feed();


//        keyboard_loop();
        //drawDate("%Y-%m-%d  %H:%M:%S");

    //    battery_percent (200, 0, 0xffff, 0x0000);

        uint8_t touch_data[8] = {};
        struct touchPoints touchPoint = {};
        int returnvalue = touch_refresh(&touchPoint, touch_data);

        
        drawNumber(8*3,0*16,touchPoint.touchX,0xffff,0x0000,3);
        drawNumber(8*3,1*16,touchPoint.touchY,0xffff,0x0000,3);
        drawNumber(8*3,2*16,touchPoint.event,0xffff,0x0000,3);
        drawNumber(8*3,3*16,touchPoint.gesture,0xffff,0x0000,3);
        drawNumber(8*3,4*16,touchPoint.pressure,0xffff,0x0000,3);
        drawNumber(8*3,5*16,returnvalue,0xffff,0x0000,3);

        for (int i = 0; i < 8; i++) {
            if (touch_data[i] != 0)
                drawNumber(8*9,i*16,touch_data[i],0xffff,0x0000,3);
        }


        if (touch_data[1] == 2) {
            writesquare(0, 0, 240, 240, 0xffff);
        } else {
            swipeUp = 0;
        }


        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
        }

       // if (toggle) {
       //     writesquare(0, 0, 20, 20, 0x1A9F);
       // } else {
       //     writesquare(0, 0, 20, 20, 0xFFFF);
       // }
        if (!nrf_gpio_pin_read(19)) {
            writesquare(220, 220, 240, 240, 0x1A9F);
    //        display_backlight(0);
        } else {
            writesquare(220, 220, 240, 240, 0xFFFF);
            display_backlight(255);
        }
        
        


    }

}
