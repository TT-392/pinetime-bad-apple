#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "display_defines.h"
#include "display.h"
#include "nrf_assert.h"
#include <string.h>

#define ppi_set() NRF_PPI->CHENSET = 0xff; __disable_irq();// enable first 8 ppi channels
#define ppi_clr() NRF_PPI->CHENCLR = 0xff; __enable_irq(); // disable first 8 ppi channels

#define COLOR_18bit 0x06
#define COLOR_16bit 0x05
#define COLOR_12bit 0x03
static uint8_t colorMode = COLOR_16bit;

// placeholder for actual brightness control see https://forum.pine64.org/showthread.php?tid=9378, pwm is planned
void display_backlight(char brightness) {
    nrf_gpio_cfg_output(LCD_BACKLIGHT_HIGH);
    if (brightness != 0) {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,0);
    } else {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,1);
    }
}

// send one byte over spi
void display_send(bool mode, uint8_t byte) {
    nrf_gpio_pin_write(LCD_COMMAND,mode);


    NRF_SPIM0->TXD.MAXCNT = 1;
    NRF_SPIM0->TXD.PTR = (uint32_t)&byte;

    NRF_SPIM0->EVENTS_ENDTX = 0;
    NRF_SPIM0->EVENTS_ENDRX = 0;
    NRF_SPIM0->EVENTS_END = 0;

    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_ENDTX == 0) __NOP();
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
    NRF_SPIM0->TASKS_STOP = 1;
    while (NRF_SPIM0->EVENTS_STOPPED == 0) __NOP();
}

// send a bunch of bytes from buffer
void display_sendbuffer(bool mode, uint8_t* m_tx_buf, int m_length) {
    NRF_SPIM0->TXD.MAXCNT = m_length;
    NRF_SPIM0->TXD.PTR = (uint32_t)m_tx_buf;

    NRF_SPIM0->EVENTS_ENDTX = 0;
    NRF_SPIM0->EVENTS_ENDRX = 0;
    NRF_SPIM0->EVENTS_END = 0;

    NRF_SPIM0->TASKS_START = 1;
    while(NRF_SPIM0->EVENTS_ENDTX == 0) __NOP();
    while(NRF_SPIM0->EVENTS_END == 0) __NOP();
    NRF_SPIM0->TASKS_STOP = 1;
    while (NRF_SPIM0->EVENTS_STOPPED == 0) __NOP();
}

// send a bunch of bytes from buffer
void display_sendbuffer_noblock(uint8_t* m_tx_buf, int m_length) {
    NRF_SPIM0->TXD.MAXCNT = m_length;
    NRF_SPIM0->TXD.PTR = (uint32_t)&m_tx_buf[0];

    NRF_SPIM0->EVENTS_ENDTX = 0;
    NRF_SPIM0->EVENTS_ENDRX = 0;
    NRF_SPIM0->EVENTS_END = 0;

    NRF_SPIM0->TASKS_START = 1;
}

// this function must be called after display_sendbuffer_noblock has been called
// and before the next call of spim related functions. It will wait for spim to
// finish and will then stop spim0
void display_sendbuffer_finish() {
    while(NRF_SPIM0->EVENTS_ENDTX == 0) 
        __NOP();

    while(NRF_SPIM0->EVENTS_END == 0) 
        __NOP();

    NRF_SPIM0->TASKS_STOP = 1;
    while (NRF_SPIM0->EVENTS_STOPPED == 0)
        __NOP();
}

void cmd_enable(bool enabled) {
    if (enabled) {
        // create GPIOTE task to switch LCD_COMMAND pin
        NRF_GPIOTE->CONFIG[1] = GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos |
            GPIOTE_CONFIG_POLARITY_Toggle << GPIOTE_CONFIG_POLARITY_Pos |
            LCD_COMMAND << GPIOTE_CONFIG_PSEL_Pos | 
            GPIOTE_CONFIG_OUTINIT_Low << GPIOTE_CONFIG_OUTINIT_Pos;
        ppi_set();
    } else {
        NRF_GPIOTE->CONFIG[1] = 0;
        ppi_clr();
        nrf_gpio_cfg_output(LCD_COMMAND);
    }
}

void set_colormode(uint8_t colormode) {
    cmd_enable(0);
    display_send (0, CMD_COLMOD);
    display_send (1, colormode);
    cmd_enable(1);
    colorMode = colormode;
}

