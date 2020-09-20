#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "display_defines.h"
#include "display.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(0);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

// placeholder for actual brightness control see https://forum.pine64.org/showthread.php?tid=9378, pwm is planned
void display_backlight(char brightness) {
    nrf_gpio_cfg_output(LCD_BACKLIGHT_HIGH);	
    if (brightness != 0) {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,0);
    } else {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,1);
    }
}


// handler that will be called when bytes are sent
void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
    spi_xfer_done = true;
}

// send one byte over spi
void display_send(bool mode, uint8_t byte) {
    nrf_gpio_pin_write(LCD_COMMAND,mode);
    spi_xfer_done = false;

    uint8_t m_tx_buf[1];         
    m_tx_buf[0] = byte;

    uint8_t m_length = sizeof(m_tx_buf); 

    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);


    while (!spi_xfer_done) {
        __WFE();
    }
}

// send a bunch of bytes from buffer
void display_sendbuffer(bool mode, uint8_t* m_tx_buf, int m_length) {
    nrf_gpio_pin_write(LCD_COMMAND,mode);
    spi_xfer_done = false;

    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);

    while (!spi_xfer_done) {
        __WFE();
    }
}


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
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin= NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.miso_pin = LCD_MISO;
    spi_config.mosi_pin = LCD_MOSI;
    spi_config.sck_pin  = LCD_SCK;
    spi_config.irq_priority  = APP_IRQ_PRIORITY_LOW;
    spi_config.frequency  = NRF_DRV_SPI_FREQ_8M;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    spi_config.mode=NRF_DRV_SPI_MODE_3; // Trailing edge clock, active low

    nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);


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
    display_send (1, 0x55);

	display_send (0, CMD_MADCTL); 
    display_send (1, 0x00);

	display_send (0, CMD_INVON); // for standard 16 bit colors
	display_send (0, CMD_NORON);
	display_send (0, CMD_DISPON);
}

void writesquare(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    /* set square to draw in */
    display_send (0, CMD_CASET);
 
    display_send (1,x1 >> 8);
    display_send (1,x1 & 0xff);

    display_send (1,x2 >> 8);
    display_send (1,x2 & 0xff);

    display_send (0,CMD_RASET);

    display_send (1,y1 >> 8);
    display_send (1,y1 & 0xff);

    display_send (1,y2 >> 8);
    display_send (1,y2 & 0xff);
    /**/

    /* prepare to write pixels */
    display_send (0,CMD_RAMWR);
    nrf_gpio_pin_write(LCD_COMMAND,1);
    /**/

    /* actually write the pixels */
    int pixelcount = 125; // amount of pixels to send per packet (maximum of 255/2)
    int screensize = (x2-x1+1)*(y2-y1+1);
    int packetcount = screensize / pixelcount;
    int overflow = screensize % pixelcount;

    for (int packet = 0; packet < (packetcount + (overflow > 0)); packet++) {
        if (packet == packetcount)
            pixelcount = overflow;

        uint8_t m_tx_buf[2 * pixelcount]; // 2 bytes per pixel

        int i = 0;
        for (int pixel = 0; pixel < pixelcount; pixel++) {
            m_tx_buf[i] = color >> 8;
            i++;
            m_tx_buf[i] = color;
            i++;
        }

        uint8_t m_length = sizeof(m_tx_buf); 


        display_sendbuffer(1,m_tx_buf,m_length);
    }
    /**/
}

