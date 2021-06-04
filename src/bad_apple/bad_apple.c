#include "display.h"
#include "nrf.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bad_apple_flash.h"
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

struct dataBlock readBlock() {
    struct dataBlock retval = {};

    retval.eof = 0;

    uint8_t c = ringbuf_getc();
    retval.newFrame = c & 1;
    bool shortCoords = (c >> 2) & 1;
    retval.flipped = (c >> 1) & 1;
    retval.staticFrames = (c >> 3) & 1;

    if (retval.staticFrames) {
        retval.staticAmount = ringbuf_getc();
        return retval;
    }

    retval.x1 = ringbuf_getc();
    retval.y1 = ringbuf_getc();

    if (shortCoords) {
        c = ringbuf_getc();
        retval.x2 = c & 0xf;
        retval.y2 = (c >> 4) & 0xf;
    } else {
        retval.x2 = ringbuf_getc();
        retval.y2 = ringbuf_getc();
    }

    int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

    retval.bitmap = malloc(blockSize);

    for (int i = 0; i < blockSize + 7 / 8; i++) {
        retval.bitmap[i] = ringbuf_getc();
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
        display_pause();
        ringbuf_fetch(64000000);
        display_resume();
        while (1) {
            struct dataBlock data = readBlock();

            if (data.eof)
                break;

            if (data.staticFrames) {
                for (int i = 0; i < data.staticAmount; i++) {
                    if (!(SysTick->CTRL & (1 << 16))) {
                        display_pause();
                        ringbuf_fetch (SysTick->VAL);
                        display_resume();
                    }
                    while (!(SysTick->CTRL & (1 << 16)));
                }
            } else {
                if (data.newFrame) {
                    //uint64_t newTime = cpuTime();

                    //                if (newTime > lastTime + 2133333)
                    //                    lag += newTime - (lastTime + 2133333);


                    // if (newTime < lastTime + 2133333) {
                    //     display_pause();
                    //     ringbuf_fetch ((lastTime + 2133333) - newTime);
                    //     display_resume();
                    // }

                    if (!(SysTick->CTRL & (1 << 16))) {
                        display_pause();
                        ringbuf_fetch (SysTick->VAL);
                        display_resume();
                    }
                    while (!(SysTick->CTRL & (1 << 16)));
                    //while (newTime < lastTime + 2133333)
                    //    newTime = cpuTime();

                    // lastTime = newTime;
                    //__asm__("BKPT");
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
