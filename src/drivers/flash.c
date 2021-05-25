#include <nrf.h>
#include <string.h>
#include <stdint.h>
#include <nrf_gpio.h>
#include "flash.h"
#include "break.h"

static void spiWrite(uint8_t *data, int length) {
    NRF_SPIM0->TXD.MAXCNT = length;
    NRF_SPIM0->TXD.PTR = (uint32_t)data;

    NRF_SPIM0->EVENTS_END = 0;
    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
}

static void spiWriteByte(uint8_t byte) {
    nrf_gpio_pin_write(FLASH_CS,0);
    spiWrite(&byte, 1);
    nrf_gpio_pin_write(FLASH_CS,1);
}

static void spiRead(volatile uint8_t *data, int length) {
    NRF_SPIM0->RXD.MAXCNT = length;
    NRF_SPIM0->RXD.PTR = (uint32_t)data;

    NRF_SPIM0->EVENTS_END = 0;
    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
    NRF_SPIM0->RXD.MAXCNT = 0;
}

static uint8_t checkStatus() {
    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t data;
    uint8_t cmd = FLASH_RDSR;
    spiWrite(&cmd, 1);
    spiRead(&data, 1);
    nrf_gpio_pin_write(FLASH_CS,1);
    return (data);
}

void spiflash_sector_erase(int addr) {
    spiWriteByte(FLASH_WREN);

    nrf_gpio_pin_write(FLASH_CS,0);
    uint8_t cmd[] = {FLASH_SE, addr >> 16, addr >> 8, addr};
    spiWrite(cmd, 4);
    nrf_gpio_pin_write(FLASH_CS,1);
    while (checkStatus() & 1);
}

void spiflash_write_data(uint32_t addr, uint8_t* data, uint32_t length) {
    while (length > 0) {
        spiWriteByte(FLASH_WREN);

        nrf_gpio_pin_write(FLASH_CS,0);

        uint8_t cmd[0x80 + 4];
        cmd[0] = FLASH_PP; cmd[1] = addr >> 16; cmd[2] = addr >> 8; cmd[3] = addr;

        int writeLen = length > 0x80 ? 0x80 : length; // not the fastest transfer, but it works

        memcpy(&cmd[4], data, writeLen);
        spiWrite(cmd, sizeof(cmd));

        length -= writeLen;
        addr += writeLen;
        data += writeLen;

        nrf_gpio_pin_write(FLASH_CS,1);

        while (checkStatus() & 1); // potential optimization here, potentially unneccesary idle time
    }
}

void spiflash_read_data(uint32_t addr, uint8_t* data, uint32_t length) {
    while (length > 0) {
        nrf_gpio_pin_write(FLASH_CS,0);

        uint8_t cmd[] = {FLASH_READ, addr >> 16, addr >> 8, addr};
        int readLen = length > 0xff ? 0xff : length;

        spiWrite(cmd, sizeof(cmd));
        spiRead(data, readLen);

        length -= readLen;
        addr += readLen;
        data += readLen;

        nrf_gpio_pin_write(FLASH_CS,1);
    }
}

void spiflash_init() {
    nrf_gpio_cfg_output(FLASH_CS);
    nrf_gpio_pin_write(FLASH_CS,1);

    spiWriteByte(FLASH_RDI);
}

