#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_clock.h"
#include "nrf_timer.h"


#include "wdt.c"
#include "display.h"
//#include "touch.c"
#include "modules/steamLocomotive.c"
//#include "modules/keyboard.c"
#include "frame.c"

_Bool toggle = 0;

int time = 1;
void TIMER0_IRQHandler(void) {
//    display_backlight(255*toggle);
    time++;

    nrf_timer_event_clear(NRF_TIMER0, NRF_TIMER_EVENT_COMPARE0);
    nrf_timer_task_trigger(NRF_TIMER0, NRF_TIMER_TASK_CLEAR);
}



int main(void) {
    // Irq setup
    NVIC_SetPriority(TIMER0_IRQn, 15); // Lowes priority
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER0_IRQn);


    nrf_timer_mode_set(NRF_TIMER0,NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(NRF_TIMER0, NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_frequency_set(NRF_TIMER0, NRF_TIMER_FREQ_1MHz);
    nrf_timer_cc_write(NRF_TIMER0, NRF_TIMER_CC_CHANNEL0, 1000000/8);

    nrf_timer_int_enable(NRF_TIMER0,NRF_TIMER_INT_COMPARE0_MASK );
    nrf_timer_task_trigger(NRF_TIMER0, NRF_TIMER_TASK_START);






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

///    drawmono(0, 0, 239, 239, frame, 0x0000, 0xffff);


    nrf_delay_ms(2000);
    
 //       writesquare(0, 0, 240, 240, 0xffff);
    writesquare(0, 0, 240, 240, 0x0000);

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);




    bool backlight = 1;
    bool lastButState = 0;
    int swipeUp = 0;

    int lastTime = 0;
    int i = 0;
    while(osRunning) {
       // heart_loop();
        wdt_feed();

//        sl_static(40, 90, 0xffff, 0x0000);
        if (lastTime != time) {
            i++;
            if (i >= (30+22)) { 
                i = 0;
            }
            sl(65,i,0xffff,0x0000);
            lastTime = time;
        }








        nrf_delay_us(1);

        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
        }

        
        


    }

}
