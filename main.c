#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"


#include "display.c"


int main(void)
{
    display_backlight(255);
    display_init();

    writesquare(0, 0, 50, 240, 0x6000);
    writesquare(0, 0, 240, 50, 0x6000);
    writesquare(50, 50, 100, 150, 0x6666);
    writesquare(50, 50, 100, 100, 0xffff);
    

	while(1) {
        writesquare(0, 0, 240, 240, 0xffff);
        writesquare(0, 0, 240, 240, 0x0000);
	}

}
