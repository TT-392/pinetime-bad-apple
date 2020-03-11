#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "display_defines.h"


uint8_t display_send(bool mode, uint8_t byte) {
	nrf_gpio_pin_write(LCD_COMMAND,mode);
	uint8_t r;
	NRF_SPI0->EVENTS_READY=0;           // ready
	NRF_SPI0->TXD=byte;                 // out
    while(NRF_SPI0->EVENTS_READY==0){;} // wait
	r=NRF_SPI0->RXD;                    // in
	return (int)r;
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
	NRF_SPI0->ENABLE=0;

	NRF_SPI0->PSELSCK=LCD_SCK;
	NRF_SPI0->PSELMOSI=LCD_MOSI;
	NRF_SPI0->PSELMISO=LCD_MISO;
	NRF_SPI0->FREQUENCY=SPI_FREQUENCY_FREQUENCY_M8;

	NRF_SPI0->CONFIG=(0x03 << 1);
	NRF_SPI0->EVENTS_READY=0;
	NRF_SPI0->ENABLE=(SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);

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

// placeholder for actual brightness control
void display_backlight(char brightness) {
    nrf_gpio_cfg_output(LCD_BACKLIGHT_HIGH);	
    nrf_gpio_pin_write(LCD_BACKLIGHT_HIGH,0);
}


