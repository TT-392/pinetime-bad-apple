#include <stdint.h>

uint32_t adler32(char* data, int length, uint32_t checksum) {
    uint16_t A = checksum & 0xffff;
    uint16_t B = (checksum >> 16) & 0xffff;

    for (int i = 0; i < length; i++) {
        A = (A + data[i]) % 65521;
        B = (B + A) % 65521;
    }
    
    return (B << 16) | A;
}
