#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.h"
//#include "display.h"
//#include "display_defines.h"
#include "battery.h"
#include "frame.c"
#include "steamLocomotive.h"
#include "breakout.h"
//#include "modules/date.c"
//#include "modules/heart.c"
#include "semihost.h"
#include "statusbar.h"
#include "scrollMenu.h"
#include "systick.h"
#include "watchface.h"
#include "date.h"
#include "touch.h"
#include "uart.h"
#include "sleep.h"
#include "core.h"
#include "main_menu.h"
#include "bad_apple.h"
#include "spi.h"
#include "display_print.h"
#include "display_defines.h"

static bool toggle = 1;

#define FLASH_CS 5

// send one byte over spi

int main(void) {
    nrf_gpio_cfg_output(LCD_SELECT);
    nrf_gpio_pin_write(LCD_SELECT,1);
    nrf_gpio_cfg_output(FLASH_CS);
    nrf_gpio_pin_write(FLASH_CS,1);
    display_init();

    nrf_delay_ms(1000);
    

    display_backlight(255);

    drawSquare(0,0,239,239,0x0000);
    drawSquare(50, 50, 100, 100, 0xffff);
    

    display_pause();


    
        nrf_gpio_pin_write(FLASH_CS,0);
        volatile uint8_t data[3] = {0, 1, 2};
        uint8_t byte = 0x9f;
        NRF_SPIM0->TXD.MAXCNT = 1;
        NRF_SPIM0->TXD.PTR = (uint32_t)&byte;

        NRF_SPIM0->EVENTS_ENDTX = 0;
        NRF_SPIM0->EVENTS_ENDRX = 0;
        NRF_SPIM0->EVENTS_END = 0;
        NRF_SPIM0->EVENTS_STOPPED = 0;

        NRF_SPIM0->TASKS_START = 1;
        while(NRF_SPIM0->EVENTS_END == 0) __NOP();

        NRF_SPIM0->RXD.MAXCNT = 3;
        NRF_SPIM0->RXD.PTR = (uint32_t)&data;

        NRF_SPIM0->EVENTS_END = 0;
        NRF_SPIM0->TASKS_START = 1;
        while(NRF_SPIM0->EVENTS_END == 0) __NOP();
        NRF_SPIM0->RXD.MAXCNT = 0;

        NRF_SPIM0->EVENTS_ENDTX = 0;
        NRF_SPIM0->EVENTS_ENDRX = 0;
        NRF_SPIM0->EVENTS_END = 0;
        NRF_SPIM0->EVENTS_STOPPED = 0;
        nrf_gpio_pin_write(FLASH_CS,1);
        





    display_resume();

    nrf_delay_ms(1000);

    display_backlight(255);
    drawSquare(0,0,239,239,0x0000);
    drawSquare(0,0,50,50,0xffff);
    drawNumber(50, 50, data[0], 0xffff, 0x0000, 0, 0);
    drawNumber(80, 50, data[1], 0xffff, 0x0000, 0, 0);
    drawNumber(110, 50, data[2], 0xffff, 0x0000, 0, 0);
    drawString(50, 70, __TIME__, 0xffff, 0x0000);












    while(1);




    battery_init();
    display_init();
    date_init();
    sysTick_init();
    date_init();
    bool osRunning = 1;

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);
    display_backlight(255);

    drawSquare(0,0,239,239,0x0000);
    drawSquare(0,0,50,50,0xffff);


    render_video();
 //   while (1);


    //uart_init();

    int x = 0;


    int i = 320;
    int counter = 0;


    nrf_gpio_cfg_output(15);	
    nrf_gpio_pin_write(15,1);
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLDOWN);


    struct touchPoints touchPoint = {0};
    bool backlight = 0;

    //statusBar_refresh();
    touch_init();

    core_start_process(&main_menu);
    core_start_process(&statusbar);

    while(osRunning) {
        //partialMode(0,90);
        core_run();
        wdt_feed();
    }
}
