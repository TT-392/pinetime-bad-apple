#include "./external/infinitime/i2c_pine.h"

#define TOUCH_I2C_DEVICE (0x15)

struct touchPoints {
    uint8_t gesture;
    uint8_t event;
    uint8_t touchX;
    uint8_t touchY;
    uint8_t pressure;
};

void touch_init() {
    i2c_setup();

    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(5);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(50);

//    uint8_t dummy = 0;
//    i2c_read(TOUCH_I2C_DEVICE, 0x15, 1, &dummy);
//    nrf_delay_ms(5);
//    i2c_read(TOUCH_I2C_DEVICE, 0xa7, 1, &dummy);
    // not sure if all of this is needed


}

int touch_refresh(struct touchPoints* touchPoint,uint8_t touch_data[8]) {

    int error = i2c_read(TOUCH_I2C_DEVICE, 0x00, 8, touch_data);
    touchPoint->gesture = touch_data[1];
    touchPoint->event = touch_data[2] & 0xf;
    touchPoint->touchX = touch_data[4];
    touchPoint->touchY = touch_data[6];
    touchPoint->pressure = touch_data[8];


    return error;
}
