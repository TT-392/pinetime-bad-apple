#include "flash.h"
#include "resources/video_3.h"
#include "display.h"

void write_video() {
    uint64_t addr = 3 * videoLength;

    int sectorSize = 0x1000;

    uint32_t length = videoLength;
    const uint8_t* tempVid = video;

    int sector = 0;
    while (length > 0) {
        spiflash_sector_erase(addr);

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
static int ringbuf_read_ptr = 0;
static int ringbuf_write_ptr = 0;
static const uint64_t speed = 900000; // 8 Mbps = 1 MBps * 90% for margin
static const int timePerByte = (64000000 + speed - 1) / speed; // rounded up
static uint32_t readAddr = 0;

void ringbuf_fetch(uint64_t time) {
    uint16_t bytesToFetch = time / timePerByte;
    uint16_t bytesTilEndBuf = ringbuf_size - ringbuf_write_ptr;
    uint16_t freeSpace = ringbuf_read_ptr > ringbuf_write_ptr ? 
            ringbuf_read_ptr - ringbuf_write_ptr - 1: 
            (ringbuf_size - ringbuf_write_ptr) + ringbuf_read_ptr - 1;

    if (bytesToFetch > freeSpace) bytesToFetch = freeSpace;

    if (bytesToFetch <= bytesTilEndBuf) {
        spiflash_read_data(readAddr, (uint8_t*)ringbuf + ringbuf_write_ptr, bytesToFetch);
        readAddr += bytesToFetch;
        ringbuf_write_ptr = (ringbuf_write_ptr + bytesToFetch) % ringbuf_size;
    } else {
        spiflash_read_data(readAddr, (uint8_t*)ringbuf + ringbuf_write_ptr, bytesTilEndBuf);
        readAddr += bytesTilEndBuf;
        ringbuf_write_ptr = 0;
        bytesToFetch -= bytesTilEndBuf;
        spiflash_read_data(readAddr, (uint8_t*)ringbuf, bytesToFetch);
        readAddr += bytesToFetch;
        ringbuf_write_ptr = (ringbuf_write_ptr + bytesToFetch) % ringbuf_size;
    }
}

uint8_t ringbuf_getc() {
    static uint32_t counter = 0;
    if (counter > videoLength*8) {
        while (1);
    }
    counter++;
    if (ringbuf_read_ptr != ringbuf_write_ptr) {
        uint8_t retval = ringbuf[ringbuf_read_ptr];

        ringbuf_read_ptr++;
        if (ringbuf_read_ptr == ringbuf_size)
            ringbuf_read_ptr = 0;

        return retval;
    } else {
        display_pause();
        ringbuf_fetch (1600000);
        display_resume();

        uint8_t retval = ringbuf[ringbuf_read_ptr];

        ringbuf_read_ptr++;
        if (ringbuf_read_ptr == ringbuf_size)
            ringbuf_read_ptr = 0;

        return retval;
    }
}
