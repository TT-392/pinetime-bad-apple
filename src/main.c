#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.c"
#include "display.h"
//#include "touch.c"
#include "modules/steamLocomotive.c"
//#include "modules/keyboard.c"
#include "frame.c"


_Bool toggle = 0;
_Bool touchEvent = 0;
//void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
//    touchEvent = 1;
//    toggle = !toggle;
//}

int main(void) {
    bool osRunning = 1;

    //osRunning = !nrf_sdh_enable_request();
//    sd_softdevice_enable();
    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);


    display_backlight(255);
    display_init();

    drawmono(0, 0, 239, 239, frame, 0x0000, 0xffff);


 //       writesquare(0, 0, 240, 240, 0xffff);
 //       writesquare(0, 0, 240, 240, 0x0000);
    //heart_init();
   // battery_init();
   // battery_percent (200, 0, 0xffff, 0x0000);
//    for (int i = 0; i < 240; i++) {
//        writesquare(i, i, i, i, 0xffff);
//        writesquare(240-i, i, 240-i, i, 0xffff);
//    }

//    keyboard_init();

 //   touch_init();

    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);


    //sl(40, 90, 0xffff, 0x0000);
   // battery_init();
    //date_init();

//    nrf_drv_gpiote_init();
//
//    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
//    in_config.pull = NRF_GPIO_PIN_NOPULL;
//
//    nrf_drv_gpiote_in_init(28, &in_config, in_pin_handler);
//
//    nrf_drv_gpiote_in_event_enable(28, true);


    bool backlight = 1;
    bool lastButState = 0;
    int swipeUp = 0;

    int i = 0;
    while(osRunning) {
       // heart_loop();
        wdt_feed();


//        keyboard_loop();
        //drawDate("%Y-%m-%d  %H:%M:%S");

    //    battery_percent (200, 0, 0xffff, 0x0000);

    //    if (i < 80) {
    //        writesquare(0, 320-i, 240, 320-i, i*6);
    //        scroll(320-i, i, 0, 0);
    //    } else if (i < 160) {
    //        writesquare(0, 320-(i-80), 240, 320-(i-80), i*6);
    //        scroll(320-i, i, 0, 320-80);
    //    }
    //    if (i == 80) {
    //        scroll(0,0,320,0);
    //    }
       // if (i < 80) {
       //     writesquare(0, 320-i, 240, 320-i, i*6);
       //     scroll((i+80), 320-(i+80), 320, 80);
       // }


////#define scrollup
//#ifdef scrollup
//        
//        if (i < 80) {
//            writesquare(0, 239+i, 239, 239+i, (i+80)*200); // write to D
//            writesquare(0, 239-i, 239, 239-i, (160-i)*200); //write to C
//            scroll(239-i,80+i,0,240); // roll D to C
//        } else if (i < 160) {
//            int j = i-80;
//            writesquare(0, 239+j, 239, 239+j, (i+80)*200); // write to D
//            writesquare(0, 239-i, 239, 239-i, (240-i)*200); //write to C
//            scroll(239-80-j,j+160,0,240-80);
//        } else if (i == 160) {
//            scroll(239-79, 80, 80, 320);
//            tempFunction();
//            for (int x = 0; x < 80; x++) {
//                writesquare(0, 239-80+x, 239, 239-80+x, (160+x)*200); //write to C
//            }
//        } else if (i < 240) {
//            int j = i-160;
//            writesquare(0, 239+j, 239, 240+j, (i+80)*200); // write to D
//            scroll(239-160-j,j+239,0,240-160);
//        }
//#endif
//            
//#ifndef scrollup
//        if (i < 80) {
//            writesquare(0, 239+i, 239, 239+i, (i+80)*200); // write to D
//            writesquare(0, 239-i, 239, 239-i, (160-i)*200); //write to C
//            scroll(239-i,80+i,0,240); // roll D to C
//        } else if (i < 160) {
//            int j = i-80;
//            writesquare(0, 239+j, 239, 239+j, (i+80)*200); // write to D
//            writesquare(0, 239-i, 239, 239-i, (240-i)*200); //write to C
//            scroll(239-80-j,j+160,0,240-80);
//        } else if (i == 160) {
//            scroll(239-79, 80, 80, 320);
//            tempFunction();
//            for (int x = 0; x < 80; x++) {
//                writesquare(0, 239-80+x, 239, 239-80+x, (160+x)*200); //write to C
//            }
//        } else if (i < 241) {
//            int j = i-160;
//            writesquare(0, 320-j, 239, 320-j, (80-j)*200); // write to D
//            writesquare(0, 320-j, 239, 320-j, (80-j)*200); // write to D
//            scroll(0, j, 320-j, 320);
//        }
//#endif





    //    writesquare(0, i, 240, i, (80-i)*6);

       // if (i > 80) {
       //     int j = i - 80;
       //     scroll(320-j, j, (j > 80) ? j - 80 : 0, 320 - 80);
       // }

        scroll(0, 400, 0, i);
        nrf_delay_ms(5);
        i++;
        if (i > 400) { 
            i = 0;
//            nrf_delay_ms(1000);
//            drawmono(0, 0, 239, 239, frame, 0x0000, 0xffff);
//            scroll(320,0,0,0);
//            nrf_delay_ms(500);
        }
       // uint8_t touch_data[8] = {};
       // struct touchPoints touchPoint = {};
       // int returnvalue = touch_refresh(&touchPoint, touch_data);

       // 
       // drawNumber(8*3,0*16,touchPoint.touchX,0xffff,0x0000,3);
       // drawNumber(8*3,1*16,touchPoint.touchY,0xffff,0x0000,3);
       // drawNumber(8*3,2*16,touchPoint.event,0xffff,0x0000,3);
       // drawNumber(8*3,3*16,touchPoint.gesture,0xffff,0x0000,3);
       // drawNumber(8*3,4*16,touchPoint.pressure,0xffff,0x0000,3);
       // drawNumber(8*3,5*16,returnvalue,0xffff,0x0000,3);

       // for (int i = 0; i < 8; i++) {
       //     if (touch_data[i] != 0)
       //         drawNumber(8*9,i*16,touch_data[i],0xffff,0x0000,3);
       // }


       // if (touch_data[1] == 2) {
       //     writesquare(0, 0, 240, 240, 0xffff);
       // } else {
       //     swipeUp = 0;
       // }


        if (nrf_gpio_pin_read(13)) {
            osRunning = 0;
        }

       // if (toggle) {
       //     writesquare(0, 0, 20, 20, 0x1A9F);
       // } else {
       //     writesquare(0, 0, 20, 20, 0xFFFF);
       // }
//        if (!nrf_gpio_pin_read(19)) {
//            writesquare(220, 220, 240, 240, 0x1A9F);
//    //        display_backlight(0);
//        } else {
//            writesquare(220, 220, 240, 240, 0xFFFF);
//            display_backlight(255);
//        }
        
        


    }

}
