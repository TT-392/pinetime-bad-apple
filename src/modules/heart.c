#include "../display_print.h"
#include "../external/infinitime/i2c_pine.h"

#define HEART_I2C_DEVICE (0x44)


void heart_init() {

    i2c_setup();
    //uint8_t writedata[2] = {0x01,0x00};
    uint8_t writedata[2] = {0x01,0xff};
    i2c_write(HEART_I2C_DEVICE,writedata,2, false);
    uint8_t writedata2[2] = {0x0c,0x28};
    i2c_write(HEART_I2C_DEVICE,writedata2,2, false);
    uint8_t writedata3[2] = {0x16,0x66};
    i2c_write(HEART_I2C_DEVICE,writedata3,2, false);
    uint8_t writedata4[2] = {0x17,0x10};
    i2c_write(HEART_I2C_DEVICE,writedata4,2, false);
  //  i2c_read(HEART_I2C_DEVICE,0,8,touch_data);
    //drawNumber(150,50,3,0x0000,0xffff,1);
}

void heart_loop() {
    uint8_t touch_data[0x20];
    i2c_read(HEART_I2C_DEVICE,0,0x20,touch_data);
    
    //drawNumber(150,0*16,touch_data[0],0xffff,0x0000,3);
    //drawNumber(150,1*16,touch_data[1],0xffff,0x0000,3);
    //drawNumber(150,2*16,touch_data[8],0xffff,0x0000,3);
    //drawNumber(150,3*16,touch_data[9],0xffff,0x0000,3);
    //drawNumber(150,4*16,touch_data[0xa],0xffff,0x0000,3);
    //drawNumber(150,5*16,touch_data[0xb],0xffff,0x0000,3);
    //drawNumber(150,6*16,touch_data[0xc],0xffff,0x0000,3);
    //drawNumber(150,7*16,touch_data[0xd],0xffff,0x0000,3);
    //drawNumber(150,8*16,touch_data[0xe],0xffff,0x0000,3);
    //drawNumber(150,9*16,touch_data[0xf],0xffff,0x0000,3);
    //drawNumber(150,10*16,touch_data[0x16],0xffff,0x0000,3);
    //drawNumber(150,11*16,touch_data[0x17],0xffff,0x0000,3);
}
