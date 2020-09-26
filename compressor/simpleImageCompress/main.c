#include <stdint.h>
#include <stdio.h>
#include "frame.c"

int main() {
    uint8_t temp[240*240] = {};
    int length = 0;

    printf("%i\n",sizeof(frame));

    _Bool currentColor = 0;
    uint8_t count = 0;
    // not sure why the 1+
    for (int i = 0; i < (1+sizeof(frame))*8; i++) {
        if (count == 0xff) {
            temp[length] = count;
            length++;
            temp[length] = 0;
            length++;
            count = 0;
        } else if (((frame[i / 8]) >> (i % 8)) % 2 != currentColor) {
            currentColor = !currentColor;
            temp[length] = count;
            count = 0;
            length++;
        }
        count++;
    }
    uint8_t compressedFrame[length];
    for (int i = 0; i < length; i++) {
        compressedFrame[i] = temp[i];
    }
    printf("%i\n",sizeof(compressedFrame));

    FILE *fp;

    fp = fopen("test.c", "w+");

    fprintf(fp, "#define originalFrameLength %i\n",sizeof(frame));
    fprintf(fp, "#define compressedLength %i\nuint8_t compressedFrame[%i] = {\n",length, length);
    for (int i = 0; i < length - 1; i++) {
        fprintf(fp, "0x%02x,", compressedFrame[i]);
        if (i % 16 == 15) {
            fprintf(fp, "\n");
        }
    }
    fprintf(fp, "0x%02x", compressedFrame[length]);

    fprintf(fp, "};");
    fclose(fp);
    
    int total = 0;
    for (int i = 0; i < length; i++) {
        total += compressedFrame[i];
    }
    printf("%i\n", total);
}
