#pragma once

extern volatile uint64_t cpuTimeOverflow;

void sysTick_init();

// overflow takes 9000+ years 
#define cpuTime() (cpuTimeOverflow + ((uint64_t)16000000 - (uint64_t)SysTick->VAL))
