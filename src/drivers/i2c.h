#pragma once
#include <stdint.h>

#define EVENT_START_DATA_RECEIVE 1 << 0
#define EVENT_RECEIVE_DATA_FINISHED 1 << 1

void i2c_setup(volatile uint8_t *data);

void subscribeAndInitI2CInterrupt(void function());

void i2c_sleep();

void i2c_wake();

