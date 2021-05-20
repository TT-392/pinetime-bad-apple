#pragma once
#include <stdint.h>

// start with a checksum of 1 if not adding to an existing checksum
uint32_t adler32(char* data, int length, uint32_t checksum);
