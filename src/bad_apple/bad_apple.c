#include "video.h"
#include "display.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "systick.h"

struct dataBlock {
    int x1;
    int y1;
    int x2; // relative coords from x1 and y1
    int y2;

    bool newFrame;

    bool eof;

    uint8_t* bitmap;
};

static int index = 0;
struct dataBlock readBlock() {
    struct dataBlock retval = {};

    retval.eof = 0;


    retval.newFrame = video[index];
    index++;

    retval.x1 = video[index];
    index++;

    retval.y1 = video[index];
    index++;

    retval.x2 = video[index];
    index++;

    retval.y2 = video[index];
    index++;

    int blockSize = ((retval.x2+1) * (retval.y2+1) + 7) / 8; // bytes rounded up

    retval.bitmap = malloc(blockSize);

    for (int i = 0; i < blockSize + 7 / 8; i++) {
        retval.bitmap[i] = video[index];
        index++;
    }
    
    if (index > videoLength)
        retval.eof = 1; // TODO potentially reads past end of array

    return retval;
}

void render_video() {
    sysTick_init();
    while (1) {
        index = 0;
        uint64_t lastTime = cpuTime();
        while (1) {
            struct dataBlock data = readBlock();


            if (data.eof)
                break;

            if (data.newFrame) {
                uint64_t newTime = cpuTime();
                while (newTime < lastTime + 2133333)
                    newTime = cpuTime();

                lastTime = newTime;
            }


            drawMono(data.x1, data.y1, data.x2+data.x1, data.y2+data.y1, data.bitmap, 0xffff, 0x0000);

            free(data.bitmap);

        }
    }
}
