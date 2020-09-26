#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_clock.h"
#include "nrf_timer.h"
#include "nrf_gpiote.h"
#include "nrf_clock.h"
#include "nrf_timer.h"


#include "wdt.c"
#include "display.h"
#include "display_defines.h"
//#include "touch.c"
//#include "steamLocomotive.h"
#include "modules/battery.c"
//#include "modules/keyboard.c"
//#include "frame.c"
#include "test.c"

_Bool toggle = 0;

int time = 1;


int main(void) {
    battery_init();
    bool osRunning = 1;

    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);

    display_backlight(255);
    display_init();
    

    nrf_gpio_cfg_output(LCD_COMMAND);


    nrf_timer_mode_set(NRF_TIMER3,NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(NRF_TIMER3, NRF_TIMER_BIT_WIDTH_16);
    nrf_timer_frequency_set(NRF_TIMER3, NRF_TIMER_FREQ_16MHz);
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL0, 5+(0*2));
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL1, 5+(8*2));
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL2, 5+(40*2));
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL3, 5+(48*2));
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL4, 5+(80*2));
    nrf_timer_cc_write(NRF_TIMER3, NRF_TIMER_CC_CHANNEL5, 5+(88*2));

    nrf_timer_int_enable(NRF_TIMER3,NRF_TIMER_INT_COMPARE0_MASK);
    nrf_timer_task_trigger(NRF_TIMER3, NRF_TIMER_TASK_START);



   // NRF_GPIOTE->CONFIG[2] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
   //                         (LCD_COMMAND << GPIOTE_CONFIG_PSEL_Pos) |
   //                         (GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos);

    NRF_GPIOTE->CONFIG[1] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
                            GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
                            LCD_COMMAND << GPIOTE_CONFIG_PSEL_Pos | 
                            GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos;


    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[0].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[0];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[0].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH0_Enabled << PPI_CHENSET_CH0_Pos;

    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[1].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[1];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[1].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH1_Enabled << PPI_CHENSET_CH1_Pos;

    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[2].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[2];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[2].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH2_Enabled << PPI_CHENSET_CH2_Pos;

    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[3].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[3];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[3].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH3_Enabled << PPI_CHENSET_CH3_Pos;

    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[4].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[4];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[4].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH4_Enabled << PPI_CHENSET_CH4_Pos;

    //NRF_PPI->CH[0].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[5].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[5];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[5].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH5_Enabled << PPI_CHENSET_CH5_Pos;

  //  NRF_PPI->CH[1].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[0];// &NRF_GPIOTE->EVENTS_IN[2];
  //  NRF_PPI->CH[1].TEP = (uint32_t) &NRF_TIMER3->TASKS_CLEAR;

  //  NRF_PPI->CHENSET = PPI_CHENSET_CH1_Enabled << PPI_CHENSET_CH1_Pos;

    
    NRF_PPI->CH[6].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;// &NRF_GPIOTE->EVENTS_IN[2];
    //NRF_PPI->CH[3].EEP = (uint32_t) &NRF_GPIOTE->EVENTS_IN[2]; 
    //NRF_PPI->CH[2].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[0];// &NRF_GPIOTE->EVENTS_IN[2];
    NRF_PPI->CH[6].TEP = (uint32_t) &NRF_TIMER3->TASKS_CLEAR;
    //NRF_PPI->CH[3].TEP = (uint32_t) &NRF_GPIOTE->TASKS_OUT[1];

    NRF_PPI->CHENSET = PPI_CHENSET_CH6_Enabled << PPI_CHENSET_CH6_Pos;




    uint8_t frame[originalFrameLength] = {};
    _Bool currentColor = 0;
    uint16_t binAddress = 0;
    for (int i = 0; i < compressedLength; i++) {
        for (int j = 0; j < compressedFrame[i]; j++) {
            frame[binAddress / 8] |= currentColor << (binAddress % 8);
            binAddress++;
        }
        currentColor = !currentColor;
    }


   //drawmono(0, 0, 239, 239, frame, 0x0000, 0xffff);


  // writesquare(0, 0, 240, 240, 0xffff);
  // writesquare(1, 1, 50, 50, 0x0000);

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);

    bool backlight = 1;
    bool lastButState = 0;
    int swipeUp = 0;

    int lastTime = 0;
    int i = 0;

    bool running = 1;
//    while (running) {
//        wdt_feed();
//        if (sl_nextFrameReady) {
//            running = !sl(65, 0xffff, 0x0000);
//        }
//    }

    while(osRunning) {
//        battery_percent(200,0,0xffff,0x0000);
        wdt_feed();

 //       if (sl_nextFrameReady) {
 //          sl(65, 0xffff, 0x0000);
 //       }

        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
        }
        display_backlight(255);
        nrf_delay_ms(100);
        tempFunction();
//        tempFunction();
    }
}
