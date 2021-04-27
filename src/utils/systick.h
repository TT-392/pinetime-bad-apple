#pragma once
#include "nrf.h"


extern volatile uint64_t cpuTimeOverflow;

void sysTick_init();

// overflow takes 9000+ years 
#define cpuTime() (cpuTimeOverflow + ((uint64_t)16000000 - (uint64_t)SysTick->VAL))// + ((SysTick->CTRL & (1 << 16)) ? 16000000 : 0))
