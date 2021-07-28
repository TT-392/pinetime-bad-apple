#include "display.h"
#include "ringbuff.h"
#include "flash.h"
#include "lz4.h"

static ringbuffer *buff50k;
// NOTE: possible to make this a sort of ringbuffer for a performance gain, might be negligible
static uint8_t spiflashbuffer[255];
static int spiflashbuffer_rptr = 0;
static uint32_t flashAddr = 0;

void spiflash_fetch() {
    display_pause();
    spiflash_read_data(flashAddr, spiflashbuffer, 255);
    flashAddr += 255;
    spiflashbuffer_rptr = 0;
    display_resume();
}

uint8_t spiflash_getc() {
    if (spiflashbuffer_rptr == 255)
        spiflash_fetch();

    return spiflashbuffer[spiflashbuffer_rptr++];
}

int bad_apple_fetch_and_decompress(uint64_t time) {
    static const uint64_t speed = 900000; // 8 Mbps = 1 MBps * 90% for margin
    static const int timePerByte = (64000000 + speed - 1) / speed; // rounded up
    uint16_t byteCount = time / timePerByte;

    static enum lz4_retval lzstatus = LZ4_MOREDATA;
    static uint8_t input;

    static int first = 1;
    if (first) {
        input = spiflash_getc();
        byteCount--;
        first = 0;
    }

    bool dataAdded = 0;
    while (byteCount > 0) {
        lzstatus = lz4_decompress(input, buff50k);
        if (lzstatus == LZ4_MOREDATA) {
            input = spiflash_getc();
            byteCount--;
            dataAdded = 1;
        } else if (lzstatus == LZ4_EOF)
            return 0;
        else
            return 1; // full buffer
    }
    return 0;
}

ringbuffer *bad_apple_init() {
    spiflash_fetch();
    buff50k = ringbuff_create(50001);
    bad_apple_fetch_and_decompress(64000000);
    return buff50k;
}

