#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "ringbuff.c"

ringbuffer *buff;

uint8_t mock_ringbuf[40000000];
#define buflen 40000000
int ringInd = 0;

void ringbuf_print() {
    for (int i = 0; i < ringInd; i++)
        printf("%x, ", mock_ringbuf[i]);
    //mock_ringbuf[ringInd + 1] = '\0';
    //printf("%s\n", mock_ringbuf);
}

void ringbuf_writeFile(FILE *file, bool finish) {
    int size = buff->filled_space;
    if (!finish)
        size -= 10000;

    printf("size: %i\n", size);

    for (int i = 0; i < size; i++) {
        uint8_t byte;
        ringbuff_getc(&byte, buff);
        putc(byte, file);
    }
}

enum state {
    STATE_READHDR,
    STATE_GETBLOCKSIZE,
    STATE_READ_UNCOMPRESSED,
    STATE_START_READ_COMPRESSED,
    STATE_LONGER_LITERAL,
    STATE_GET_LITERAL,
    STATE_GET_OFFSET,
    STATE_LONGER_MATCHLENGTH,
    STATE_COPY_BYTES
};

enum lz4_retval {
    LZ4_BUFFERFULL,
    LZ4_MOREDATA
};

enum lz4_retval lz4_decompress (uint8_t input) {
    int inLen = 1;
    static int i = 0;
    static uint32_t blockSize;
    static bool compressed;
    static enum state state = STATE_READHDR;
    static uint8_t token;
    static int blockInd = 0;
    static int literals;
    static uint8_t byte;
    static int offset = 0;
    static int matchLength;
    static uint8_t lastByte;

    int spaceleft;
    bool repeat;
    do {
        repeat = 0;
        switch (state) {
        case STATE_READHDR:
            i++;
            if (i == 7) { // ignore header
                state = STATE_GETBLOCKSIZE;
                i = 0;
            }
            break;
        case STATE_GETBLOCKSIZE:
            if (i == 0) {
                blockSize = 0;
                blockInd = 0;
            }
            blockSize |= input << (i*8);
            if (i == 3) {
                compressed = !(blockSize & 0x80000000);
                blockSize &= ~0x80000000;

                i = 0;
                if (compressed)
                    state = STATE_START_READ_COMPRESSED;
                else
                    state = STATE_READ_UNCOMPRESSED;
                break;
            }
            i++;
            break;
        case STATE_READ_UNCOMPRESSED:
            if (i < blockSize) {
                spaceleft = ringbuff_putc(input, buff);

                if (spaceleft == 0) {
                    printf("read_uncompressed\n");
                    return LZ4_BUFFERFULL;
                }
                i++;
            } else {
                i = 0;
                state = STATE_GETBLOCKSIZE;
                repeat = 1;
            }
            break;
        case STATE_START_READ_COMPRESSED:
            token = input;
            blockInd++;
            literals = token >> 4 & 0xf;
            if (literals == 0xf)
                state = STATE_LONGER_LITERAL;
            else
                state = STATE_GET_LITERAL;
            break;
        case STATE_LONGER_LITERAL:
            byte = input;
            blockInd++;
            literals += byte;
            if (byte != 0xff)
                state = STATE_GET_LITERAL;
            break;
        case STATE_GET_LITERAL:
            if (i < literals) {
                spaceleft = ringbuff_putc(input, buff);

                if (spaceleft == 0) {
                    printf("get literal\n");
                    return LZ4_BUFFERFULL;
                }
                blockInd++;
                i++;
            } else if (blockInd == blockSize) {
                i = 0;
                state = STATE_GETBLOCKSIZE;
                repeat = 1;
            } else {
                i = 0;
                offset = 0;
                state = STATE_GET_OFFSET;
                repeat = 1;
            }
            break;
        case STATE_GET_OFFSET:
            if (i < 2) {
                offset |= input << (i*8);
                blockInd++;
                i++;
            } else {
                i = 0;
                matchLength = (token & 0xf);
                if (matchLength == 0xf)
                    state = STATE_LONGER_MATCHLENGTH;
                else {
                    matchLength += 4;
                    state = STATE_COPY_BYTES;
                }
                repeat = 1;
            }
            break;
        case STATE_LONGER_MATCHLENGTH:
            byte = input;
            blockInd++;
            matchLength += byte;
            if (byte != 0xff) {
                matchLength += 4;
                state = STATE_COPY_BYTES;
            }
            break;
        case STATE_COPY_BYTES:
            if (i < matchLength) {
                if (i == 0)
                    lastByte = input;

                spaceleft = ringbuff_putc(ringbuff_getc_wrptr(offset, buff), buff);

                if (spaceleft == 0) {
                    printf("copy_bytes\n");
                    return LZ4_BUFFERFULL;
                }
                i++;
                repeat = 1;
            } else {
                input = lastByte;
                i = 0;
                state = STATE_START_READ_COMPRESSED;
                repeat = 1;
            }
            break;
        }
    } while (repeat);
    return LZ4_MOREDATA;
}

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");
    buff = ringbuff_create(10001);

    if (argc < 2) {
        printf("gimme filename\n");
        return 1;
    }
    printf(argv[1]);
    printf("\n");

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file) - 4;
    fseek(file, 0, SEEK_SET);

    uint8_t buffer[fileSize];
    for (int i = 0; i < fileSize; i++)
        buffer[i] = fgetc(file);

    FILE *outfile = fopen("outfile", "wb");

    int x = 0;
    enum lz4_retval retval = LZ4_MOREDATA;
    for (int i = 0; i < fileSize; i++) {
        //printf("i = %i\n", i);
        retval = lz4_decompress(buffer[i]);
        if (retval != LZ4_MOREDATA) {
            i--;

            x++;
//           if (x%10 == 0)
                ringbuf_writeFile(outfile, 0);
        }
    }
    //ringbuf_print();
    ringbuf_writeFile(outfile, 1);
    fclose(outfile);


    int inPtr = 0;

    fclose(file);
}
