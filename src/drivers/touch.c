//#include "./external/infinitime/i2c_pine.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "display_defines.h"
#include "display.h"
#include "semihost.h"
#include "systick.h"

#define TOUCH_I2C_DEVICE (0x15)

#define PIN_SCL        (7)
#define PIN_SDA        (6)
#define PIN_TouchInt   (28)


static volatile uint8_t touch_data[8] = {0};
static int error = 0;
static int timeOutCount = 0;
static bool touchInt = 0;
static uint8_t tab = 0;

// event = 1: finger lifted from screen
// event = 2; finger touching screen
// event = 0; no contact is being made
// this getting a 2 followed by a 0 without a 1
// seems to indicate a finger bouncing while 
// sliding over the screen this should probably 
// not be counted as a lifting finger

void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
    // start of new data read
    if (NRF_TWIM1->EVENTS_TXSTARTED) {
        NRF_TWIM1->EVENTS_TXSTARTED = 0;
        NRF_TIMER1->TASKS_CLEAR = 1;
        NRF_TIMER1->TASKS_START = 1;
    }

    // end of a data read
    if (NRF_TWIM1->EVENTS_LASTRX) {
        NRF_TWIM1->EVENTS_LASTRX = 0;

        static uint8_t lastEvent = 0;
        static uint8_t touch = 0;

        uint8_t event = touch_data[3] >> 6;

        if (event == 0 && lastEvent == 1) { // finger actually going up
            touch = 0;
        } 
        if (event == 2 && touch == 0) {
            touch = 1;
        }

        tab = touch;

        lastEvent = event;

        NRF_TWIM1->TASKS_STARTTX = 1;
    }
}

void GPIOTE_IRQHandler(void) {
    if (NRF_GPIOTE->EVENTS_IN[2]) {
        NRF_GPIOTE->EVENTS_IN[2] = 0;
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

    NRF_TWIM1->EVENTS_LASTRX = 0;
    error = 1;

    NRF_TIMER1->TASKS_STOP = 1;
    // restart i2c
    NRF_TWIM1->TASKS_STARTTX = 1;
    timeOutCount++;
}

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
    uint8_t error;
    uint8_t tab;
};

static volatile uint8_t registerNr = 0;

int touch_init() { 
    // send a reset
    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(5);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);

    // create GPIOTE event for the int pin
    NRF_GPIOTE->CONFIG[2] = GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos |
        1 << GPIOTE_CONFIG_POLARITY_Pos |
        PIN_TouchInt << GPIOTE_CONFIG_PSEL_Pos;

    // setup interrupt
    NRF_GPIOTE->INTENSET = 1 << 2;
    NVIC_SetPriority(GPIOTE_IRQn, 2);
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);

    // init i2c // 
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    nrf_gpio_cfg_input(PIN_TouchInt, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);

    NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;

    NRF_TWIM1->PSEL.SCL = PIN_SCL;
    NRF_TWIM1->PSEL.SDA = PIN_SDA;

    // setup interrupts
    NRF_TWIM1->INTEN = 0;
    NRF_TWIM1->INTENSET = TWIM_INTENSET_TXSTARTED_Msk | TWIM_INTENSET_LASTRX_Msk;

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
    NRF_TWIM1->SHORTS = TWIM_SHORTS_LASTTX_STARTRX_Msk;
    //          //

    // setup rx //
    NRF_TWIM1->ADDRESS = TOUCH_I2C_DEVICE;
    NRF_TWIM1->TASKS_RESUME = 1;
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

    touchPoint->error = 0;

    if (touch_data[1] != 255 && touch_data[1])
        touchPoint->gesture = touch_data[1];
    else touchPoint->error = 1;

    if (touch_data[3] != 255)
        touchPoint->event = touch_data[3] >> 6;
    else touchPoint->error = 1;

    if (touch_data[4] != 255 && touch_data[4])
        touchPoint->touchX = touch_data[4];
    else touchPoint->error = 1;

    if (touch_data[6] != 255 && touch_data[6])
        touchPoint->touchY = touch_data[6];
    else touchPoint->error = 1;

    if (touch_data[7] != 255)
        touchPoint->pressure = touch_data[7];
    else touchPoint->error = 1;

    touchPoint->tab = tab;

    touchInt = 0;




    return 0;
}
