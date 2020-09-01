#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "wdt.c"
//#include "frame.h"

#include "display.h"
#include "timecake/i2c_pine.h"


int main(void) {
	//i2c_setup();
    display_backlight(255);
    display_init();
    
    //drawmono(0, 0, 239, 239, frame);
    writesquare(0, 0, 240, 240, 0x0);

    nrf_gpio_cfg_input(12, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


    drawChar (50, 50, 65);
    writesquare(50+8, 50, 50+8+7, 50+15, 0x1A9F);

    writesquare(90, 90, 90, 90, 0x1A9F);

    while(1) {
        //wdt_feed();
        //i2c_read(HEART_I2C_DEVICE,0,128,heart_data);
        if (nrf_gpio_pin_read(13)) {
            display_backlight(0);
        }
        if (!nrf_gpio_pin_read(12)) {
            writesquare(0, 0, 20, 20, 0x1A9F);
        } else {
            writesquare(0, 0, 20, 20, 0xFFFF);
        }
        if (!nrf_gpio_pin_read(19)) {
            writesquare(20, 20, 40, 40, 0x1A9F);
        } else {
            writesquare(20, 20, 40, 40, 0xFFFF);
        }
       // nrf_delay_ms(5000);
       // NVIC_SystemReset();
	}

}