void flip (bool flipped) {
    cmd_enable(0);
    if (flipped)
        display_send (0, CMD_INVON);
    if (!flipped)
        display_send (0, CMD_INVOFF);

    cmd_enable(1);
}

void cc_setup(int flip1, int flip2, int flip3, int flip4, int flip5, int flip6) {
    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    // offset from spim events started to first bit
    // cmd pin gets sampled at end of byte, so we flip it in the middle for better stability
    // 2 * 1 nibble = 8 = offset to get to middle of byte
    NRF_TIMER3->CC[0] = 5 + 8 + (8 * flip1 * 2);
    NRF_TIMER3->CC[1] = 5 + 8 + (8 * flip2 * 2);
    NRF_TIMER3->CC[2] = 5 + 8 + (8 * flip3 * 2);
    NRF_TIMER3->CC[3] = 5 + 8 + (8 * flip4 * 2);
    NRF_TIMER3->CC[4] = 5 + 8 + (8 * flip5 * 2);
    NRF_TIMER3->CC[5] = 5 + 8 + (8 * flip6 * 2);
} // TODO: I am pretty sure there is no reason to have the first flip at 0, this is a wasted CC and ppi


void display_init() {
    ////////////////
    // setup pins //
    ////////////////
    nrf_gpio_cfg_output(LCD_MOSI);
    nrf_gpio_cfg_output(LCD_SCK);
    nrf_gpio_cfg_input(LCD_MISO, NRF_GPIO_PIN_NOPULL);

    nrf_gpio_cfg_output(LCD_SELECT);
    nrf_gpio_cfg_output(LCD_COMMAND);
    nrf_gpio_cfg_output(LCD_RESET);

    nrf_gpio_pin_write(LCD_SELECT,1);
    nrf_gpio_pin_write(LCD_COMMAND,1);
    nrf_gpio_pin_write(LCD_RESET,1);


    ///////////////
    // spi setup //
    ///////////////
    NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos;
    NRF_SPIM0->PSEL.SCK  = LCD_SCK;
    NRF_SPIM0->PSEL.MOSI = LCD_MOSI;
    NRF_SPIM0->PSEL.MISO = LCD_MISO;

    NRF_SPIM0->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst  << SPIM_CONFIG_ORDER_Pos)|
                        (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                        (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);

    NRF_SPIM0->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8 << SPIM_FREQUENCY_FREQUENCY_Pos;
    NRF_SPIM0->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;




    ///////////////////
    // reset display //
    ///////////////////
    nrf_gpio_pin_write(LCD_RESET,0);
    nrf_delay_ms(200);
    nrf_gpio_pin_write(LCD_RESET,1);
    nrf_delay_ms(200);
    nrf_gpio_pin_write(LCD_SELECT,0);


    display_send (0, CMD_SWRESET);
    display_send (0, CMD_SLPOUT);

    display_send (0, CMD_COLMOD);
    display_send (1, COLOR_12bit);

    display_send (0, CMD_MADCTL); 
    display_send (1, 0x00);

    display_send (0, CMD_INVON); // for standard 16 bit colors
    display_send (0, CMD_NORON);
    display_send (0, CMD_DISPON);



    ///////////////////////////
    // setup LCD_COMMAND PIN //
    ///////////////////////////
    nrf_gpio_cfg_output(LCD_COMMAND);


    NRF_TIMER3->MODE = 0 << TIMER_MODE_MODE_Pos; // timer mode
    NRF_TIMER3->BITMODE = 0 << TIMER_BITMODE_BITMODE_Pos; // 16 bit
    NRF_TIMER3->PRESCALER = 0 << TIMER_PRESCALER_PRESCALER_Pos; // 16 MHz

    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    cc_setup(0,1,5,6,10,11);


    // PPI channels for toggeling pin
    for (int channel = 0; channel < 6; channel++) { 
        NRF_PPI->CH[channel].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[channel];
        if (channel % 2)
            NRF_PPI->CH[channel].TEP = (uint32_t) &NRF_GPIOTE->TASKS_SET[1];
        else 
            NRF_PPI->CH[channel].TEP = (uint32_t) &NRF_GPIOTE->TASKS_CLR[1];
    }


    NRF_PPI->CH[6].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;
    NRF_PPI->CH[6].TEP = (uint32_t) &NRF_TIMER3->TASKS_CLEAR;

    NRF_PPI->CH[7].EEP = (uint32_t) &NRF_SPIM0->EVENTS_STARTED;
    NRF_PPI->CH[7].TEP = (uint32_t) &NRF_TIMER3->TASKS_START;

    //  NRF_PPI->CH[8].EEP = (uint32_t) &NRF_TIMER3->EVENTS_COMPARE[5];
    //  NRF_PPI->CH[8].TEP = (uint32_t) &NRF_TIMER3->TASKS_STOP;

    cmd_enable(1);
}

