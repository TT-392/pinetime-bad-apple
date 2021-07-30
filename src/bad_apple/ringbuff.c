#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ringbuff.h" 

uint8_t buff50Kalloc[40000];
ringbuffer structAlloc;
ringbuffer *ringbuff_create(int size) {
    ringbuffer *ringbuff = &structAlloc;//malloc(sizeof(ringbuffer));
    ringbuff->buffer = buff50Kalloc;//malloc(sizeof(uint8_t) * size);
    ringbuff->size = 40000;//size;
    ringbuff->read_ptr = 0;
    ringbuff->filled_space = 0;
    ringbuff->eof = -1;
    return ringbuff;
}

void ringbuff_destroy(ringbuffer *ringbuff) {
    //free(ringbuff->buffer);
    //free(ringbuff);
}

// returns 1 when no more data is available, returns -1 if last byte in file (this byte is still a valid byte)
int ringbuff_getc(uint8_t *byte, ringbuffer *ringbuff) {
    if (ringbuff->filled_space == 0) 
        return 1;

    if (ringbuff->read_ptr == ringbuff->eof)
        return -1;

    *byte = ringbuff->buffer[ringbuff->read_ptr];

    ringbuff->filled_space--; 
    ringbuff->read_ptr++;

    ringbuff->read_ptr %= ringbuff->size;
    
    return 0;
}

// get byte backwards relative to write pointer (1 = last written byte), (0 = undefined behavior)
uint8_t ringbuff_getc_wrptr(int relative_address, ringbuffer *ringbuff) {
    static int maxAddr = 0;
    if (relative_address > maxAddr) {
        maxAddr = relative_address;
    }

    int addr = ringbuff->read_ptr + ringbuff->filled_space - relative_address;

    addr %= ringbuff->size;

    return ringbuff->buffer[addr];
}

void ringbuff_putc_eof(ringbuffer *ringbuff) {
    // eof is put on the last valid byte, not the byte after this, this means a
    // full buffer isn't a problem
    ringbuff->eof = (ringbuff->read_ptr + ringbuff->filled_space - 1) % ringbuff->size;
}

// returns free space before writing
int ringbuff_putc(uint8_t byte, ringbuffer *ringbuff) {
    if (ringbuff->filled_space == ringbuff->size) {
        return 0;
    }

    ringbuff->buffer[(ringbuff->read_ptr + ringbuff->filled_space++) % ringbuff->size] = byte;

    return ringbuff->size - ringbuff->filled_space + 1;
}
