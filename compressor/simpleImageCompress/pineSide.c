
    uint8_t frame[originalFrameLength] = {};
    _Bool currentColor = 0;
    uint16_t binAddress = 0;
    for (int i = 0; i < compressedLength; i++) {
        for (int j = 0; j < compressedFrame[i]; j++) {
            frame[binAddress / 8] |= currentColor << (binAddress % 8);
            binAddress++;
        }
        currentColor = !currentColor;
    }


   drawMono(0, 0, 239, 239, frame, 0x0000, 0xffff);


