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

static bool toggle = 1;

#define FLASH_SCK 2 //30
#define FLASH_MOSI 3//11
#define FLASH_MISO 4
#define FLASH_CS 5

// send one byte over spi
void spi_send(uint8_t byte) {
    NRF_SPIM0->TXD.MAXCNT = 1;
    NRF_SPIM0->TXD.PTR = (uint32_t)&byte;

    NRF_SPIM0->EVENTS_ENDTX = 0;
    NRF_SPIM0->EVENTS_ENDRX = 0;
    NRF_SPIM0->EVENTS_END = 0;

    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_ENDTX == 0) __NOP();
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
    NRF_SPIM0->TASKS_STOP = 1;
    while (NRF_SPIM0->EVENTS_STOPPED == 0) __NOP();
}

int main(void) {
    ////////////////
    // setup pins //
    ////////////////
    nrf_gpio_cfg_output(FLASH_MOSI);
    nrf_gpio_cfg_output(FLASH_SCK);
    nrf_gpio_cfg_input(FLASH_MISO, NRF_GPIO_PIN_NOPULL);

    nrf_gpio_cfg_output(FLASH_CS);
    nrf_gpio_pin_write(FLASH_CS,1);

    //nrf_gpio_pin_write(FLASH_CS,1);


    ///////////////
    // spi setup //
    ///////////////
    NRF_SPIM0->PSEL.SCK  = FLASH_SCK;
    NRF_SPIM0->PSEL.MOSI = FLASH_MOSI;
    NRF_SPIM0->PSEL.MISO = FLASH_MISO;

    NRF_SPIM0->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst  << SPIM_CONFIG_ORDER_Pos)|
                        (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                        (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);

    NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M4 << SPIM_FREQUENCY_FREQUENCY_Pos;
    NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;


    nrf_gpio_pin_write(FLASH_CS,0);
    //while (1) {
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
     //   nrf_gpio_pin_write(FLASH_CS,1);
    //}

   // NRF_SPIM0->TASKS_STOP = 1;
   // while (NRF_SPIM0->EVENTS_STOPPED == 0) __NOP();
























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
