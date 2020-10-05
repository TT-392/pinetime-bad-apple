#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "wdt.c"
#include "display.h"
#include "display_defines.h"
#include "modules/battery.c"
#include "test.c"
#include "frame.c"
#include "modules/steamLocomotive.h"

#include <math.h>

_Bool toggle = 0;

int time = 1;


struct Ball {
    uint8_t x;
    uint8_t y;
    int vx;
    int vy;
};

static int seed = 1;

int randnumber (void) {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed/65536) % 32768;

}

uint8_t blocksCache[8] = {0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f};

void render_breakout (uint8_t bat, struct Ball* ball) {
    static int firstRender = 1;
    uint8_t bw = 8;
    uint8_t step = 240/bw;
    uint8_t gaps = 2;
    uint8_t blHeight = 10;
    uint16_t color = 0xffff;
    uint8_t offset = 10;

    static int lastBat;
    int batWidth = 26;
    int batHeight = 7;
    if (bat > 239 - batWidth / 2)
        bat = 239 - batWidth / 2;
    if (bat < batWidth / 2)
        bat = batWidth / 2;

    int ballW = 6;
    int ballH = 4;

    uint8_t ball_x1 = ball->x + ball->vx;
    uint8_t ball_y1 = ball->y + ball->vy;
    uint8_t ball_x2 = ball_x1 + ballW;
    uint8_t ball_y2 = ball_y1 + ballH;

    uint8_t batX1 = bat - batWidth/2;
    uint8_t batY1 = 239 - batHeight-1;
    uint8_t batX2 = bat + batWidth/2-1;
    uint8_t batY2 = 239 - 2;


    drawSquare(batX1, batY1, batX2, batY2, 0xffff); 

    uint8_t removeX1;
    uint8_t removeWidth = fabs(bat - lastBat);
    if (bat > lastBat) {
        removeX1 = lastBat - batWidth/2;
    } else {
        removeX1 = lastBat + batWidth/2;
    }

    if (removeWidth > 0)
        drawSquare(removeX1, batY1, removeX1 + removeWidth - 1, batY2, 0x0000); 

    lastBat = bat;


    if (ball_x2 > batX1 && ball_x2 < batX2 && ball_y1 > batY1 && ball_y1 < batY2) 
        ball->vy = -ball->vy;




    if (firstRender) {
        for (int row = 0; row < 6; row++) {
            if (row < 2)
                color = 0xf800;
            else if (row < 4)
                color = 0xfc40;
            else 
                color = 0x47E0;

            for (int column = 0; column < bw; column++) {
                uint8_t x1 = column*step*1+gaps;
                uint8_t y1 = offset + blHeight*row + gaps;
                uint8_t x2 = step+column*step*1-gaps;
                uint8_t y2 = offset + blHeight*(row+1) - gaps;

                drawSquare(x1, y1, x2, y2, color);

            }
        }
        firstRender = 0;
    }
    for (int row = 0; row < 6; row++) {
        for (int column = 0; column < bw; column++) {
            if ((blocksCache[column] >> row) & 1) {
                uint8_t x1 = column*step*1+gaps;
                uint8_t y1 = offset + blHeight*row + gaps;
                uint8_t x2 = step+column*step*1-gaps;
                uint8_t y2 = offset + blHeight*(row+1) - gaps;


                int destroy = 0;
                if (ball_x2 > x1 && ball_x2 < x2 && ball_y1 > y1 && ball_y1 < y2)
                    destroy = 1;

                if (ball_x2 > x1 && ball_x2 < x2 && ball_y2 > y1 && ball_y2 < y2)
                    destroy = 1;

                if (ball_x1 > x1 && ball_x1 < x2 && ball_y1 > y1 && ball_y1 < y2)
                    destroy = 1;

                if (ball_x1 > x1 && ball_x1 < x2 && ball_y2 > y1 && ball_y2 < y2)
                    destroy = 1;


                if (destroy) {
                    drawSquare(x1, y1, x2, y2, 0x0000);
                    blocksCache[column] &= ~(1 << row);

                    int ballCenterX = ball_x1 + ballW/2;
                    int ballCenterY = ball_y1 + ballH/2;
                    // TODO: make bounce direction detection less shitty
                    if (ballCenterY > y1 && ballCenterY < y2) {
                        ball->vx = -ball->vx;
                    } else {
                        ball->vy = -ball->vy;
                    }
                }
            }
        }
    }




    if (ball->x + ball->vx < 0)
        ball->vx = -ball->vx;

    if (ball->y + ball->vy < 0)
        ball->vy = -ball->vy;

    if (ball->x + ballW-1 + ball->vx > 239)
        ball->vx = -ball->vx;

    if (ball->y + ballH-1 + ball->vy > 239)
        ball->vy = -ball->vy;



    uint8_t x1;
    uint8_t y1;
    uint8_t width;
    uint8_t height;

    // area to remove from the side
    width = fabs(ball->vx);

    if (width > 0) {
        if (ball->vx >= 0) {
            x1 = ball->x;
        } else {
            x1 = ball->x + ballW-1 + ball->vx + 1;
        }

        y1 = ball->y;
        height = ballH;
        drawSquare(x1, y1, x1 + width-1, y1 + height-1, 0x0000); 
    }

    // area to remove from the top/bottom
    height = fabs(ball->vy);

    if (height > 0) {
        if (ball->vy >= 0) {
            y1 = ball->y;
        } else {
            y1 = ball->y + ballH-1 + ball->vy + 1;
        }

        x1 = ball->x;
        width = ballW;
        drawSquare(x1, y1, x1 + width-1, y1 + height-1, 0x0000); 
    }


    ball->x += ball->vx;
    ball->y += ball->vy;

    drawSquare(ball->x, ball->y, ball->x+ballW-1, ball->y+ballH-1, 0xffff);
}



