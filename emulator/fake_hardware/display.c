#include "display.h"
#include <SDL2/SDL.h>



///// placeholder for actual brightness control see https://forum.pine64.org/showthread.php?tid=9378, pwm is planned
///void display_backlight(char brightness) {
///    nrf_gpio_cfg_output(LCD_BACKLIGHT_HIGH);	
///    if (brightness != 0) {
///        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,0);
///    } else {
///        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,1);
///    }
///}
///
///
///// handler that will be called when bytes are sent
///void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
///    spi_xfer_done = true;
///}
///
///// send one byte over spi
///void display_send(bool mode, uint8_t byte) {
///    nrf_gpio_pin_write(LCD_COMMAND,mode);
///    spi_xfer_done = false;
///
///    uint8_t m_tx_buf[1];         
///    m_tx_buf[0] = byte;
///
///    uint8_t m_length = sizeof(m_tx_buf); 
///
///    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);
///
///
///    while (!spi_xfer_done) {
///        __WFE();
///    }
///}
///
///// send a bunch of bytes from buffer
///void display_sendbuffer(bool mode, uint8_t* m_tx_buf, int m_length) {
///    spi_xfer_done = false;
///
///    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);
///
///    while (!spi_xfer_done) {
///        __WFE();
///    }
///}
///
///// send a bunch of bytes from buffer
///void display_sendbuffer_noblock(uint8_t* m_tx_buf, int m_length) {
///    spi_xfer_done = false;
///
///    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);
///
///}


static SDL_Event event;
static SDL_Renderer *renderer;
static SDL_Window *window;

void display_init() {

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(240, 240, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}



void drawSquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int area = (x2-x1+1)*(y2-y1+1);

    SDL_SetRenderDrawColor(renderer, ((color & 0xf800)*0xff)/0xf800,
            ((color & 0x07E0)*0xff)/0x07E0,
            ((color & 0x1f)*0xff)/0x1f, 255);

    SDL_Rect srcrect;

    srcrect.x = x1;
    srcrect.y = y1;
    srcrect.w = x2-x1;
    srcrect.h = y2-y1;

    SDL_FillRect(renderer, &srcrect,SDL_MapRGB(s->format, 255, 255, 255));
    SDL_RenderPresent(renderer);

}
//
//
//void drawBitmap (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t* bitmap) {
//    ppi_set();
//
//    int maxLength = 254; // TODO: this should be TXD.MAXCNT
//    uint8_t byteArray[maxLength];
//
//    /* setup display for writing */
//    byteArray[0] = CMD_CASET;
//
//    byteArray[1] = x1 >> 8;
//    byteArray[2] = x1 & 0xff;
//
//    byteArray[3] = x2 >> 8;
//    byteArray[4] = x2 & 0xff;
//
//    byteArray[5] = CMD_RASET;
//
//    byteArray[6] = y1 >> 8;
//    byteArray[7] = y1 & 0xff;
//
//    byteArray[8] = y2 >> 8;
//    byteArray[9] = y2 & 0xff;
//
//    byteArray[10] = CMD_RAMWR;
//    /**/
//
//
//    int areaToWrite;
//    int area = (x2-x1+1)*(y2-y1+1);
//
//    if (area > maxLength / 2 - 11)
//        areaToWrite = maxLength / 2 - 11;
//    else 
//        areaToWrite = area;
//
//
//    for (int i = 0; i < areaToWrite; i++) {
//        byteArray[i*2 + 11] = bitmap[i*2];
//        byteArray[i*2+1 + 11] = bitmap[i*2+1];
//    }
//
//    area -= areaToWrite;
//
//    display_sendbuffer(0, byteArray, (areaToWrite * 2)+11);
//
//    ppi_clr();
//
//    int offset = 0;
//
//    while (area > 0) {
//        offset += areaToWrite*2;
//
//        if (area > maxLength / 2)
//            areaToWrite = maxLength / 2;
//        else 
//            areaToWrite = area;
//
//        display_sendbuffer(0, bitmap+offset, areaToWrite * 2);
//        ppi_clr();
//        area -= areaToWrite;
//    }
//}
//
//
void drawMono(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t* frame, uint16_t posColor, uint16_t negColor) {
    //spi_xfer_done = true;

    int maxLength = 254; // TODO: this should be TXD.MAXCNT
    uint8_t byteArray0[maxLength];
    uint8_t byteArray1[maxLength];


    int area = (x2-x1+1)*(y2-y1+1);


    int pixel = 0;
    int byte = 11;
    int bytesToSend = byte + area*2;
    int packet = 0;

    while (area > pixel) {
        uint8_t* byteArray;
        if (packet % 2 == 0) 
            byteArray = byteArray0;
        else 
            byteArray = byteArray1;


        while (byte < maxLength - 1 && byte < bytesToSend) {
            uint16_t color = 0;

            if ((frame[pixel / 8] >> (pixel % 8)) & 1) {
                color = posColor;
            } else {
                color = negColor;
            }

            SDL_SetRenderDrawColor(renderer, ((color & 0xf800)*0xff)/0xf800,
                    ((color & 0x07E0)*0xff)/0x07E0,
                    ((color & 0x1f)*0xff)/0x1f, 255);

            int x = byte - 11 % (x2-x1+1);
            int y = byte - 11 / (x2-x1+1);
            SDL_RenderDrawPoint(renderer,x,y);

            byte++;
            byte++;

            pixel++;
        }




        bytesToSend -= byte;

        byte = 0;

        packet++;
        SDL_RenderPresent(renderer);
    }
}

//void scroll(uint16_t TFA, uint16_t VSA, uint16_t BFA, uint16_t scroll_value) {
//    /* set square to draw in */
//    display_send (0, CMD_VSCRDEF);
//
//    display_send (1,TFA >> 8);
//    display_send (1,TFA & 0xff);
//
//    display_send (1,VSA >> 8);
//    display_send (1,VSA & 0xff);
//
//    display_send (1,BFA >> 8);
//    display_send (1,BFA & 0xff);
//
//    display_send (0, CMD_MADCTL);
//    display_send (1, 0x0/*0x10*/);
//
//
//    display_send (0,CMD_VSCSAD);
//
//    display_send (1,scroll_value >> 8);
//    display_send (1,scroll_value & 0xff);
//
//    /**/
//}
//
//void partialMode(uint16_t PSL, uint16_t PEL) {
//    display_send (0, CMD_MADCTL);
//    display_send (1, 0x0/*0x10*/);
//
//    display_send (0, CMD_PTLAR);
//
//    display_send (1,PSL >> 8);
//    display_send (1,PSL & 0xff);
//
//    display_send (1,PEL >> 8);
//    display_send (1,PEL & 0xff);
//
//    display_send (0, CMD_PTLON);
//
//}
