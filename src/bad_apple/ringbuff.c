#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t *buffer;
    int size;
    int read_ptr;
    int filled_space;
} ringbuffer;

ringbuffer *ringbuff_create(int size) {
    ringbuffer *ringbuff = malloc(sizeof(ringbuffer));
    ringbuff->buffer = malloc(sizeof(int) * size);
    ringbuff->size = size;
    ringbuff->read_ptr = 0;
    ringbuff->filled_space = 0;
    return ringbuff;
}

void ringbuff_destroy(ringbuffer *ringbuff) {
    free(ringbuff->buffer);
    free(ringbuff);
}

// returns 1 when no more data is available
int ringbuff_getc(uint8_t *byte, ringbuffer *ringbuff) {
    if (ringbuff->filled_space == 0) 
        return 1;

    *byte = ringbuff->buffer[ringbuff->read_ptr];

    ringbuff->filled_space--; 
    ringbuff->read_ptr++;
    if (ringbuff->read_ptr == ringbuff->size)
        ringbuff->read_ptr = 0;
    
    return 0;
}

// get byte backwards relative to write pointer (1 = last written byte), (0 = undefined behavior)
uint8_t ringbuff_getc_wrptr(int relative_address, ringbuffer *ringbuff) {
    static int maxAddr = 0;
    if (relative_address > maxAddr) {
        maxAddr = relative_address;
        printf("%i\n", maxAddr);
    }

    int addr = ringbuff->read_ptr + ringbuff->filled_space - relative_address;

    if (addr >= ringbuff->size)
        addr -= ringbuff->size;

    return ringbuff->buffer[addr];
}

// returns free space before writing
int ringbuff_putc(uint8_t byte, ringbuffer *ringbuff) {
    if (ringbuff->filled_space == ringbuff->size) {
        printf("full\n");
        return 0;
    }

    ringbuff->buffer[(ringbuff->read_ptr + ringbuff->filled_space++) % ringbuff->size] = byte;

    return ringbuff->size - ringbuff->filled_space + 1;
}
