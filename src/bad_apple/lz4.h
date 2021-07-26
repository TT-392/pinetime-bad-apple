#pragma once
#include <stdint.h>
#include <stdbool.h>

enum lz4_retval {
    LZ4_BUFFERFULL,
    LZ4_MOREDATA
};

enum lz4_retval lz4_decompress (uint8_t input, ringbuffer *buff);
