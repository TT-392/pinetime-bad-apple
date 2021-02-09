#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define PIN_TXD (11)

void uart_send(char* data) {
    int i = 0;
    while(data[i] != '\0') {
        i++;
    }
    NRF_UARTE0->TXD.MAXCNT = i;//sizeof(data);
    NRF_UARTE0->TXD.PTR = (uint32_t)&data[0];
    NRF_UARTE0->TASKS_STARTTX = 1;

    while (NRF_UARTE0->EVENTS_ENDTX == 0);
    NRF_UARTE0->EVENTS_ENDTX = 0;

    NRF_UARTE0->TASKS_STOPTX = 1;
    while (NRF_UARTE0->EVENTS_TXSTOPPED == 0);

    NRF_UARTE0->EVENTS_TXSTOPPED = 0;

    static int x = 0;
    if (x == 0)
        nrf_delay_us(1);
    x = 1;
}

void uart_init() { 
    nrf_gpio_cfg_output(PIN_TXD);	


    NRF_UARTE0->CONFIG = (0 << UARTE_CONFIG_HWFC_Pos) |
        (0 << UARTE_CONFIG_PARITY_Pos);

    NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud9600 << UARTE_BAUDRATE_BAUDRATE_Pos;
    NRF_UARTE0->PSEL.TXD = PIN_TXD;
    NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;

    int x = 0;

}

void uart_send_number(int number) {
    if (number == 0) {
        char* numString = "0\n\r";
        uart_send(numString);
        return;
    }

    int numLength = 0;
    int tempNumber = number;
    while (tempNumber > 0) {
        tempNumber /= 10;
        numLength++;
    }
    char numString[numLength + 3];
    numString[numLength] = '\n';
    numString[numLength+1] = '\r';
    numString[numLength+2] = '\0';
    
    for (int i = numLength-1; i >= 0; i--) {
        numString[i] = '0' + number % 10;
        number /= 10;
    }
    uart_send(numString);
}

