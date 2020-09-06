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

#include "display.h"
#include "nrf_drv_gpiote.h"

#define TOUCH_I2C_DEVICE (0x15)

_Bool toggle = 1;

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
}

int main(void) {
    display_backlight(255);
    display_init();
    
    //drawmono(0, 0, 239, 239, frame);
    writesquare(0, 0, 240, 240, 0x0);

//    nrf_gpio_cfg_input(28, NRF_GPIO_PIN_NOPULL);
//    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_output(10);	



    sl(40, 90, 0xffff, 0x0000);
    battery_init();
    date_init();


    nrf_drv_gpiote_init();

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = NRF_GPIO_PIN_NOPULL;

    nrf_drv_gpiote_in_init(28, &in_config, in_pin_handler);

    nrf_drv_gpiote_in_event_enable(28, true);


	i2c_setup();



    bool backlight = 1;
    bool lastButState = 0;
    bool osRunning = 1;
    while(osRunning) {
        wdt_feed();
        drawDate("%Y-%m-%d  %H:%M:%S");

        battery_percent (200, 0, 0xffff, 0x0000);

        uint8_t touch_data[128];

        i2c_read(TOUCH_I2C_DEVICE,0,128,touch_data);

        if (touch_data[0] > 0)
            drawNumber(200,180,touch_data[0],0xffff,0x0000,3);
        if (touch_data[1] > 0)
            drawNumber(200,200,touch_data[1],0xffff,0x0000,3);
        if (touch_data[2] > 0)
            drawNumber(200,220,touch_data[2],0xffff,0x0000,3);

        if (touch_data[1] == 4)
            toggle = !toggle; 

        if (nrf_gpio_pin_read(13)) {
            //osRunning = 0;
            nrf_gpio_pin_write(10,0);

            //if (!lastButState)
            //    backlight = !backlight;
            //display_backlight(backlight);

            lastButState = 1;
        } else {
            nrf_gpio_pin_write(10,1);
            lastButState = 0;
        }

        if (toggle) {
            writesquare(0, 0, 20, 20, 0x1A9F);
        } else {
            writesquare(0, 0, 20, 20, 0xFFFF);
        }

    }

}
