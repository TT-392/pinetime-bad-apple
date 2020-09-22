#ifndef emulator
#error THIS FILE IS ONLY MEANT FOR THE EMULATOR, THERE IS A GOOD CHANCE THAT USING THIS OUTSIDE OF THE EMULATOR WILL BRICK YOUR DEVICE
#endif

#pragma once
#include <stdint.h>
#include <stddef.h>

#define bool _Bool


//struct WDT {
//    unsigned char* RR[3];
//};
//
//struct WDT* NRF_WDT;


struct WDT {
    int RR[5];
} *NRF_WDT;