void display_pause() {
    cmd_enable(0);
    nrf_gpio_pin_write(LCD_SELECT,1);
}
void display_resume() {
    cmd_enable(1);
    nrf_gpio_pin_write(LCD_SELECT,0);
}

void drawSquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    ppi_set();

    int maxLength = 254; 
    uint8_t byteArray [maxLength + 1];

    // addresses are offset by 1 to give the ability to recycle the array
    /* setup display for writing */
    byteArray[1] = CMD_CASET;

    byteArray[2] = x1 >> 8;
    byteArray[3] = x1 & 0xff;

    byteArray[4] = x2 >> 8;
    byteArray[5] = x2 & 0xff;

    byteArray[6] = CMD_RASET;

    byteArray[7] = y1 >> 8;
    byteArray[8] = y1 & 0xff;

    byteArray[9] = y2 >> 8;
    byteArray[10] = y2 & 0xff;

    byteArray[11] = CMD_RAMWR;
    /**/

    int area = (x2-x1+1)*(y2-y1+1);

    int areaToWrite;
    if (area > (maxLength - 11) / 2)
        areaToWrite = (maxLength - 11) / 2;
    else 
        areaToWrite = area;


    for (int i = 0; i < areaToWrite; i++) {
        byteArray[12+i*2] = color >> 8;
        byteArray[12+i*2+1] = color & 0xff;
    }

    area -= areaToWrite;

    // non blocking SPI here is negligible and unreliable cause stuff 
    // would be written memory while sending that same memory
    display_sendbuffer(0, byteArray + 1, (areaToWrite * 2)+11);
    ppi_clr();

    if (area > 0) {
        for (int i = 0; i < 6; i++) {
            byteArray[i*2] = color >> 8;
            byteArray[i*2+1] = color;
        }

        while (area > 0) {
            if (area > maxLength / 2)
                areaToWrite = maxLength / 2;
            else 
                areaToWrite = area;

            area -= areaToWrite;

            display_sendbuffer(0, byteArray, areaToWrite * 2);
        }
    }
}


void drawBitmap (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t* bitmap) {
    ppi_set();

    int maxLength = 254; 
    uint8_t byteArray[maxLength];

    /* setup display for writing */
    byteArray[0] = CMD_CASET;

    byteArray[1] = x1 >> 8;
    byteArray[2] = x1 & 0xff;

    byteArray[3] = x2 >> 8;
    byteArray[4] = x2 & 0xff;

    byteArray[5] = CMD_RASET;

    byteArray[6] = y1 >> 8;
    byteArray[7] = y1 & 0xff;

    byteArray[8] = y2 >> 8;
    byteArray[9] = y2 & 0xff;

    byteArray[10] = CMD_RAMWR;
    /**/


    int areaToWrite;
    int area = (x2-x1+1)*(y2-y1+1);

    if (area > maxLength / 2 - 11)
        areaToWrite = maxLength / 2 - 11;
    else 
        areaToWrite = area;


    for (int i = 0; i < areaToWrite; i++) {
        byteArray[i*2 + 11] = bitmap[i*2];
        byteArray[i*2+1 + 11] = bitmap[i*2+1];
    }

    area -= areaToWrite;

    display_sendbuffer(0, byteArray, (areaToWrite * 2)+11);

    ppi_clr();

    int offset = 0;

    while (area > 0) {
        offset += areaToWrite*2;

        if (area > maxLength / 2)
            areaToWrite = maxLength / 2;
        else 
            areaToWrite = area;

        display_sendbuffer(0, bitmap+offset, areaToWrite * 2);
        ppi_clr();
        area -= areaToWrite;
    }
}

