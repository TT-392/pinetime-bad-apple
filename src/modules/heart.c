#include "../display_print.h"
#include "../timecake/i2c_pine.c"

#define HEART_I2C_DEVICE (0x44)


void heart_init() {

    i2c_setup();
    uint8_t touch_data[11];
    i2c_read(HEART_I2C_DEVICE,0x17,8,touch_data);
    drawNumber(150,50,touch_data[0],0x0000,0xffff,1);
}

void heart_loop() {
 //   drawNumber(150,50,corner,0xffff,0x0000,1);
 //   drawNumber(150,20,currentQuarter,0xffff,0x0000,1);
}
