#include "flash.h"
#include "video.h"

void write_video() {
    int sectorSize = 0x1000;

    uint32_t length = videoLength;
    const uint8_t* tempVid = video;

    int sector = 0;
    uint32_t addr = 0;
    while (length > 0) {
        spiflash_sector_erase(sectorSize * sector);

        int writeLen = length > sectorSize ? sectorSize : length;

        spiflash_write_data(addr, (char*)tempVid, writeLen);

        addr += writeLen;
        tempVid += writeLen;
        length -= writeLen;
        sector++;
    }
}

static uint8_t ringbuf[0xa000];
static const int ringbuf_size = 0xa000;
static int ringbuf_read_pointer = 0;
static int ringbuf_write_pointer = 0;
#define speed 900000 // 8 Mbps = 1 MBps times 90% for margin

void ringbuf_fetch(double seconds) {
    
}

uint8_t ringbuf_getc() {
    if (ringbuf_read_pointer != ringbuf_write_pointer) {
        uint8_t retval = ringbuf[ringbuf_read_pointer];
        ringbuf_read_pointer++;

        if (ringbuf_read_pointer == ringbuf_size)
            ringbuf_read_pointer = 0;

        return retval;
    } else {
        // help, abort
    }
}
