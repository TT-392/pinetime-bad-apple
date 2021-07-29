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
#endif
#include "display.h"

void write_video() {
#ifdef SPIFLASH
#pragma message "adding spiflash data"

    uint64_t addr = SPIFLASH * 491520;

    int sectorSize = 0x1000;

    uint32_t length = videoLength;
    const uint8_t* tempVid = video;

    int sector = 0;
    display_pause();
    while (length > 0) {
        spiflash_sector_erase(addr);

        int writeLen = length > sectorSize ? sectorSize : length;

        spiflash_write_data(addr, (char*)tempVid, writeLen);

        addr += writeLen;
        tempVid += writeLen;
        length -= writeLen;
        sector++;
    }
    display_resume();
#endif
}
