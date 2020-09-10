#include "../display_print.h"
#include "../timecake/i2c_pine.c"

#define TOUCH_I2C_DEVICE (0x15)

int lastCorner = 0;
int lastTouch = 0;
int quartersPassed = 0;

char quarter1 [] = "eihugvq";
char quarter2 [] = "tnlmwkz";
char quarter3 [] = "asdfyx";
char quarter4 [] = "orcpbj";

void keyboard_init() {
    for (int i = 0; i < 240; i++) {
        writesquare(i, i, i, i, 0xffff);
        writesquare(240-i, i, 240-i, i, 0xffff);
    }



    drawSelection (30, 0, quarter1, 0xffff, 0x0000, -1, 0);
    drawSelection (140, 120, quarter2, 0xffff, 0x0000, -1, 0);
    drawSelection (30, 220, quarter3, 0xffff, 0x0000, -1, 0);
    drawSelection (0, 120, quarter4, 0xffff, 0x0000, -1, 0);

    i2c_setup();
}

void keyboard_loop() {
    // if (touchEvent) {
    uint8_t touch_data[11];

    if (i2c_read(TOUCH_I2C_DEVICE,0x00,8,touch_data)) {
        i2c_setup();
        nrf_gpio_cfg_output(10);
        nrf_gpio_pin_write(10,0);
        nrf_delay_ms(20);
        nrf_gpio_pin_write(10,1);
        nrf_delay_ms(20);

    }

    _Bool touch = touch_data[2];
    uint8_t touchX = touch_data[4];
    uint8_t touchY = touch_data[6];

    //writesquare(touchX, touchY, touchX+1, touchY+1, 0x07E8);

    int corner = 0;

    if (touchX && touchY) {
        if (touchX > 90 && touchX < 240 - 90 && touchY > 90 && touchY < 240 - 90) {
            corner = 5;
        } else {
            if (touchX > touchY) {
                if (touchX > 240 - touchY) {
                    corner = 2;
                } else {
                    corner = 1;
                }
            } else {
                if (touchX > 240 - touchY) {
                    corner = 3;
                } else {
                    corner = 4;
                }
            }
        }
    }

    if (corner != lastCorner && corner != 0) {

        //        for (int i = 0; i < 119; i++) {
        //            writesquare(i+1, i, 240-i-1, i, (corner == 1) *0x07e8);
        //        } 

        //        for (int i = 0; i < 119; i++) {
        //            writesquare(i, i+1, i, 240-i-1, (corner == 4) * 0xf800);
        //        }

        //        for (int i = 0; i < 119; i++) {
        //            writesquare(i+1, 240-i, 240-i-1, 240-i, (corner == 3) * 0x0fff);
        //        }

        //        for (int i = 0; i < 119; i++) {
        //            writesquare(240-i, i+1, 240-i, 240-i-1, (corner == 2) * 0xf800);
        //        }
        int currentQuarter;
        if (lastCorner == 5) {
            currentQuarter = corner;
            quartersPassed = 0;
        }
        if (touch && !lastTouch) {
       //     currentQuarter = corner;
        }
        if (corner != 0) {
            drawNumber(150,50,corner,0xffff,0x0000,1);
            drawNumber(150,20,currentQuarter,0xffff,0x0000,1);
            if ((lastCorner == corner - 1) || (lastCorner == 4 && corner == 1))
                quartersPassed++;


            switch (currentQuarter) {
                case 1:
                    drawSelection (30, 0, quarter1, 0xffff, 0x0000, quartersPassed, 0);
                    break;
                case 2:
                    drawSelection (140, 120, quarter2, 0xffff, 0x0000, quartersPassed, 0);
                    break;
                case 3:
                    drawSelection (30, 220, quarter3, 0xffff, 0x0000, quartersPassed, 0);
                    break;
                case 4:
                    drawSelection (0, 120, quarter4, 0xffff, 0x0000, quartersPassed, 0);
                    break;
            }

        }
    }
    lastTouch = touch;
    if (corner != 0)
        lastCorner = corner;
}
