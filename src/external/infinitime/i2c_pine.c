// this file is ported to C from the infinitime driver
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "display_defines.h"

#define PIN_SCL        (7)
#define PIN_SDA        (6)


// TODO use shortcut to automatically send STOP when receive LastTX, for example
// TODO use DMA/IRQ

#define reset_events() NRF_TWIM1->EVENTS_LASTRX = 0;\
  NRF_TWIM1->EVENTS_STOPPED = 0;\
  NRF_TWIM1->EVENTS_LASTTX = 0;\
  NRF_TWIM1->EVENTS_ERROR = 0;\
  NRF_TWIM1->EVENTS_RXSTARTED = 0;\
  NRF_TWIM1->EVENTS_SUSPENDED = 0;\
  NRF_TWIM1->EVENTS_TXSTARTED = 0;\

volatile bool lasttxInterrupt = 0;

//void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
//    if (NRF_TWIM1->EVENTS_LASTTX) {
//        NRF_TWIM1->EVENTS_LASTTX = 0;
//        lasttxInterrupt = 1;
//    }
//}

void i2c_setup() {
    // disable i2c
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;


    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);

	NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400 << TWIM_FREQUENCY_FREQUENCY_Pos;

    NRF_TWIM1->PSEL.SCL = PIN_SCL;
    NRF_TWIM1->PSEL.SDA = PIN_SDA;

    NRF_TWIM1->INTEN = 0;
    NRF_TWIM1->INTENSET = 1 << 24;

	NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 2);
    NVIC_ClearPendingIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
    NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);

    reset_events();
    NRF_TWIM1->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
}

// fix i2c when it gets stuck
void i2c_fix() {
    // disable i2c
    uint32_t twi_state = NRF_TWIM1->ENABLE;
    NRF_TWIM1->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    // forcee pins back
    nrf_gpio_cfg_input(PIN_SCL, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(PIN_SDA, NRF_GPIO_PIN_PULLUP);


    reset_events();
    NRF_TWIM1->ENABLE = twi_state;
}


int Read_noblock (uint8_t deviceAddress, uint8_t *buffer, size_t size) {
    static bool newRead = 1;
    static int counter = 0;
    static int stop = 0;
    if (newRead) {
        reset_events();
        NRF_TWIM1->ADDRESS = deviceAddress;
        NRF_TWIM1->TASKS_RESUME = 0x1UL;
        NRF_TWIM1->RXD.PTR = (uint32_t)buffer;
        NRF_TWIM1->RXD.MAXCNT = size;

        NRF_TWIM1->TASKS_STARTRX = 1;
        newRead = 0;
        counter = 0;
        stop = 0;
    }

    if (!stop) {
        if ((!NRF_TWIM1->EVENTS_RXSTARTED || !NRF_TWIM1->EVENTS_LASTRX) & !NRF_TWIM1->EVENTS_ERROR){
            counter++;
            if (counter > 5000) {
                newRead = 1;
                return 2;
            }
            return 0;
        } else stop = 2;
    }

    if (stop) {
        if (stop == 2) {
            NRF_TWIM1->TASKS_STOP = 0x1UL;
            stop = 1;
            counter = 0;
        }
        if (!NRF_TWIM1->EVENTS_STOPPED){
            counter++;
            if (counter > 5000) {
                newRead = 1;
                return 2;
            }
            return 0;
        }

    }

    newRead = 1;
    return 1;
}

int Read(uint8_t deviceAddress, uint8_t *buffer, size_t size, bool stop) {
    int returnValue;
    do {
        returnValue = Read_noblock(deviceAddress, buffer, size);
    } while (returnValue == 0);

    if (returnValue == 2)
        return 2;
    else return 0;
}

int i2c_write (uint8_t deviceAddress, uint8_t *data, size_t size, bool dummy) {
    static bool newWrite = 1;
    static int counter = 0;
    static int stop = 1;
    static bool once = 1;
    newWrite = 1;
    if (newWrite) {
        reset_events();
        NRF_TWIM1->ADDRESS = deviceAddress;
        NRF_TWIM1->TASKS_RESUME = 0x1UL;
        NRF_TWIM1->TXD.PTR = (uint32_t)data;
        NRF_TWIM1->TXD.MAXCNT = size;
        NRF_TWIM1->SHORTS = 1 << 8;

        NRF_TWIM1->TASKS_STARTTX = 1;
        newWrite = 0;
        counter = 0;
        stop = 0;
        lasttxInterrupt = 0;
    }


    while(!NRF_TWIM1->EVENTS_SUSPENDED){
        counter++;
        if (counter > 5000) return 9;
    }
    NRF_TWIM1->EVENTS_SUSPENDED = 0x0UL;

    newWrite = 1;
    return 0;
}

int i2c_read(uint8_t deviceAddress, uint8_t registerAddress, size_t size, uint8_t *data) {
    int error = 0;
    error = i2c_write(deviceAddress, &registerAddress, 1, false);
    if (error == 0) {
        error = Read(deviceAddress, data, size, true);
    }
    if (error) {
        i2c_fix();
    }
    return error;
}


