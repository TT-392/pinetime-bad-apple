#include <stdint.h>
#include "../../src/display_defines.h"

#define bool _Bool
#define false 0
#define true 1
#define nrf_drv_spi_t int

#include <stdio.h>


#define __WFE() 0

typedef int nrf_drv_spi_config_t;
typedef struct
{
    uint8_t inst_idx;
    union
    {
#ifdef SPIM_PRESENT
        nrfx_spim_t spim;
#endif
#ifdef SPI_PRESENT
        nrfx_spi_t  spi;
#endif
    } u;
    bool    use_easy_dma;
} nrf_drv_spi_t;

int spi_xfer_done = 0;

bool commandMode;
uint8_t currentCommand = 0;
int dataCounter = 0;
uint16_t framememory[240][320] = {};
uint16_t currentDisplay[240][240] = {};
uint16_t display_x1,display_x2,display_y1,display_y2;

void drawFrame() {
    for (int x = 0; x < 240; x++) {
        for (int y = 0; y < 240; y++) {
            if (currentDisplay[x][y] != framememory[x][y]) {
                currentDisplay[x][y] = framememory[x][y];
                SDL_SetRenderDrawColor(renderer, ((currentDisplay[x][y] & 0xf800)*0xff)/0xf800,
                        ((currentDisplay[x][y] & 0x07E0)*0xff)/0x07E0,
                        ((currentDisplay[x][y] & 0x1f)*0xff)/0x1f, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }


    SDL_RenderPresent(renderer);
}


void display_byte_receive (uint8_t byte);

void nrf_gpio_pin_write(uint8_t pin, _Bool H_L) {
    if (pin == LCD_COMMAND) {
        commandMode = !H_L;
    }
}

void nrf_drv_spi_transfer(nrf_drv_spi_t* p_instance, uint8_t* p_tx_buffer, uint8_t tx_buffer_length, uint8_t * p_rx_buffer, uint8_t rx_buffer_length) {
    for (int i = 0; i < tx_buffer_length; i++) {
        display_byte_receive (p_tx_buffer[i]);
    }
}

void display_byte_receive (uint8_t byte) {
    if (commandMode) {
        currentCommand = byte;
        dataCounter = 0;
    } else {
        if (currentCommand == CMD_CASET) {
            switch (dataCounter) {
                case 0:
                    display_x1 = byte << 8;
                    break;
                case 1:
                    display_x1 |= byte;
                    break;
                case 2:
                    display_x2 = byte << 8;
                    break;
                case 3:
                    display_x2 |= byte;
                    break;
            }
        } else if (currentCommand == CMD_RASET) {
            switch (dataCounter) {
                case 0:
                    display_y1 = byte << 8;
                    break;
                case 1:
                    display_y1 |= byte;
                    break;
                case 2:
                    display_y2 = byte << 8;
                    break;
                case 3:
                    display_y2 |= byte;
                    break;
            }
        } else if (currentCommand == CMD_RAMWR) {
            
            // msb
            if (dataCounter % 2) {
                framememory[(dataCounter/2) % (display_x2 - display_x1+1)+display_x1][(dataCounter/2) / (display_x2 - display_x1+1) + display_y1] |= byte;
            } else {
                framememory[(dataCounter/2) % (display_x2 - display_x1+1)+display_x1][(dataCounter/2) / (display_x2 - display_x1+1) + display_y1] = byte << 8;
            }
            if (dataCounter == (display_x2 - display_x1 + 1) * (display_y2 - display_y1+1)*2-2)
                drawFrame();
        }
        dataCounter++;
    }
}
