#pragma once
#include <stdint.h>

#define FLASH_WREN 0x06
#define FLASH_WRDI 0x04
#define FLASH_PP 0x02
#define FLASH_SE 0x20
#define FLASH_READ 0x03
#define FLASH_RDID 0x9F
#define FLASH_RDSR 0x05
#define FLASH_RDI 0xab

#define FLASH_CS 5

uint8_t checkStatus();

void spiflash_sector_erase(uint32_t addr);

void spiflash_write_data(uint32_t addr, uint8_t* data, uint32_t length);

void spiflash_read_data(uint32_t addr, uint8_t* data, uint32_t length);

void spiflash_init();
