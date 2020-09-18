#include "../display_print.h"
#include "../external/infinitime/i2c_pine.h"

#define TOUCH_I2C_DEVICE (0x15)

int location[3] = {};
int lastTouch = 0;
int quartersPassed = 0;
int currentQuarter = 0;

char quarter1 [] = "eihugvq";
char quarter2 [] = "tnlmwkz";
char quarter3 [] = "asdfyx";
char quarter4 [] = "orcpbj";

void keyboard_init() {
    writesquare(90, 90, 240-90, 240-90, 0x9492);
    for (int i = 0; i < 240; i++) {
        writesquare(i, i, i, i, 0xffff);
        writesquare(240-i, i, 240-i, i, 0xffff);
    }

    drawSelection (90, 70, quarter1, 0xffff, 0x0000, -1, 0);
    drawSelection (240-8, 80, quarter2, 0xffff, 0x0000, -1, 1);
    drawSelection (30, 220, quarter3, 0xffff, 0x0000, -1, 0);
    drawSelection (0, 80, quarter4, 0xffff, 0x0000, -1, 1);

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

}

void keyboard_loop() {
    uint8_t touch_data[63] = {};

    int error = i2c_read(TOUCH_I2C_DEVICE, 0x00, 63, touch_data);
    if (error) {


        drawNumber(150,50,error,0x0000,0x07E2,1);
    }

    _Bool touch = touch_data[2];
    uint8_t touchX = touch_data[4];
    uint8_t touchY = touch_data[6];

    location[0] = 0;

    // detect touch location
    if (touchX && touchY) {
        if (touchX > 90 && touchX < 240 - 90 && touchY > 90 && touchY < 240 - 90) {
            location[0] = 5;
        } else {
            if (touchX > touchY) {
                if (touchX > 240 - touchY) {
                    location[0] = 2;
                } else {
                    location[0] = 1;
                }
            } else {
                if (touchX > 240 - touchY) {
                    location[0] = 3;
                } else {
                    location[0] = 4;
                }
            }
        }
    }

    if (location[0] != 0) {
        if (location[0] != location[1]) {

            if (location[0] != location[2]) {
                //        for (int i = 0; i < 119; i++) {
                //            writesquare(i+1, i, 240-i-1, i, (location[0] == 1) *0x07e8);
                //        } 

                //        for (int i = 0; i < 119; i++) {
                //            writesquare(i, i+1, i, 240-i-1, (location[0] == 4) * 0xf800);
                //        }

                //        for (int i = 0; i < 119; i++) {
                //            writesquare(i+1, 240-i, 240-i-1, 240-i, (location[0] == 3) * 0x0fff);
                //        }

                //        for (int i = 0; i < 119; i++) {
                //            writesquare(240-i, i+1, 240-i, 240-i-1, (location[0] == 2) * 0xf800);
                //        }

                if (location[1] == 5) {
                    currentQuarter = location[0];
                    quartersPassed = 0;
                }


                drawNumber(150,20,currentQuarter,0xffff,0x0000,3);
                drawNumber(150,50,location[0],0xffff,0x0000,3);

                if (location[0] != 5 && location[1] != 5)
                    quartersPassed++;


    drawSelection (90, 70, quarter1, 0xffff, 0x0000, -1, 0);
    drawSelection (240-8, 80, quarter2, 0xffff, 0x0000, -1, 1);
    drawSelection (30, 220, quarter3, 0xffff, 0x0000, -1, 0);
    drawSelection (0, 80, quarter4, 0xffff, 0x0000, -1, 1);
                // draw letter selection
                switch (currentQuarter) {
                    case 1:
                        drawSelection (90, 70, quarter1, 0xffff, 0x0000, quartersPassed, 0);
                        break;
                    case 2:
                        drawSelection (240-8, 80, quarter2, 0xffff, 0x0000, quartersPassed, 1);
                        break;
                    case 3:
                        drawSelection (30, 220, quarter3, 0xffff, 0x0000, quartersPassed, 0);
                        break;
                    case 4:
                        drawSelection (0, 80, quarter4, 0xffff, 0x0000, quartersPassed, 1);
                        break;
                }
                ////
            }
            location[2] = location[1];
        }

        lastTouch = touch;

        location[1] = location[0];
    }
}
