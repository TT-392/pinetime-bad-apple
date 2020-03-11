#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"


#include "display.c"


int main(void)
{
    display_backlight(255);
    display_init();

    writesquare(50, 50, 100, 200, 0x6666);


    

	while(1) {
	}

}
