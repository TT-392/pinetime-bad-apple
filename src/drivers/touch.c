//#include "./external/infinitime/i2c_pine.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "display_defines.h"
#include "display.h"
#include "systick.h"
#include "i2c.h"
#include <math.h>
#include <stdlib.h>


#define TOUCH_I2C_DEVICE (0x15)

#define PIN_SCL        (7)
#define PIN_SDA        (6)
#define PIN_TouchInt   (28)

static volatile uint8_t touch_data[8] = {0};
static int error = 0;
static int timeOutCount = 0;
static bool touchInt = 0;
static uint8_t tab = 0;
static int errorCount = 0;
static uint64_t time = 0;
static uint64_t fingerStatus = 0;
static _Bool newData = 0;

// event = 1: finger lifted from screen
// event = 2; finger touching screen
// event = 0; no contact is being made
// this getting a 2 followed by a 0 without a 1
// seems to indicate a finger bouncing while 
// sliding over the screen this should probably 
// not be counted as a lifting finger

static bool backlight = 1;
static uint8_t Xdistance = 0;
static uint8_t Ydistance = 0;

void i2cInterruptHandler(int event) {
    // end of a data read
        
    if (event & EVENT_RECEIVE_DATA_FINISHED) {

        // detect faulty data
        if (touch_data[4] != 255 && touch_data[4] != 0 &&
            touch_data[6] != 255 && touch_data[6] != 0) {
            newData = 1;

            static int lastTab = 0;

            if (touch_data[1] == 5 && lastTab == 0)
                tab = 1;

            lastTab = touch_data[1] == 5;



            static uint8_t lastEvent = 0;
            static uint8_t touch = 0;
            uint8_t touchX = touch_data[4];
            uint8_t touchY = touch_data[6];

            uint8_t event = touch_data[3] >> 6;

            static int status = 0;

            if (event == 0 && lastEvent == 1) { // finger actually going up
                touch = 0;
                fingerStatus = 0;
            } 


            if (event == 2 && touch == 0) { // finger actually going down
                touch = 1;
                fingerStatus = 1;
            }


            lastEvent = event;
        } else {
            error = 1;
        }
    }
}



static void (*touchInterrupt)();
static bool interruptInitialized = 0;

void GPIOTE_IRQHandler(void) {
    if (NRF_GPIOTE->EVENTS_IN[2]) {
        NRF_GPIOTE->EVENTS_IN[2] = 0;
        if (interruptInitialized) {
            touchInterrupt();
        }

    } 
}

void subscribeTouchInterrupt(void function()) {
    touchInterrupt = function; 
    interruptInitialized = 1;
}

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
    uint8_t error;
    uint8_t tab;
    uint8_t fingerStatus;
    bool New;
    int errorCount;
};



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
        PIN_TouchInt  << GPIOTE_CONFIG_PSEL_Pos;

    // setup interrupt
    NRF_GPIOTE->INTENSET = 1 << 2;
    NVIC_SetPriority(GPIOTE_IRQn, 2);
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);

    i2c_setup(touch_data);
    subscribeAndInitI2CInterrupt(i2cInterruptHandler);


    return 0;
}

void touch_sleep() {
    i2c_sleep();
}

void touch_wake() {
    i2c_wake();
}


int touch_refresh(struct touchPoints* touchPoint) {
    static bool once = 1;
    if (once) {
        touch_init();
        once = 0;
    }

    //semihost_print("reading touch\n", 14);

    //touchPoint->error = 0;

    if (touch_data[1] != 255 && touch_data[1])
        touchPoint->gesture = touch_data[1];
    else error = 1;

    if (touch_data[3] != 255)
        touchPoint->event = touch_data[3] >> 6;
    else error = 1;

    if (touch_data[4] != 255 && touch_data[4] != 0)
        touchPoint->touchX = touch_data[4];
    else {
        error = 1;
    }

    if (touch_data[6] != 255 && touch_data[6] != 0)
        touchPoint->touchY = touch_data[6];
    else {
        error = 1;
    }

    if (touch_data[7] != 255)
        touchPoint->pressure = touch_data[7];
    else error = 1;

    touchPoint->error = error;
    touchPoint->errorCount = sqrt(pow(Xdistance,2) + pow(Ydistance,2));
    
    touchPoint->fingerStatus = fingerStatus;

    touchPoint->New = newData;
    newData = 0;

    error = 0;
    touchPoint->tab = tab;
    tab = 0;

    touchInt = 0;



    time++;

    return 0;
}