int main(void) {
    battery_init();
    bool osRunning = 1;

    nrf_gpio_cfg_output(10);
    nrf_gpio_pin_write(10,0);
    nrf_delay_ms(20);
    nrf_gpio_pin_write(10,1);
    nrf_delay_ms(20);

    display_backlight(255);
    display_init();

    struct Ball ball;
    ball.x = 120;
    ball.y = 120;
    ball.vx = 2;
    ball.vy = 1;


    drawSquare(0, 0, 239, 239, 0x0000);


    while (1) {
        render_breakout(ball.x, &ball);

        wdt_feed();
        nrf_delay_ms(10);
    };

    /// uint8_t bitmap [50*50*2] = {};
    /// for (int x = 0; x < 50*50*2; x++) {
    ///         bitmap[x] = (x%2)*0xff;
    /// }

    //drawBitmap(50,50,99,99,bitmap);
    // while (1) {
    //     //drawSquare(0, 0, 239, 239, 0xffff);
    //     //drawSquare(0, 0, 239, 239, 0xf800);
    //     //drawSquare(0, 0, 239, 239, 0xf800);
    //     drawMono(0, 0, 239, 239, frame, 0x0000, 0xffff);
    //     //display_init();
    //     wdt_feed();
    // }




    //    nrf_gpio_cfg_output(16);

    //  for (int i = 0; i < 240; i++) {
    //      drawSquare(i, 10+10*sin((float)i/10), i, 10+10*sin((float)i/10), 0xffff);
    //  }

    //    nrf_gpio_cfg_input(19, NRF_GPIO_PIN_NOPULL);
    //    nrf_gpio_pin_write(16,0);

    //   sl(65, 0xffff, 0x0000);
    while (!nrf_gpio_pin_read(13)) {
        while (!sl_nextFrameReady) {
            __WFI();
        }

        sl(65, 0xffff, 0x0000);
        wdt_feed();
        battery_percent(200,0,0xffff,0x0000);
    }
    //    nrf_gpio_pin_write(16,1);

    while(osRunning) {
        wdt_feed();

        display_backlight(0);

        drawSquare(0, 0, 239, 239, 0x0000);
        nrf_delay_ms(1000);

        //   if (nrf_gpio_pin_read(13)) {
        //       osRunning = 0;
        //   }
        //   display_backlight(255);
        //   nrf_delay_ms(100);


    }
}
