#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

int main(void)
{			
    nrf_gpio_cfg_output(23);
    while (1){	
        nrf_gpio_pin_clear(23);
        nrf_delay_ms(1000);
        nrf_gpio_pin_set(23);
        nrf_delay_ms(1000);
    }
}


