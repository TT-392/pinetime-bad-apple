#include "flash.h"
#ifdef SPIFLASH
#if SPIFLASH == 0
#include "resources/video_0.h"
#endif
#if SPIFLASH == 1
#include "resources/video_1.h"
#endif
#if SPIFLASH == 2
#include "resources/video_2.h"
#endif
#if SPIFLASH == 3
#include "resources/video_3.h"
#endif
#if SPIFLASH == 4
#include "resources/video_4.h"
#endif
#if SPIFLASH == 5
#include "resources/video_5.h"
#endif
#if SPIFLASH == 6
#include "resources/video_6.h"
#endif
#if SPIFLASH == 7
#include "resources/video_7.h"
#endif
#if SPIFLASH == 8
#include "resources/video_8.h"
#endif
#if SPIFLASH == 9
#include "resources/video_9.h"
#endif
#if SPIFLASH == 10
#include "resources/video_10.h"
#endif
#if SPIFLASH == 11
#include "resources/video_11.h"
#endif
#if SPIFLASH == 12
#include "resources/video_12.h"
#endif
#if SPIFLASH == 13
#include "resources/video_13.h"
#endif
#endif
#include "display.h"
#include "wdt.h"
#include <stdio.h>
#include "display_print.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

void write_video() {
#ifdef SPIFLASH

    uint64_t addr = SPIFLASH * 303104;

    int sectorSize = 0x1000;

    uint32_t length = videoLength;
    const uint8_t* tempVid = video;

    char buffer[30];
    sprintf(buffer, "Ready to flash: %i", SPIFLASH);
    drawString(0, 0, buffer, 0x0000, 0xffff);

    drawString(8, 110, "<- Start", 0x0000, 0xffff);

    while (!nrf_gpio_pin_read(13))
        wdt_feed();

    drawString(8, 110, "        ", 0x0000, 0xffff);


    int sector = 0;
    while (length > 0) {
        wdt_feed();
        display_pause();

        spiflash_sector_erase(addr);

        int writeLen = length > sectorSize ? sectorSize : length;

        spiflash_write_data(addr, (char*)tempVid, writeLen);

        addr += writeLen;
        tempVid += writeLen;
        length -= writeLen;
        sector++;

        display_resume();

        sprintf(buffer, "Bytes left: %i           ", length);
        drawString(0, 0, buffer, 0x0000, 0xffff);
    }

    sprintf(buffer, "Done flashing: %i        ", SPIFLASH);
    drawString(0, 0, buffer, 0x0000, 0xffff);

#endif
}
