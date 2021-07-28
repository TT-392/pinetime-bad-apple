#include "nrf.h"
#include "systick.h"

volatile uint64_t cpuTimeOverflow = 0;

//void SysTick_Handler(void) {
//    cpuTimeOverflow += 16000000;
//}

void sysTick_init() {
    SysTick->LOAD = 16000000;
    SysTick->VAL = 0;
    SysTick->CTRL = 1 << 0 | 1 << 1;

    NVIC_EnableIRQ(SysTick_IRQn);
}

