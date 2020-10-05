#include <stdint.h>
#include "fake_hardware/display.h"
#include "fake_hardware/nrf_delay.h"

void render_pong(uint8_t bat1, uint8_t ballx, uint8_t bally, uint8_t *blocks) {
    //drawSquare(0,bat1,10,50+bat1,0xffff);
    uint8_t bw = 8;
    uint8_t step = 240/bw;
    for (int i = 0; i < bw; i++) {
        drawSquare(i*step , 0, step+i*step, 10, 0xffff);
    }
}


int main() {
    display_init();
    uint8_t blocks[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

    render_pong(120, 0, 0, blocks);
    usleep(10000000);

}
