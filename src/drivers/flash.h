#pragma once

#define FLASH_WREN 0x06
#define FLASH_WRDI 0x04
#define FLASH_PP 0x02
#define FLASH_SE 0x20
#define FLASH_READ 0x03
#define FLASH_RDID 0x9F
#define FLASH_RDSR 0x05
#define FLASH_RDI 0xab

void spiflash();
