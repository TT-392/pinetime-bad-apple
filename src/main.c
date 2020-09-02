#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "wdt.c"
//#include "frame.h"

#include "display.h"
//#include "display_print.c"
#include "timecake/i2c_pine.h"
//#include "timecake/clock_pine.c"
#include "modules/steamLocomotive.c"
#include "modules/battery.c"
#include "modules/date.c"

#include "display.h"

int main(void) {
	//i2c_setup();
    display_backlight(255);
    display_init();
    
    //drawmono(0, 0, 239, 239, frame);
    writesquare(0, 0, 240, 240, 0x0);

    nrf_gpio_cfg_input(12, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


    sl(40, 90, 0xffff, 0x0000);
    battery_init();
    date_init();



    bool backlight = 1;
    bool lastButState = 0;
    while(1) {
        wdt_feed();
        drawDate("%Y-%m-%d  %H:%M:%S");

        battery_percent (200, 0, 0xffff, 0x0000);

//        //i2c_read(HEART_I2C_DEVICE,0,128,heart_data);
        if (nrf_gpio_pin_read(13)) {
            if (!lastButState)
                backlight = !backlight;
            display_backlight(backlight);

            lastButState = 1;
        } else {
            lastButState = 0;
        }
//
//        if (!nrf_gpio_pin_read(12)) {
//            writesquare(0, 0, 20, 20, 0x1A9F);
//        } else {
//            writesquare(0, 0, 20, 20, 0xFFFF);
//        }
//        if (!nrf_gpio_pin_read(19)) {
//            writesquare(20, 20, 40, 40, 0x1A9F);
//        } else {
//            writesquare(20, 20, 40, 40, 0xFFFF);
//        }
//
//
//        long long int time = clock_time();
//        int ms = time;
//        int second, minute, hour, day, month, year;
//
//        epochtotime(ms, &second, &minute, &hour, &day, &month, &year);
//        ms = second + minute * 1000 + hour * 1000000;
// 
//        ///ms = 1599051600;
//
//        char timeString[] = "0000000:00";
//
//        int powerOfTen = 1;
//
//        for (int i = 0; i < 10; i++) {
//            timeString[9-i] = (ms / powerOfTen) % 10 + 48;
//            powerOfTen *= 10;
//        }
//        
//
//        drawString (100, 50, 10, timeString, 0x0000, 0xffff);


    }

}