void drawMono(int x1, int y1, int x2, int y2, uint8_t* frame, uint16_t posColor, uint16_t negColor) {
    ppi_set();

    int maxLength = 255; 
    uint8_t byteArray0[maxLength];
    uint8_t byteArray1[maxLength];

    /* setup display for writing */
    byteArray0[0] = CMD_CASET;

    byteArray0[1] = x1 >> 8;
    byteArray0[2] = x1 & 0xff;

    byteArray0[3] = x2 >> 8;
    byteArray0[4] = x2 & 0xff;

    byteArray0[5] = CMD_RASET;

    byteArray0[6] = y1 >> 8;
    byteArray0[7] = y1 & 0xff;

    byteArray0[8] = y2 >> 8;
    byteArray0[9] = y2 & 0xff;

    byteArray0[10] = CMD_RAMWR;
    /**/

    int area = (x2-x1+1)*(y2-y1+1);

    int byte = 11;
    int bitsppixel = 12;
    int pixel = -1;
    int bitsToWrite = 0;

    int packetNr = 0;

    uint8_t* byteArray = byteArray0;

    int bytesMod3 = 0;

    bool color;

    // 746
    bool stop = 0;
    //semihost_print("drawmono\n", 9);
    while (!stop) {
        uint8_t smallByte = 0;
        if (bytesMod3 == 0) {
            pixel++;
            if (pixel == area)
                break;

            color = (frame[pixel >> 3] >> (pixel % 8)) & 1;
            smallByte = color * 3;
            bytesMod3 = 1;
        } else if (bytesMod3 == 1) {
            smallByte = color * 2;

            pixel++;

            if (pixel == area) {
                stop = 1;
            } else {
                color = (frame[pixel >> 3] >> (pixel % 8)) & 1;
                smallByte |= color;
            }
            bytesMod3 = 2;
        } else if (bytesMod3 == 2) {
            smallByte = color * 3;
            bytesMod3 = 0;
        }

        switch (smallByte) {
            case 0:
                byteArray[byte] = 0xff;
                break;
            case 1:
                byteArray[byte] = 0xf0;
                break;
            case 2:
                byteArray[byte] = 0x0f;
                break;
            case 3:
                byteArray[byte] = 0x00;
                break;
        }
        byte++;

        if (byte == maxLength) {
            //semihost_print("packet\n", 7);
            if (packetNr > 0) {
                display_sendbuffer_finish();
                ppi_clr(); 
            }

            display_sendbuffer_noblock(byteArray, byte);

            packetNr++;
            if (packetNr % 2 == 0)
                byteArray = byteArray0;
            else
                byteArray = byteArray1;

            byte = 0;
        }
    }
    if (byte != 0) { 
        //semihost_print("Packet\n", 7);
        if (packetNr > 0) {
            display_sendbuffer_finish();
            ppi_clr(); 
        }

        display_sendbuffer_noblock(byteArray, byte);
    }
    display_sendbuffer_finish();
    ppi_clr();
}

void display_scroll(uint16_t TFA, uint16_t VSA, uint16_t BFA, uint16_t scroll_value) {
    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    cc_setup(0,1,7,8,9,10);
    ppi_set();

    uint8_t byteArray[12];
    byteArray[0] = CMD_VSCRDEF;

    byteArray[1] = TFA >> 8;
    byteArray[2] = TFA & 0xff;

    byteArray[3] = VSA >> 8;
    byteArray[4] = VSA & 0xff;

    byteArray[5] = BFA >> 8;
    byteArray[6] = BFA & 0xff;

    byteArray[7] = CMD_MADCTL;
    byteArray[8] = 0x0;

    byteArray[9] = CMD_VSCSAD;

    byteArray[10] = scroll_value >> 8;
    byteArray[11] = scroll_value & 0xff;

    display_sendbuffer(0, byteArray, 12);

    /**/

    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    cc_setup(0,1,5,6,10,11);
    ppi_clr();
}

void partialMode(uint16_t PSL, uint16_t PEL) {
    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    cc_setup(0,1,2,3,7,50);
    ppi_set();

    uint8_t byteArray[12];
    byteArray[0] = CMD_MADCTL;
    byteArray[1] = 0;

    byteArray[2] = CMD_PTLAR;

    byteArray[3] = PSL >> 8;
    byteArray[4] = PSL & 0xff;

    byteArray[5] = PEL >> 8;
    byteArray[6] = PEL & 0xff;

    byteArray[7] = CMD_PTLON;

    /**/

    // the following CC setup will cause byte 0, 5 and 10 
    // of any SPIM0 dma transfer to be treated as CMD bytes
    cc_setup(0,1,5,6,10,11);
    ppi_clr();
}
