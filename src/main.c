#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "display.h"


int main(void) {
    display_backlight(255);
    display_init();
    writesquare(0, 0, 240, 240, 0x0);
    display_backlight(255);
    

    while(1) {
        writesquare(40, 40, 200, 200, 0xffff);
        nrf_delay_ms(400);
        writesquare(40, 40, 200, 200, 0x0000);
        nrf_delay_ms(400);
	}

}
