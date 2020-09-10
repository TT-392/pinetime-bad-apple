#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
//#include "nrf_sdh.h"
#include "wdt.c"

#include "wdt.h"
//#include "frame.h"

#include "display.h"
//#include "display_print.c"
//#include "timecake/i2c_pine.h"
//#include "timecake/heart_pine.c"
//#include "timecake/clock_pine.c"
#include "modules/steamLocomotive.c"
//#include "modules/battery.c"
//#include "modules/date.c"
//#include "modules/keyboard.c"
#include "modules/heart.c"
//#include "display_print.h"

//#include "nrf_drv_gpiote.h"


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
    heart_init();
   // battery_init();
   // battery_percent (200, 0, 0xffff, 0x0000);
//    for (int i = 0; i < 240; i++) {
//        writesquare(i, i, i, i, 0xffff);
//        writesquare(240-i, i, 240-i, i, 0xffff);
//    }

//    keyboard_init();

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


//    sl(40, 90, 0xffff, 0x0000);
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
    while(osRunning) {
        wdt_feed();


//        keyboard_loop();
        //drawDate("%Y-%m-%d  %H:%M:%S");

    //    battery_percent (200, 0, 0xffff, 0x0000);






        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
            if (!lastButState)
                backlight = !backlight;
            //display_backlight(backlight);

            lastButState = 1;
        } else {
            lastButState = 0;
        }

       // if (toggle) {
       //     writesquare(0, 0, 20, 20, 0x1A9F);
       // } else {
       //     writesquare(0, 0, 20, 20, 0xFFFF);
       // }
        //        if (!nrf_gpio_pin_read(19)) {
        //            writesquare(20, 20, 40, 40, 0x1A9F);
        //        } else {
        //            writesquare(20, 20, 40, 40, 0xFFFF);
        //        }
        //
        //


    }

}