void drawmono(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t* frame, uint16_t posColor, uint16_t negColor) {
    /* set square to draw in */
    display_send (0, CMD_CASET);
 
    display_send (1,0);
    display_send (1,x1);

    display_send (1,0);
    display_send (1,x2);

    display_send (0,CMD_RASET);

    display_send (1,0);
    display_send (1,y1);

    display_send (1,0);
    display_send (1,y2);
    /**/

    /* prepare to write pixels */
    display_send (0,CMD_RAMWR);
    nrf_gpio_pin_write(LCD_COMMAND,1);
    /**/

    /* actually write the pixels */
    int pixelcount = 125; // amount of pixels to send per packet (maximum of 255/2)
    int screensize = (x2-x1+1)*(y2-y1+1);
    int packetcount = screensize / pixelcount;
    int overflow = screensize % pixelcount;

    int pixelnumber = 0;
    for (int packet = 0; packet < (packetcount + (overflow > 0)); packet++) {
        if (packet == packetcount)
            pixelcount = overflow;

        uint8_t m_tx_buf[2 * pixelcount]; // 2 bytes per pixel

        int i = 0;
        for (int pixel = 0; pixel < pixelcount; pixel++) {
            uint16_t color;
            if ((frame[pixelnumber / 8] >> (pixelnumber % 8)) & 1) {
                color = posColor;
            } else {
                color = negColor;
            }

            m_tx_buf[i] = color >> 8;
            i++;
            m_tx_buf[i] = color;
            i++;
            pixelnumber++;
        }

        uint8_t m_length = sizeof(m_tx_buf); 


        display_sendbuffer(1,m_tx_buf,m_length);
    }
    /**/
}

void scroll(uint16_t TFA, uint16_t VSA, uint16_t BFA, uint16_t scroll_value) {
    /* set square to draw in */
    display_send (0, CMD_VSCRDEF);
 
    display_send (1,TFA >> 8);
    display_send (1,TFA & 0xff);

    display_send (1,VSA >> 8);
    display_send (1,VSA & 0xff);

    display_send (1,BFA >> 8);
    display_send (1,BFA & 0xff);

    display_send (0, CMD_MADCTL);
    display_send (1, 0x0/*0x10*/);


    display_send (0,CMD_VSCSAD);

    display_send (1,scroll_value >> 8);
    display_send (1,scroll_value & 0xff);

    /**/
}

void partialMode(uint16_t PSL, uint16_t PEL) {
    display_send (0, CMD_MADCTL);
    display_send (1, 0x0/*0x10*/);

    display_send (0, CMD_PTLAR);
 
    display_send (1,PSL >> 8);
    display_send (1,PSL & 0xff);

    display_send (1,PEL >> 8);
    display_send (1,PEL & 0xff);

    display_send (0, CMD_PTLON);

}

void tempFunction() {
    uint16_t x1 = 0;
    uint16_t y1 = 240-80;
    uint16_t x2 = 240;
    uint16_t y2 = 240;
    uint16_t color = 0x0000;
    /* set square to draw in */
    display_send (0, CMD_CASET);
 
    display_send (1,x1 >> 8);
    display_send (1,x1 & 0xff);

    display_send (1,x2 >> 8);
    display_send (1,x2 & 0xff);

    display_send (0,CMD_RASET);

    display_send (1,y1 >> 8);
    display_send (1,y1 & 0xff);

    display_send (1,y2 >> 8);
    display_send (1,y2 & 0xff);
    /**/

    /* prepare to write pixels */
    display_send (0,CMD_RAMWR);
    nrf_gpio_pin_write(LCD_COMMAND,1);
    /**/

    /* actually write the pixels */
    int pixelcount = 125; // amount of pixels to send per packet (maximum of 255/2)
    int screensize = (x2-x1+1)*(y2-y1+1);
    int packetcount = screensize / pixelcount;
    int overflow = screensize % pixelcount;

    for (int packet = 0; packet < (packetcount + (overflow > 0)); packet++) {
        if (packet == packetcount)
            pixelcount = overflow;

        uint8_t m_tx_buf[2 * pixelcount]; // 2 bytes per pixel

        int i = 0;
        for (int pixel = 0; pixel < pixelcount; pixel++) {
            color = ((pixel / 240) + 80)*6;
            m_tx_buf[i] = color >> 8;
            i++;
            m_tx_buf[i] = color;
            i++;
        }

        uint8_t m_length = sizeof(m_tx_buf); 


        display_sendbuffer(1,m_tx_buf,m_length);
    }
    /**/
}



