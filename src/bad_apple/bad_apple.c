#include "display.h"
#include "nrf.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bad_apple_data.h"
#include "ringbuff.h"
//#include "systick.h"

volatile uint64_t lag = 0;

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
    uint8_t byte;
    int retval = ringbuff_getc(&byte, buffer);
    if (retval == 1)
        bad_apple_fetch_and_decompress(18400);
    return byte;
}

struct dataBlock readBlock(ringbuffer* buffer) {
    struct dataBlock retval = {};
    static int blockCount = 0;
    blockCount++;

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

// 16 bit color:
// lag = 17761904
// 12 bit color:
// lag = 16610091

void render_video() {
    SysTick->LOAD = 2133333;
    SysTick->VAL = 0;
    SysTick->CTRL = 1 << 0;
    //sysTick_init();
    //while (1) {
    //index = 0;
    //uint64_t lastTime = cpuTime();
    ringbuffer *buff50k = bad_apple_init();
    drawSquare(0,0,200,200,0x1111);

    while (1) {
        struct dataBlock data = readBlock(buff50k);

        if (data.eof)
            break;

        if (data.staticFrames) {
            for (int i = 0; i < data.staticAmount; i++) {
                if (!(SysTick->CTRL & (1 << 16))) {
                    bad_apple_fetch_and_decompress(SysTick->VAL);
                }
                while (!(SysTick->CTRL & (1 << 16)));
            }
        } else {
            if (data.newFrame) {
                if (!(SysTick->CTRL & (1 << 16))) {
                    bad_apple_fetch_and_decompress(SysTick->VAL);
                }
                while (!(SysTick->CTRL & (1 << 16)));
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
