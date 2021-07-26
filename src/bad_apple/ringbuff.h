#pragma once
#include <stdint.h>

typedef struct {
    uint8_t *buffer;
    int size;
    int read_ptr;
    int filled_space;
} ringbuffer;

ringbuffer *ringbuff_create(int size);

void ringbuff_destroy(ringbuffer *ringbuff);

// returns 1 when no more data is available
int ringbuff_getc(uint8_t *byte, ringbuffer *ringbuff);

// get byte backwards relative to write pointer (1 = last written byte), (0 = undefined behavior)
uint8_t ringbuff_getc_wrptr(int relative_address, ringbuffer *ringbuff);

// returns free space before writing
int ringbuff_putc(uint8_t byte, ringbuffer *ringbuff);
