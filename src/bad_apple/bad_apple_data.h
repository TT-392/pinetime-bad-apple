#pragma once
#include <stdint.h>
#include "ringbuff.h"

int bad_apple_fetch_and_decompress(uint64_t time);

ringbuffer *bad_apple_init();

