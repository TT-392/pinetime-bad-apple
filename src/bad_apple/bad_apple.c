#include "display.h"
#include "nrf.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bad_apple_data.h"
#include "ringbuff.h"
#include "rtc.c"

struct dataBlock {
    int x1;
    int y1;
    int x2; // relative coords from x1 and y1
    int y2;

    bool newFrame;
    bool flipped;
    bool eof;
    bool staticFrames;
    int staticAmount;

    uint8_t* bitmap;
};

uint8_t bad_apple_getc(ringbuffer* buffer) {
    static bool next_byte_eof = 0;
    if (next_byte_eof)
        __asm__("BKPT");
    uint8_t byte;
    int retval = ringbuff_getc(&byte, buffer);
    if (retval == 1) {
        bad_apple_fetch_and_decompress(18400);
        ringbuff_getc(&byte, buffer);
    } if (retval == -1) {
        next_byte_eof = 1;
    }
    return byte;
}

struct dataBlock readBlock(ringbuffer* buffer) {
    struct dataBlock retval = {};

    retval.eof = 0;

    uint8_t c = bad_apple_getc(buffer);
    retval.newFrame = c & 1;
    bool shortCoords = (c >> 2) & 1;
    retval.flipped = (c >> 1) & 1;
    retval.staticFrames = (c >> 3) & 1;

    if (retval.staticFrames) {
        retval.staticAmount = bad_apple_getc(buffer);
        return retval;
    }

    retval.x1 = bad_apple_getc(buffer);
    retval.y1 = bad_apple_getc(buffer);

    if (shortCoords) {
        c = bad_apple_getc(buffer);
        retval.x2 = c & 0xf;
        retval.y2 = (c >> 4) & 0xf;
    } else {
        retval.x2 = bad_apple_getc(buffer);
        retval.y2 = bad_apple_getc(buffer);
    }

    int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

    retval.bitmap = (uint8_t*)malloc(sizeof(uint8_t) * 1);

    for (int i = 0; i < blockSize + 7 / 8; i++) {
        retval.bitmap[i] = bad_apple_getc(buffer);
    }
    
    //if (index > videoLength)
    //    retval.eof = 1; // TODO potentially reads past end of array

    return retval;
}


void wait_for_next_frame() {
    static int renderedFrames = 0;

    // counter increments at 32768 Hz, beware, will overflow after about 4 minutes.
    while (((uint64_t)NRF_RTC0->COUNTER * 15) / 16384 < renderedFrames)
        bad_apple_fetch_and_decompress(18400);
    renderedFrames++;
}

void render_video() {
    rtc_setup();

    ringbuffer *buff50k = bad_apple_init();
    drawSquare(0,0,200,200,0x1111);

    while (1) {
        struct dataBlock data = readBlock(buff50k);

        if (data.eof)
            break;

        if (data.staticFrames) {
            for (int i = 0; i < data.staticAmount; i++) {
                wait_for_next_frame();
            }
        } else {
            if (data.newFrame) {
                wait_for_next_frame();
            }

            static bool flipped = 1;
            if (data.flipped) {
                flipped = !flipped;
                flip (flipped);
            }

            drawMono(data.x1, data.y1, data.x2+data.x1, data.y2+data.y1, data.bitmap, 0xffff, 0x0000);

            free(data.bitmap);
        }

    }
    while (1);
    //}
}
