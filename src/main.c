#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "wdt.c"
//#include "frame.h"

#include "display.h"
//#include "display_print.c"
#include "timecake/i2c_pine.h"
//#include "timecake/heart_pine.c"
//#include "timecake/clock_pine.c"
#include "modules/steamLocomotive.c"
#include "modules/battery.c"
#include "modules/date.c"
#include "display_print.h"

#include "nrf_drv_gpiote.h"

#define HEART_I2C_DEVICE (0x15)

_Bool toggle = 0;
_Bool touchEvent = 0;
void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    touchEvent = 1;
    toggle = !toggle;
}

int main(void) {
    nrf_gpio_cfg_output(10);	
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);

	i2c_setup();
    display_backlight(255);
    display_init();
    
    //drawmono(0, 0, 239, 239, frame);
    writesquare(0, 0, 240, 240, 0x0);

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


    sl(40, 90, 0xffff, 0x0000);
    battery_init();
    date_init();

    nrf_drv_gpiote_init();

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = NRF_GPIO_PIN_NOPULL;

    nrf_drv_gpiote_in_init(28, &in_config, in_pin_handler);

    nrf_drv_gpiote_in_event_enable(28, true);

    nrf_gpio_cfg_output(10);	
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);


//    uint8_t acc_data[2];
//    acc_data[0]=0x00;
//    acc_data[1]=0x00;
//    i2c_write(HEART_I2C_DEVICE,2,acc_data);

    bool backlight = 1;
    bool lastButState = 0;
    bool osRunning = 1;
    while(osRunning) {
        wdt_feed();
        drawDate("%Y-%m-%d  %H:%M:%S");

        battery_percent (200, 0, 0xffff, 0x0000);


       // if (touchEvent) {
            uint8_t heart_data[8];

            i2c_read(HEART_I2C_DEVICE,0,8,heart_data);
            if (heart_data[3] != 0) {
                drawNumber(200,200,heart_data[3],0xffff,0x0000,3);
            }
            touchEvent = 0;
        //}


        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
            if (!lastButState)
                backlight = !backlight;
            display_backlight(backlight);

            lastButState = 1;
        } else {
            lastButState = 0;
        }

        if (toggle) {
            writesquare(0, 0, 20, 20, 0x1A9F);
        } else {
            writesquare(0, 0, 20, 20, 0xFFFF);
        }
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
