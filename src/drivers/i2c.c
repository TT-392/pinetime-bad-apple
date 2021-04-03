#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "i2c.h"


#define PIN_SCL        (7)
#define PIN_SDA        (6)
#define PIN_TouchInt   (28)
#define TOUCH_I2C_DEVICE (0x15)

static volatile uint8_t registerNr = 0;
void i2c_setup(volatile uint8_t *data) {
    // init i2c // 
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    nrf_gpio_cfg_input(PIN_TouchInt, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(PIN_SCL, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);

    NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;

    NRF_TWIM1->PSEL.SCL = PIN_SCL;
    NRF_TWIM1->PSEL.SDA = PIN_SDA;


    // setup interrupts
    NRF_TWIM1->INTEN = 0;
    NRF_TWIM1->INTENSET = TWIM_INTENSET_TXSTARTED_Msk | TWIM_INTENSET_LASTRX_Msk | TWIM_INTENSET_STOPPED_Msk;

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
    NRF_TWIM1->TASKS_RESUME = 1;
    NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STARTTX_Msk;
    //          //

    // setup rx //
    NRF_TWIM1->ADDRESS = TOUCH_I2C_DEVICE;
    NRF_TWIM1->TASKS_RESUME = 1;
    NRF_TWIM1->RXD.PTR = (uint32_t)data;
    NRF_TWIM1->RXD.MAXCNT = 8;
    //          //

    NRF_TWIM1->TASKS_STARTTX = 1;
}

static void (*TWIMInterrupt)(int event);
static bool TWIMInterruptInitialized = 0;

void subscribeAndInitI2CInterrupt(void function()) {
    TWIMInterrupt = function; 
    TWIMInterruptInitialized = 1;
}

static bool i2c_stop = 0;
void i2c_sleep() {
    i2c_stop = 1;
    NRF_TIMER1->TASKS_STOP = 1;
}

void i2c_wake() {
    i2c_stop = 0;
    NRF_TWIM1->EVENTS_LASTRX = 0;
    NRF_TWIM1->EVENTS_TXSTARTED = 0;

    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
}

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
    int event = 0;

    if (NRF_TWIM1->EVENTS_TXSTARTED) {
        NRF_TWIM1->EVENTS_TXSTARTED = 0;
        event |= EVENT_START_DATA_RECEIVE;

        if (event & EVENT_START_DATA_RECEIVE) {
            NRF_TIMER1->TASKS_CLEAR = 1;
            NRF_TIMER1->TASKS_START = 1;
        }
    }

    if (NRF_TWIM1->EVENTS_LASTRX) {
        NRF_TWIM1->EVENTS_LASTRX = 0;
        event |= EVENT_RECEIVE_DATA_FINISHED;
        if (i2c_stop) {
            NRF_TIMER1->TASKS_STOP = 1;
        }
    }

    if (NRF_TWIM1->EVENTS_STOPPED) {
        NRF_TWIM1->EVENTS_STOPPED = 0;
        if (i2c_stop) {
            NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
        }
    }

    if (TWIMInterruptInitialized) {
        TWIMInterrupt(event);
    }
}

// twim timeout interrupt
// interrupt triggers when either i2c gets stuck, or when controller asleep
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

    NRF_TWIM1->EVENTS_LASTRX = 0;

    NRF_TIMER1->TASKS_STOP = 1;
    // restart i2c
    NRF_TWIM1->TASKS_STARTTX = 1;
}

