//#include "./external/infinitime/i2c_pine.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "display_defines.h"
#include "display.h"
#include "semihost.h"

#define TOUCH_I2C_DEVICE (0x15)

#define PIN_SCL        (7)
#define PIN_SDA        (6)

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
    if (NRF_TWIM1->EVENTS_TXSTARTED) {
        NRF_TWIM1->EVENTS_TXSTARTED = 0;
        NRF_TIMER1->TASKS_CLEAR = 1;
    }
}


// timer 1 detects if twim is taking too long
void TIMER1_IRQHandler(void) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    // fix i2c when it gets stuck
    
    // disable i2c
    uint32_t twi_state = NRF_TWIM1->ENABLE;
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    // force pins back
    nrf_gpio_cfg_input(PIN_SCL, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);

    NRF_TWIM1->ENABLE = twi_state;

    // restart i2c
    NRF_TWIM1->TASKS_STARTTX = 1;
}

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
};

static volatile uint8_t touch_data[8];
static volatile uint8_t registerNr = 0;

int touch_init() { 
//    semihost_print("enabling touch\n", 15);

    // send a reset
    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(5);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);


    // init i2c // 
    
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;


    nrf_gpio_cfg_input(PIN_SCL, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);

    NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;

    NRF_TWIM1->PSEL.SCL = PIN_SCL;
    NRF_TWIM1->PSEL.SDA = PIN_SDA;

    // setup interrupts
    NRF_TWIM1->INTEN = 0;
    NRF_TWIM1->INTENSET = /*1 << 1 |*/ 1 << 20;

    NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 2);
    NVIC_ClearPendingIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
    NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);

    NRF_TWIM1->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);


    // setup timer which detects if twim gets stuck
    NRF_TIMER1->MODE = 0 << TIMER_MODE_MODE_Pos; // timer mode
    NRF_TIMER1->BITMODE = 0 << TIMER_BITMODE_BITMODE_Pos; // 16 bit
    NRF_TIMER1->PRESCALER = 0xf << TIMER_PRESCALER_PRESCALER_Pos; // 16 MHz
    NRF_TIMER1->CC[0] = 500;
    NRF_TIMER1->INTENSET = 1 << 16;

    NVIC_SetPriority(TIMER1_IRQn, 15); // Lowes priority
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NVIC_EnableIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_START = 1;


    // setup tx //
    NRF_TWIM1->ADDRESS = TOUCH_I2C_DEVICE;
    NRF_TWIM1->TXD.PTR = (uint32_t)&registerNr;
    NRF_TWIM1->TXD.MAXCNT = 1;
    NRF_TWIM1->TASKS_RESUME = 0x1UL;
    NRF_TWIM1->SHORTS = 1 << 7 | /*1 << 12*/ 1 << 10;
    //          //

    // setup rx //
    NRF_TWIM1->ADDRESS = TOUCH_I2C_DEVICE;
    NRF_TWIM1->TASKS_RESUME = 0x1UL;
    NRF_TWIM1->RXD.PTR = (uint32_t)touch_data;
    NRF_TWIM1->RXD.MAXCNT = 8;
    //          //


    return 0;
}


int touch_refresh(struct touchPoints* touchPoint) {
    static bool once = 1;
    if (once) {
        touch_init();
        NRF_TWIM1->TASKS_STARTTX = 1;
        once = 0;
    }

    //semihost_print("reading touch\n", 14);

    touchPoint->gesture = touch_data[1];
    touchPoint->event = touch_data[3] & 0xf;
    touchPoint->touchX = touch_data[4];
    touchPoint->touchY = touch_data[6];
    touchPoint->pressure = touch_data[7];

//    drawNumber(130, 20, touch_data[1], 0xffff, 0x0000, 3, 0);
//    drawNumber(130, 40, touch_data[2], 0xffff, 0x0000, 3, 0);
//    drawNumber(130, 60, touch_data[3], 0xffff, 0x0000, 3, 0);
//    drawNumber(130, 80, touch_data[4], 0xffff, 0x0000, 3, 0);
//    drawNumber(130, 100, touch_data[5], 0xffff, 0x0000, 3, 0);
    drawNumber(130, 120, touch_data[6], 0xffff, 0x0000, 3, 0);
//    drawNumber(130, 140, touch_data[7], 0xffff, 0x0000, 3, 0);

    return 0;

}
