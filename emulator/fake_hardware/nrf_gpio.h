#ifndef emulator
#error THIS FILE IS ONLY MEANT FOR THE EMULATOR, THERE IS A GOOD CHANCE THAT USING THIS OUTSIDE OF THE EMULATOR WILL BRICK YOUR DEVICE
#endif
#pragma once

#define NRF_GPIO_PIN_NOPULL 0

_Bool nrf_gpio_pin_read(int i) {return 1;}
void nrf_gpio_cfg_output(int i) {}
void nrf_gpio_cfg_input(int i, int j) {}
void nrf_gpio_pin_write(int i, _Bool j) {}
