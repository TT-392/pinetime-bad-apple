#include "flash.h"
#include "resources/video_7.h"
#include "display.h"

void write_video() {
    uint64_t addr = 7 * videoLength;

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
}
