#ifndef emulator
#error THIS FILE IS ONLY MEANT FOR THE EMULATOR, THERE IS A GOOD CHANCE THAT USING THIS OUTSIDE OF THE EMULATOR WILL BRICK YOUR DEVICE
#endif

#pragma once

#include <unistd.h>

void nrf_delay_ms (long int ms) {usleep(ms*1000);}
void nrf_delay_us (long int us) {usleep(us);}
