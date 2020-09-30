#include <stdint.h>
#include "fake_hardware/display.h"
#include "fake_hardware/nrf_delay.h"

int main() {
    display_init();
    drawSquare(0,0,50,50,0xf800);
    usleep(10000000);
}
