#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "display_defines.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

// placeholder for actual brightness control
void display_backlight(char brightness) {
    nrf_gpio_cfg_output(LCD_BACKLIGHT_HIGH);	
    if (brightness != 0) {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,0);
    } else {
        nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,1);
    }
}

int counter = 0;
void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
    spi_xfer_done = true;
  //  counter++;
  //  if (counter > 1000) {
  //  //    display_backlight(255);
  //  }

}


uint8_t display_send(bool mode, uint8_t byte) {
    nrf_gpio_pin_write(LCD_COMMAND,mode);
    spi_xfer_done = false;
    uint8_t r = 4;

    /**/
    uint8_t m_tx_buf[1];         
    m_tx_buf[0] = byte;

    uint8_t m_length = sizeof(m_tx_buf); 

    nrf_drv_spi_transfer(&spi, m_tx_buf, m_length, NULL, 0);


    while (!spi_xfer_done) {
        __NOP();
    }
    //nrf_delay_ms(10);
    /**/


    /*
	NRF_SPI0->EVENTS_READY=0;           // ready
	NRF_SPI0->TXD=byte;                 // out
    while(NRF_SPI0->EVENTS_READY==0){;} // wait
	r=NRF_SPI0->RXD;                    // in

    nrf_delay_ms(1);
	return (int)r;
    */
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
    /*
	NRF_SPI0->ENABLE=0;

	NRF_SPI0->PSELSCK=LCD_SCK;
	NRF_SPI0->PSELMOSI=LCD_MOSI;
	NRF_SPI0->PSELMISO=LCD_MISO;
	NRF_SPI0->FREQUENCY=SPI_FREQUENCY_FREQUENCY_M8;

	NRF_SPI0->CONFIG=(0x03 << 1);
	NRF_SPI0->EVENTS_READY=0;
	NRF_SPI0->ENABLE=(SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
    */

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin= NRF_DRV_SPI_PIN_NOT_USED;
    spi_config.miso_pin = LCD_MISO;
    spi_config.mosi_pin = LCD_MOSI;
    spi_config.sck_pin  = LCD_SCK;
    spi_config.irq_priority  = APP_IRQ_PRIORITY_LOW;
    spi_config.frequency  = NRF_DRV_SPI_FREQ_8M;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
    spi_config.mode=NRF_DRV_SPI_MODE_0;

    nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);
	NRF_SPI0->CONFIG=(0x03 << 1);




    ///////////////////
	// reset display //
    ///////////////////
	nrf_gpio_pin_write(LCD_RESET,0);
	nrf_delay_ms(200);
	nrf_gpio_pin_write(LCD_RESET,1);

	// prepare to send some commands
	nrf_gpio_pin_write(LCD_SELECT,0);

    display_send (0, CMD_SWRESET);
    display_send (0, CMD_SLPOUT);
    
    
    display_send (0, CMD_COLMOD);
    display_send (1, 0x55);

	display_send (0, CMD_MADCTL); 
    display_send (1, 0x00);

	display_send (0, CMD_INVON); 
	display_send (0, CMD_NORON); 
	display_send (0, CMD_DISPON); 
}

void writesquare(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) {
    display_send (0, CMD_CASET);
    
    display_send(1,0);
    display_send(1,x1);

    display_send(1,0);
    display_send(1,x2);

    display_send(0,CMD_RASET);

    display_send(1,0);
    display_send(1,y1);

    display_send(1,0);
    display_send(1,y2);


    display_send (0,CMD_RAMWR);  // write pixels

    for (int x=0; x < (x2-x1); x++) {
        for(int y=0;y< (y2-y1); y++) {
            display_send(1,(color)>>8);
            display_send(1,(color));
        }
    }
}



