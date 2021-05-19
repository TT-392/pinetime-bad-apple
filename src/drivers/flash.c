#include <nrf.h>
#include <stdint.h>
#include <nrf_gpio.h>
#include "flash.h"
#include <nrf_delay.h>
#define FLASH_CS 5

static void spiWrite(uint8_t *data, int length) {
    NRF_SPIM0->TXD.MAXCNT = length;
    NRF_SPIM0->TXD.PTR = (uint32_t)data;

    NRF_SPIM0->EVENTS_END = 0;
    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
}

static void spiRead(volatile uint8_t *data, int length) {
    NRF_SPIM0->RXD.MAXCNT = length;
    NRF_SPIM0->RXD.PTR = (uint32_t)data;

    NRF_SPIM0->EVENTS_END = 0;
    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
    NRF_SPIM0->RXD.MAXCNT = 0;
}

uint16_t checkStatus() {
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t data[2] = {0,0};
    uint8_t RDSR[] = {0x05};
    spiWrite(RDSR, sizeof(RDSR));

    spiRead(data, 2);
    nrf_gpio_pin_write(FLASH_CS,1);

    return (data[0] + (data[1] << 8));
}

void spiflash() {

    //spinor_word_write(0, 69);

    nrf_gpio_cfg_output(FLASH_CS);
    nrf_gpio_pin_write(FLASH_CS,1);


    // init
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t RDP[] = {0xab};
    spiWrite(RDP, sizeof(RDP));
    nrf_gpio_pin_write(FLASH_CS,1);
    nrf_delay_ms(1000);
    /////////
    
    // write enable
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t WREN[] = {0x06};
    spiWrite(WREN, sizeof(WREN));
    nrf_gpio_pin_write(FLASH_CS,1);
    /////////
    nrf_delay_ms(100);

    // sector erase
    nrf_gpio_pin_write(FLASH_CS,0);
    int addr = 0;
    uint8_t erasecmd[] = {FLASH_SE, addr >> 16, addr >> 8, addr};
    spiWrite(erasecmd, sizeof(erasecmd));
    nrf_gpio_pin_write(FLASH_CS,1);
    /////////
    nrf_delay_ms(100);


    // write enable
    nrf_gpio_pin_write(FLASH_CS,0);
    spiWrite(WREN, sizeof(WREN));
    nrf_gpio_pin_write(FLASH_CS,1);
    /////////
    nrf_delay_ms(100);

    // write data
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t cmd[] = {FLASH_PP, addr >> 16, addr >> 8, addr, 69, 69, 69};
    spiWrite(cmd, sizeof(cmd));
    nrf_gpio_pin_write(FLASH_CS,1);
    /////////
    
   // volatile uint16_t status;
   // status = checkStatus();
    nrf_delay_ms(100);
   // status = checkStatus();

    // read data
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t readcmd[] = {FLASH_READ, addr >> 16, addr >> 8, addr};
    volatile uint8_t data[10] = {0};
    spiWrite(readcmd, sizeof(readcmd));
    spiRead(data, 10);
    nrf_gpio_pin_write(FLASH_CS,1);
    /////////
    nrf_delay_ms(100);


    /*

    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t pageRead[4] = {FLASH_RDID, 0, 0, 0};
    spiWrite(pageRead, 1);

    volatile uint8_t data[10] = {0, 1, 2};

    spiRead(data, 3);

    nrf_gpio_pin_write(FLASH_CS,1);
    */

    

    while(1);
}
